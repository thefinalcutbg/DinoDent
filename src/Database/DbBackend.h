#pragma once

#include <string>
#include <vector>

class DbBackend
{

public:

    //returns true if there are more rows to get from database
    virtual bool hasRows() = 0;
    virtual int asInt(int column) = 0;
    virtual long long asRowId(int column) = 0;
    virtual long long asLongLong(int column) = 0;
    virtual bool asBool(int column) = 0;
    virtual double asDouble(int column) = 0;
    virtual std::string asString(int column) = 0;
    virtual std::vector<unsigned char> asBlob(int column) = 0;
    virtual void newStatement(const std::string& query) = 0;
    virtual bool execute(const std::string& query) = 0;
    virtual int columnCount() const = 0;
    virtual std::string columnName(int column) const = 0;
    virtual int rowsAffected() const = 0;
    virtual long long lastInsertedRowID() = 0;
    virtual std::string getPreparedStatement() const = 0;

    virtual void bind(int index, const std::string& value) = 0;
    virtual void bind(int index, int value) = 0;
    virtual void bind(int index, double value) = 0;
    virtual void bind(int index, long long value) = 0;
    virtual void bind(int index, const std::vector<unsigned char>& blob) = 0;
    virtual void bindNull(int index) = 0;

    virtual bool execute() = 0;

    virtual ~DbBackend() = default;

};

