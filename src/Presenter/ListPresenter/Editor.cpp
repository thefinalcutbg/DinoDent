#include "Editor.h"

void Editor::attachEditObserver(EditObserver* observer)
{
	edit_observer = observer;
}

void Editor::makeEdited()
{
	if (edit_observer != nullptr)
		edit_observer->notify();
}
