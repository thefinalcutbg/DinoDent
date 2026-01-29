#include "RqliteBackend.h"
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QTimer>
#include <QNetworkReply>
#include "View/ModalDialogBuilder.h"
#include "GlobalSettings.h"
#include <json/json.h>
#include "Network/crypto.h"
#include <QSaveFile>

QNetworkAccessManager* getDbManager() {

	static QNetworkAccessManager* s_manager = nullptr;

	if (s_manager) return s_manager;
	
	s_manager = new QNetworkAccessManager();
	
	return s_manager;
}

QUrl s_baseUrl;

void RqliteBackend::setSettings(const std::string& url)
{
    s_baseUrl = QUrl(url.c_str());
}

RqliteBackend::RqliteBackend()
{}

void RqliteBackend::resizeToIndex(int idx)
{
    if (idx <= 0) return;
    if (static_cast<int>(m_bindings.size()) < idx + 1) m_bindings.resize(idx + 1);

}

Json::Value RqliteBackend::toJsonValue(const BindVariant& v) const
{
    switch (v.index())
    {
        case 0: return Json::Value(Json::nullValue);
        case 1: return Json::Value(std::get<int>(v));
        case 2: return Json::Value(std::get<double>(v));
        case 3: return Json::Int64(static_cast<Json::Int64>(std::get<long long>(v)));
        case 4: return Json::Value(std::get<std::string>(v));
        case 5: { 
            const auto& bytes = std::get<std::vector<unsigned char>>(v);
            return Json::Value(Crypto::base64Encode(bytes));
        }
        default:
            Q_ASSERT(false);
            return Json::Value(Json::nullValue);
        }
}



void RqliteBackend::newStatement(const std::string& query)
{
    m_statement = query;
    m_bindings.clear();
    m_needsExec = true;
    m_rs.rowIndex = -1;
    m_rs.columns.clear();
    m_rs.rows.clear();
    rows_affected = 0;
    m_lastInsertedRowID = 0;
}

void RqliteBackend::bind(int index, const std::string& value)
{
    resizeToIndex(index);
    m_bindings[index] = value;
}

void RqliteBackend::bind(int index, int value)
{
    resizeToIndex(index);
    m_bindings[index] = value;
}

void RqliteBackend::bind(int index, double value)
{
    resizeToIndex(index);
    m_bindings[index] = value;
}

void RqliteBackend::bind(int index, long long value)
{
    resizeToIndex(index);
    m_bindings[index] = value;
}

void RqliteBackend::bind(int index, const std::vector<unsigned char>& blob)
{
    resizeToIndex(index);
    m_bindings[index] = blob;
}

void RqliteBackend::bindNull(int index)
{
    resizeToIndex(index);
    m_bindings[index] = std::monostate{};
}

bool RqliteBackend::hasRows()
{
    if (m_needsExec) { execute(); }

    if (m_rs.rows.empty()) { return false; }

    m_rs.rowIndex++;

    if (m_rs.rowIndex == static_cast<int>(m_rs.rows.size())) return false;
    
    return true;
}

int RqliteBackend::asInt(int column)
{
    const auto& v = m_rs.rows[m_rs.rowIndex][column];
    return v.isNull() ? 0 : v.asInt();
}

long long RqliteBackend::asRowId(int column)
{
    const auto& v = m_rs.rows[m_rs.rowIndex][column];
    return v.isNull() ? 0 : (long long)v.asInt64();
}

long long RqliteBackend::asLongLong(int column)
{
    const auto& v = m_rs.rows[m_rs.rowIndex][column];
    return v.isNull() ? 0 : (long long)v.asInt64();
}

bool RqliteBackend::asBool(int column)
{
    const auto& v = m_rs.rows[m_rs.rowIndex][column];
    return v.isNull() ? false : v.asBool();
}

double RqliteBackend::asDouble(int column)
{
    const auto& v = m_rs.rows[m_rs.rowIndex][column];
    return v.isNull() ? 0 : v.asDouble();
}

std::string RqliteBackend::asString(int column)
{
    const auto& v = m_rs.rows[m_rs.rowIndex][column];

    if (v.isNull()) return {};

    if (v.isString()) return v.asString();

    if (v.isBool())  return v.asBool() ? "1" : "0";

    if (v.isInt())   return std::to_string(v.asInt());

    if (v.isUInt())  return std::to_string(v.asUInt());

    if (v.isDouble()) return std::to_string(v.asDouble());

    return {};
}

std::vector<unsigned char> RqliteBackend::asBlob(int column)
{
    const auto& v = m_rs.rows[m_rs.rowIndex][column];
    return v.isNull() ? std::vector<unsigned char>{} : Crypto::base64Decode(v.asString());
}

int RqliteBackend::columnCount() const
{
    return m_rs.columns.size();
}

std::string RqliteBackend::columnName(int column) const
{
    if (column < 0 || column >= (int)m_rs.columns.size())
        return {};

    return m_rs.columns[column];
}

