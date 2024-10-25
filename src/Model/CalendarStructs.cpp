#include "CalendarStructs.h"
#include "Model/Patient.h"

CalendarEvent::CalendarEvent(const Patient& p)
{
	summary = p.firstLastName();
	if (p.phone.size()) {
		summary += " ";
		summary += p.phone;
	}
}
