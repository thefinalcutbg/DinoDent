#pragma once
#include <string>
#include "Model/Date.h"
#include "Model/DynamicNum.h"
namespace DbUpdateStatus {

	void setAsUpdated(DynamicNum num);
	Date lastUpdated(DynamicNum num);

}