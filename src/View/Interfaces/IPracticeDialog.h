#pragma once

#include <string>
#include <optional>

class IAddPracticeDialog
{
public:

    struct Data{
        std::string name;
        std::string rzi;
        std::string address;
    };

    virtual std::optional<Data> getData() = 0;
};
