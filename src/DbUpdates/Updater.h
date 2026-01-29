#pragma once

namespace DbUpdater
{
	void updateDb();
}

class UpdateDialog;

namespace DbUpdates
{
	void update10(UpdateDialog& d);
	void update11(UpdateDialog& d);
	void update16();
	void update17();
	void update18();
	void update19(UpdateDialog& d);
	void update26(UpdateDialog& d);
	void update36(UpdateDialog& d);

	void commonUpdate(int toVersion);
}