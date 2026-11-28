#include "BZSRegisterService.h"
#include "NetworkManager.h"

#include <TinyXML/tinyxml.h>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <utility>

bool BZSRegisterService::sendRequest(const std::vector<std::string>& lpks,

    std::function<void(const std::map<std::string, std::string>&)> callback)
{
    if (m_callback || !callback) return false;

    m_lpks = lpks;
    m_results.clear();
    m_next = 0;

    m_callback = std::move(callback);
    queryNext();
    return true;
}

void BZSRegisterService::queryNext()
{
    while (true) {
        if (m_next >= m_lpks.size()) break;

        if (!m_lpks[m_next].empty() && !m_results.count(m_lpks[m_next])) break;

        m_next++;
    }

    if (m_next == m_lpks.size()) {
        auto callback = std::move(m_callback);
        m_callback = nullptr;
        auto results = std::move(m_results);
        m_lpks.clear();

        callback(results);
        return;
    }

    awaiting_reply = true;
    show_dialogs = false;

    NetworkManager::sendRequestToBzsRegister(m_lpks[m_next++], this);
}

void BZSRegisterService::parseReply(const std::string& reply)
{
    if (!m_callback || m_next == 0) return;
    
    auto& lpk = m_lpks[m_next - 1];

    const std::function<QString(const TiXmlNode*)> nodeText = [&nodeText](const TiXmlNode* node) {
        if (!node) return QString{};
        if (node->ToText()) return QString::fromUtf8(node->Value());
        QString text;
        for (auto child = node->FirstChild(); child; child = child->NextSibling()) {
            // TinyXML condenses whitespace at text-node boundaries.
            text += nodeText(child) + ' ';
        }
        return text;
    };

    // The surrounding page is HTML, not XML. Extract only the name and first
    // table row, excluding unclosed HTML tags and scripts before TinyXML parsing.
    static const QRegularExpression resultPattern(
        R"((<span\b[^>]*\bclass\s*=\s*["']text3["'][^>]*>.*?</span>)\s*<br\s*/?>\s*<div\b[^>]*>\s*<table\b[^>]*>\s*(<tr\b[^>]*>.*?</tr>))",
        QRegularExpression::DotMatchesEverythingOption);
    auto matches = resultPattern.globalMatch(QString::fromStdString(reply));

    while (matches.hasNext()) {
        auto match = matches.next();
        auto fragment = "<result>" + match.captured(1) + match.captured(2) + "</result>";
        fragment.replace("&nbsp;", "&#160;");
        const auto xml = fragment.toStdString();

        TiXmlDocument doc;
        doc.Parse(xml.c_str(), 0, TIXML_ENCODING_UTF8);
        if (doc.Error() || !doc.RootElement()) continue;

        TiXmlHandle result(&doc);
        auto nameElement = result.FirstChildElement("result").FirstChildElement("span").ToElement();
        auto row = result.FirstChildElement("result").FirstChildElement("tr");
        auto labelElement = row.ChildElement("td", 0).ToElement();
        auto lpkElement = row.ChildElement("td", 1).ToElement();
        if (!nameElement || !labelElement || !lpkElement) continue;

        if (nodeText(labelElement).simplified() != QString::fromUtf8("ЛПК") ||
            nodeText(lpkElement).simplified().toStdString() != lpk) continue;

        auto name = nodeText(nameElement).simplified();
        if (name.startsWith(QString::fromUtf8("Д-р "), Qt::CaseInsensitive)) name.remove(0, 4);
        const auto parts = name.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2) continue;
        m_results.emplace(lpk, (parts.first() + ' ' + parts.last()).toStdString());
        break;
    }
    queryNext();
}
