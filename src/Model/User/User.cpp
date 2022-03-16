#include "User.h"
#include "Database/DbDoctor.h"

bool User::isAdmin() const
{
	return DbDoctor::getAdminPremission(doctor.LPK, practice.rziCode);
}
