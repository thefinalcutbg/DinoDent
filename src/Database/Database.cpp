#include "Database.h"

#include "DbBackend.h"
#include "Resources.h"
#include "SqliteBackend.h"
#include "RqliteBackend.h"
#include "src/Version.h"
#include "View/ModalDialogBuilder.h"
#include <stdexcept>
#include "DbUpdates/Updater.h"
#include "View/Widgets/DbSettingsDialog.h"
#include "GlobalSettings.h"
#include "Network/UpdateService/UpdateService.h"
#include "Model/FreeFunctions.h"
#include "Model/User.h"

std::optional<DbSettings> Db::setupConnection(const DbSettings& s)
{
    std::unique_ptr<DbBackend> db_test;

    auto settings = s;

    if (settings.mode == DbSettings::DbType::Rqlite) {
        db_test = std::make_unique<RqliteBackend>(settings.rqliteUrl);
    }
    else {

        if (!SqliteBackend::createDirPath(settings.sqliteFilePath)) {
            ModalDialogBuilder::showError("Неуспешно създаване на директорията на базата данни");
            return {};
        }

        db_test = std::make_unique<SqliteBackend>(settings.sqliteFilePath);
    }

    auto ver = version(db_test.get());

    if (ver == Version::dbVersion()) return settings;
    
    //no connection
    while (ver == -1) {
        DbSettingsDialog d(settings);

        auto result = d.getResult();

        if (!result) return {};

        settings = result.value();

        if (settings.mode == DbSettings::DbType::Rqlite) {
            db_test = std::make_unique<RqliteBackend>(settings.rqliteUrl);
        }
        else {
            db_test = std::make_unique<SqliteBackend>(settings.sqliteFilePath);
        }

        ver = version(db_test.get());
    };

    //no schema at all - creating db structure
    if (ver == 0) {

        for (auto& tableSchema : Resources::dbSchema()) {
            if (!db_test->execute(tableSchema)) return {};
        }
    }

    //higher db version - needs update of the binary
    if (ver > Version::dbVersion()) {
        ModalDialogBuilder::showMessage(
            "Версията на базата данни е по-нова от тази, която се поддържа от програмата. "
            "Задължително актуализирайте софтуера до най-последна версия, преди да го използвате!"
        );

        UpdateService::restartForUpdate(true);

        FreeFn::terminateApplication(0);

        return {};
    }

    //lower db version - needs migration
    if (ver < Version::dbVersion()) {
        
        s_settings = settings;

        if (!db_test->backup()) {
            ModalDialogBuilder::showError("Неуспешно архивиране на базата данни");
            return {};
        }

        DbUpdater::updateDb(db_test.get());
    }

    return settings;
}

Db::Db()
{
    auto settings = setupConnection(s_settings);

    if (!settings.has_value()) { 
        ModalDialogBuilder::showError("Неуспешно конфигуриране на базата данни"); 
        throw std::runtime_error("");
    }

    bool dbTypeChanged = User::doctor().LPK.size() && settings->mode != s_settings.mode;

    if (s_settings != settings.value()) {
        s_settings = settings.value();
        GlobalSettings::setDbSettings(settings.value());

        if (dbTypeChanged) {
            ModalDialogBuilder::showMessage("Видът на базата данни е променен. Програмата ще се рестартира");
            FreeFn::restartApplication();
            return;
        }
    }

    if (s_settings.mode == DbSettings::DbType::Rqlite) {
        m_backend = std::make_unique<RqliteBackend>(s_settings.rqliteUrl);
    }
    else {
        m_backend = std::make_unique<SqliteBackend>(s_settings.sqliteFilePath);
    }
}

Db::Db(const std::string& query) : Db()
{
    m_backend->newStatement(query);
}

Db::~Db()
{}

bool Db::hasRows() { return m_backend->hasRows(); }

int Db::asInt(int column) { return m_backend->asInt(column); }

long long Db::asRowId(int column) { return m_backend->asRowId(column); }

long long Db::asLongLong(int column) { return m_backend->asLongLong(column); }

bool Db::asBool(int column) { return m_backend->asBool(column); }

double Db::asDouble(int column) { return m_backend->asDouble(column); }

std::string Db::asString(int column) { return m_backend->asString(column); }

std::vector<unsigned char> Db::asBlob(int column) { return m_backend->asBlob(column); }

void Db::newStatement(const std::string& query) { m_backend->newStatement(query); }

bool Db::execute(const std::string& query) { return m_backend->execute(query); }

int Db::columnCount() const { return m_backend->columnCount(); }

std::string Db::columnName(int column) const { return m_backend->columnName(column); }

int Db::rowsAffected() const { return m_backend->rowsAffected(); }

long long Db::lastInsertedRowID() { return m_backend->lastInsertedRowID(); }

std::string Db::getPreparedStatement() const { return m_backend->getPreparedStatement(); }

void Db::bind(int index, const std::string& value) { m_backend->bind(index, value); }

void Db::bind(int index, int value) { m_backend->bind(index, value); }

void Db::bind(int index, double value) { m_backend->bind(index, value); }

void Db::bind(int index, long long value) { m_backend->bind(index, value); }

void Db::bind(int index, const std::vector<unsigned char>& blob) { m_backend->bind(index, blob); }

void Db::bindNull(int index) { m_backend->bindNull(index); }

void Db::backup()
{
    m_backend->backup();
}

bool Db::execute() { 

    if (GlobalSettings::showDbDebugEnabled()) { ModalDialogBuilder::showMultilineDialog(getPreparedStatement()); }

    return m_backend->execute(); 
}

int Db::version()
{
    return version(m_backend.get());
}

int Db::version(DbBackend* const backend)
{
    backend->newStatement("PRAGMA user_version");
    while (backend->hasRows()) return backend->asInt(0);
    return -1;
}

bool Db::crudQuery(const std::string& query)
{
    Db db(query);
    return db.execute();
}

void Db::setSettings(const DbSettings& settings)
{
    s_settings = settings;
}
