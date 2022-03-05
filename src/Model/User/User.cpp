#include "User.h"
#include "Database/DbLogin.h"

bool User::isAdmin() const
{
	DbLogin db;
	return db.getAdminPremission(doctor.LPK, practice.rziCode);
}
