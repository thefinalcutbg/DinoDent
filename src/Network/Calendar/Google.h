#pragma once

#include <string>
#include <QString>

class CalendarPresenter;

namespace Google
{
	void setReciever(CalendarPresenter* p);
	void grantAccess(const std::string& refreshToken = {});
	void query(
		const QString& url,
		const QVariantMap& param,
		const QString& verb,
		const QString& requestBody,
		int callbackNumber
	);
}