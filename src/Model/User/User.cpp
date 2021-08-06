#include "User.h"

std::string User::getName() const
{
    std::string name{ u8"д-р " };
    name += fName;
    name += " ";
    name += lName;
    return name;
}
