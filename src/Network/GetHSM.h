#pragma once

#include "PKCS11.h"
#include <optional>

//small helper function

namespace GetHSM {

	std::optional<PKCS11> get();
	void setMultiPKCS11(bool multi);
}