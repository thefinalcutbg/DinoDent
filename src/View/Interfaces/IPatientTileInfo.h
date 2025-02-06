#pragma once

struct Patient;
struct PatientInfoPresenter;
class IPatientTileInfo
{
public:
	virtual void setPatient(const Patient& p, int age) = 0;
	virtual void setPresenter(PatientInfoPresenter* p) = 0;
	
};