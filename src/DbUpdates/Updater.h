#pragma once

class DbBackend;

namespace DbUpdater
{
	void updateDb(DbBackend* backend_ptr);
}

class UpdateDialog;

namespace DbUpdates
{
	void update10(UpdateDialog& d, DbBackend* backend_ptr);
	void update11(UpdateDialog& d, DbBackend* backend_ptr);
	void update16(DbBackend* backend_ptr);
	void update17(DbBackend* backend_ptr);
	void update18(DbBackend* backend_ptr);
	void update19(UpdateDialog& d, DbBackend* backend_ptr);
	void update26(UpdateDialog& d, DbBackend* backend_ptr);
	void update36(UpdateDialog& d, DbBackend* backend_ptr);

	void commonUpdate(int toVersion, DbBackend* backend_ptr);
}