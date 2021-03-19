#pragma once
#include <QDebug>
enum class Notification { edited, validID, cityInfoChanged };

class uiObserver
{
	bool listening;
protected:
	void turnOnNotifications() { listening = true; }
	void turnOffNotifications() { listening = false; }

	virtual void handleNotifications(Notification notification) = 0;
public:
	uiObserver() : listening(true) {}
	void notify(Notification notification) 
		{
			//if (listening == false) return;

			handleNotifications(notification); 
		}
};