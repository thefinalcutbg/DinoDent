#include "User.h"

CurrentUser CurrentUser::_instance;

CurrentUser& CurrentUser::instance()
{
    return _instance;
}
