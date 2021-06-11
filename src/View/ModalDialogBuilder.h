#pragma once

class ProcedureDialogPresenter;

class ModalDialogBuilder
{
	static ModalDialogBuilder _instance;

	ModalDialogBuilder();

public:
	ModalDialogBuilder(const ModalDialogBuilder&) = delete;

	void openDialog(ProcedureDialogPresenter* p);
	static ModalDialogBuilder& instance();
};

