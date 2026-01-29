#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "Model/GlobalSettingsData.h"

class DbBackend;

class Db
{
    int error_count{ 0 };

    std::unique_ptr<DbBackend> m_backend;

    static inline DbSettings s_settings;

    static inline bool s_showErrorDialog{ false };

    static int version(DbBackend* const backend);

public:

    static void setShowErrors(bool show) { s_showErrorDialog = show; }
    static void setVersion(int version);
    static bool crudQuery(const std::string& query);
    static void setSettings(const DbSettings& settings);
    static std::optional<DbSettings> setupConnection(const DbSettings& settings);

public:

    Db();
    Db(const std::string& query);
    int version();
    bool hasRows();
    int asInt(int column);
    long long asRowId(int column);
    long long asLongLong(int column);
    bool asBool(int column);
    double asDouble(int column);
    std::string asString(int column);
    std::vector<unsigned char> asBlob(int column);

    void newStatement(const std::string& query);
    bool execute(const std::string& query);
    int columnCount() const;
    std::string columnName(int column) const;

    int rowsAffected() const;
    long long lastInsertedRowID();
    std::string getPreparedStatement() const;

    void bind(int index, const std::string& value);
    void bind(int index, int value);
    void bind(int index, double value);
    void bind(int index, long long value);
    void bind(int index, const std::vector<unsigned char>& blob);
    void bindNull(int index);

    void backup();

    bool execute();

    ~Db();
};