std::string RqliteBackend::getPreparedStatement() const
{
    Json::Value req(Json::arrayValue);
    Json::Value one(Json::arrayValue);

    one.append(m_statement);

    for (size_t i = 1; i < m_bindings.size(); ++i) {
        one.append(toJsonValue(m_bindings[i]));
    }

    req.append(one);

    return Json::FastWriter().write(req);
}

static bool isQuery(const std::string& sqlUtf8)
{
    QString s = QString::fromUtf8(sqlUtf8.c_str()).trimmed();
    if (s.isEmpty()) return false;

    if (s.startsWith(QString::fromLatin1("SELECT"), Qt::CaseInsensitive) ||
        s.startsWith(QString::fromLatin1("WITH"), Qt::CaseInsensitive) ||
        s.startsWith(QString::fromLatin1("EXPLAIN"), Qt::CaseInsensitive))
    {
        return true;
    }

    if (s.startsWith("PRAGMA", Qt::CaseInsensitive)) return !s.contains('=');

    return false;
}

bool RqliteBackend::execute()
{
    m_needsExec = false;

    bool queryEndpoint = isQuery(m_statement);

    Json::Value req(Json::arrayValue);
    Json::Value one(Json::arrayValue);

    one.append(m_statement);

    for (size_t i = 1; i < m_bindings.size(); ++i) {
        one.append(toJsonValue(m_bindings[i]));
    }

    req.append(one);

    auto requestBody = Json::FastWriter().write(req);

    const QByteArray body = QByteArray::fromStdString(requestBody);

    while (true)
    {
        const QUrl url = s_baseUrl.resolved(QUrl(queryEndpoint ? "/db/query" : "/db/execute"));

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                             QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply* reply = getDbManager()->post(request, body);

        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);

        bool timedOut = false;

        QObject::connect(&timer, &QTimer::timeout, &loop, [&]() {
            timedOut = true;
            reply->abort();
            loop.quit();
            });

        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

        timer.start(8000);
        loop.exec(QEventLoop::ExcludeUserInputEvents);

        const QByteArray responseBody = reply->readAll();

        int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        bool httpOk = (httpCode >= 200 && httpCode < 300);
        bool netOk = (reply->error() == QNetworkReply::NoError);

        reply->deleteLater();

        if (timedOut || !netOk || !httpOk) {
            ModalDialogBuilder::showError("Неуспешна връзка с базата данни");
            return false;
        }

        Json::Value root;
        Json::Reader reader;

        if (!reader.parse(responseBody.constData(), responseBody.constData() + responseBody.size(), root, false)) {
             return false;

        }

        if (!root.isMember("results") || root["results"].empty()) return false;

        const Json::Value& replyValue = root["results"][0];

      
        if (replyValue.isMember("error")) {
            ModalDialogBuilder::showError(replyValue["error"].asString());
            return false;
        }

        if (queryEndpoint)
        {
            if (replyValue.isMember("columns")) {
                for (const auto& c : replyValue["columns"])
                    m_rs.columns.push_back(c.asString());
            }

            if (replyValue.isMember("values") && replyValue["values"].isArray()) {
              
                for (const auto& row : replyValue["values"]) {

                    std::vector<Json::Value> outRow;

                    outRow.reserve((size_t)row.size());

                    for (const auto& cell : row) outRow.push_back(cell);

                    m_rs.rows.push_back(std::move(outRow));
                }
            }

            return true;
        }
        else
        {
            if (replyValue.isMember("rows_affected")) {

                    rows_affected = (int)replyValue["rows_affected"].asInt64();
            }

            if (replyValue.isMember("last_insert_id")) { m_lastInsertedRowID = replyValue["last_insert_id"].asInt64(); }

            return true;
        }
    }
}

bool RqliteBackend::backup()
{
    QString outPath = QString::fromStdString(GlobalSettings::getDbBackupFilepath());

    QUrl url = s_baseUrl.resolved(QUrl("/db/backup"));

    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/octet-stream");

    QNetworkReply* reply = getDbManager()->get(request);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    bool timedOut = false;

    QObject::connect(&timer, &QTimer::timeout, &loop, [&]() {
        timedOut = true;
        reply->abort();
        loop.quit();
        });

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    timer.start(8000);
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    bool httpOk = (httpCode >= 200 && httpCode < 300);
    bool netOk = (reply->error() == QNetworkReply::NoError);

    QByteArray body = reply->readAll();
    QString errStr = reply->errorString();

    reply->deleteLater();

    if (timedOut || !netOk || !httpOk || body.isEmpty()) {
        return false;
    }

    QSaveFile file(outPath);

    if (!file.open(QIODevice::WriteOnly) ||
        file.write(body) != body.size() ||
        !file.commit())
    {
        return false;
    }

    return true;
}

bool RqliteBackend::execute(const std::string& query)
{
    newStatement(query);
    m_needsExec = false;
    return execute();
}
