#include "User.h"

User User::_instance;

User& User::instance()
{
    return _instance;
}
