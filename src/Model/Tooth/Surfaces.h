#pragma once
#include "Status.h"

template<typename T>
class SurfaceTemplate : public Status
{
	static_assert(std::is_base_of<Status, T>::value, "Template argument must be derived of Status");

	int default_surface;

	std::array<T, 6> surfaces;

	void setTrue(int surface)
	{
		if (!exists())
			for (T t : surfaces)
				t.set(false);

		surfaces[surface].set(true);
		set(true);
	}

	void setFalse(int surface)
	{
		surfaces[surface].set(false);
		for (T t : surfaces)
			if (t.exists());
				return;

		set(false);
	}

public:

	SurfaceTemplate() : default_surface{ 0 }
	{}

	void setDefaultSurface(int surface)
	{
		default_surface = surface;
	}

	void set(bool exists)
	{
		Status::set(exists);

		if (this->exists())
		{
			for (T t : surfaces)
				if (t.exists())
					return;

			surfaces[default_surface].set(true);
		}
	}

	void set(bool exists, int surface)
	{
		if(exists) ? setTrue(surface) : setFalse(surface);
	}

	T& operator[](int surface)
	{
		if (surface > surfaces.size() - 1)
			throw("surface index out of bounds!");

		return surfaces[surface];
	}

	void reset()
	{
		for (T t : surfaces)
			t.set(false);

		set(false);
	}

};