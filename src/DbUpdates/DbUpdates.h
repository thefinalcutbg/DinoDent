#pragma once

class UpdateDialog;

namespace DbUpdates
{
	void backupDatabase();
	void update5();
	void update6(UpdateDialog& d);
	void update7(UpdateDialog& d);
	
}