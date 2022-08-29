#pragma once
#include "Status.h"
#include "ToothUtils.h"

template <typename T> class SurfaceStatus;

template<typename T> class SurfaceChild : public T
{
	static_assert(std::is_base_of<Status, T>::value, "Template argument must be derived of Status");
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

		if (!m_exists)
			for (T& t : surfaces)
				t.set(false);

		m_exists = true;
		surfaces[surface].set(true);
	}

	void setFalse(int surface)
	{
		surfaces[surface].set(false);

		for (T& t : surfaces)
		{
			if (t.exists()) return;
		}

		set(false);
	}

public:

	SurfaceStatus() : default_surface{ 0 } {}

	std::string getInfo(int toothIdx) const
	{
		if (!exists()) return "";

		std::string result;

		auto surfNames = ToothUtils::getSurfaceNames(toothIdx);

		for (int i = 0; i < surfaceCount; i++) {

			if (!exists(i)) continue;

			//if (obtur.empty()) obtur.append(u8"<br><b>Обтурация:</b> ");

			result += surfNames[i];

			auto infoStr =  surfaces[i].info();
			if (infoStr.size()) {
				result.append(" (" + infoStr + ")");
			}

			result += ", ";
		}

		if (result.size()) {
			result.pop_back();
			result.pop_back();
			result.append("<br>");
		}

		return result;
	}

	void setDefaultSurface(int surface)
	{
		default_surface = surface;
	}

	bool exists(int surface) const
	{

		return m_exists ? surfaces[surface].exists() : false;
	}
/*
	bool exists(Surface surface) const
	{
		return exists(static_cast<int>(surface));
	}
	*/
	bool exists() const override { return Status::exists(); } // why do I need this?

	void set(bool exists)
	{
		m_exists = exists;

		if (exists)
		{
			for (T& t : surfaces)
				if (t.exists())
					return;

			surfaces[default_surface].set(true);
		}
	}

	void set(bool exists, int surface)
	{
		exists ? setTrue(surface) : setFalse(surface);
	}

	T& operator[](int surface) { return surfaces[surface]; }
	const T& operator[](int surface) const { return surfaces[surface]; }

	std::array<bool, 6> getBoolStatus() const{ 

		std::array<bool, 6> boolStatus; 

		for (int i = 0; i < 6; i++)
			boolStatus[i] = exists(i);

		return boolStatus;

	}

	void reset()
	{
		for (T &t : surfaces)
			t.set(false);

		set(false);
	}
};