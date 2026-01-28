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

bool Db::testConnection()
{
    std::unique_ptr<DbBackend> db_test;

    if (s_settings.mode == DbSettings::DbType::Rqlite) {
        db_test = std::make_unique<RqliteBackend>();
    }
    else {
        db_test = std::make_unique<SqliteBackend>();
    }

    auto ver = version(db_test.get());

    if (ver == Version::dbVersion()) return true;

    //no connection
    while (ver == -1) {
        DbSettingsDialog d(s_settings);

        auto result = d.getResult();

        if (!result) return false;

        setSettings(result.value());

        if (s_settings.mode == DbSettings::DbType::Rqlite) {
            db_test = std::make_unique<RqliteBackend>();
        }
        else {
            db_test = std::make_unique<SqliteBackend>();
        }

        ver = version(db_test.get());

        //saving the settings, since the database is connected
        if (ver != -1) {
            GlobalSettings::setDbSettings(result.value());
        }
        else {
            ModalDialogBuilder::showMessage("Неуспешна връзка с базата данни");
        }
    };

    //no schema at all
    if (ver == 0) {

        for (auto& tableSchema : Resources::dbSchema()) {
            if (!db_test->execute(tableSchema)) return false;
        }
    }

    //higher db version.
    if (ver > Version::dbVersion()) {
        ModalDialogBuilder::showMessage(
            "Версията на базата данни е по-нова от тази, която се поддържа от програмата. "
            "Задължително актуализирайте софтуера до най-последна версия, преди да го използвате!"
        );

        return false;
    }

    //lower db version - needs migration
    if (ver < Version::dbVersion()) { DbUpdater::updateDb(); }

    return true;;
}

Db::Db()
{
    if (s_settings.mode == DbSettings::DbType::Rqlite && !testConnection()) {
        throw std::runtime_error("Error initializing database");
    }

    if (s_settings.mode == DbSettings::DbType::Rqlite) {
        m_backend = std::make_unique<RqliteBackend>();
    }
    else {
        m_backend = std::make_unique<SqliteBackend>();
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

void Db::setVersion(int version)
{
    Db::crudQuery("PRAGMA user_version =" + std::to_string(version));
}

bool Db::crudQuery(const std::string& query)
{
    Db db(query);
    return db.execute();
}

void Db::setSettings(const DbSettings& settings)
{
    SqliteBackend::setFilePath(settings.sqliteFilePath);
    RqliteBackend::setSettings(settings.rqliteUrl);

    s_settings = settings;
}
