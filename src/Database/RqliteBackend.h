#pragma once

#include "DbBackend.h"
#include "Model/GlobalSettingsData.h"
#include <variant>
#include <json/json.h>
#include <QUrl>

namespace Json { class Value; }
class QString;

class RqliteBackend final : public DbBackend
{	

    using BindVariant = std::variant<
        std::monostate, 
        int,
        double,
        long long,
        std::string,
        std::vector<unsigned char>
    >;

    struct RqliteResultSet {
        std::vector<std::string> columns;
        std::vector<std::vector<Json::Value>> rows;
        int rowIndex = -1;
        bool hasRows() const { return !rows.empty(); }
    };

    QUrl baseUrl;
    QString usr;
    QString pass;

    std::vector<BindVariant> m_bindings;
    std::string m_statement;
    RqliteResultSet m_rs;
    bool m_needsExec = false;
    long long m_lastInsertedRowID{ 0 };
    long long rows_affected{ 0 };

    void resizeToIndex(int idx);
    Json::Value toJsonValue(const BindVariant& v) const;

public:
    RqliteBackend(const std::string& url, const std::string& usr, const std::string& pass);

    bool hasRows() override;

    bool execute(const std::string& query) override;
    bool execute() override;

    void newStatement(const std::string& query) override;

    void bind(int index, const std::string& value) override;
    void bind(int index, int value) override;
    void bind(int index, double value) override;
    void bind(int index, long long value) override;
    void bind(int index, const std::vector<unsigned char>& blob) override;
    void bindNull(int index) override;


    int asInt(int column) override;
    long long asRowId(int column) override;
    long long asLongLong(int column) override;
    bool asBool(int column) override;
    double asDouble(int column) override;
    std::string asString(int column) override;
    std::vector<unsigned char> asBlob(int column) override;

    int columnCount() const override;
    std::string columnName(int column) const override;
    int rowsAffected() const override { return rows_affected; }
    long long lastInsertedRowID() override { return m_lastInsertedRowID; }
    std::string getPreparedStatement() const override;

    bool backup() override;

    virtual ~RqliteBackend() {};
};

