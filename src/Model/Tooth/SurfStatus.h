#pragma once
#include "Status.h"

template <typename T> class SurfaceStatus;

template<typename T> class SurfaceChild : public T
{
	//enforcing that these two Status functions can be accessed only by the SurfaceStatus:
	using T::exists; 
	using T::set;
	friend SurfaceStatus<SurfaceChild<T> >;
};

template<typename T>
class SurfaceStatus : public Status
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
		{
			if (t.exists()) return;
		}

		set(false);
	}

public:

	SurfaceStatus() : default_surface{ 0 } {}

	void setDefaultSurface(int surface)
	{
		default_surface = surface;
	}

	bool exists(int surface) const
	{
		return exists() ? surfaces[surface].exists() : exists();	
	}

	bool exists() const override { return Status::exists(); } // why do I need this?

	void set(bool exists)
	{
		Status::set(exists);

		if (Status::exists())
		{
			for (T t : surfaces)
				if (t.exists())
					return;

			surfaces[default_surface].set(true);
		}
	}

	void set(bool exists, int surface)
	{
		exists == true ? setTrue(surface) : setFalse(surface);
	}

	T& operator[](int surface) { return surfaces[surface]; }
	const T& operator[](int surface) const { return surfaces[surface]; }

	void reset()
	{
		for (T t : surfaces)
			t.set(false);

		set(false);
	}
};