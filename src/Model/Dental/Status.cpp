#include "Status.h"
#include "Tooth.h"

void Dsn::init(int idx)
{
	dsn_tooth = std::make_unique<Tooth>(idx);
}

Dsn::Dsn(const Dsn& other)
{
	m_exists = other.m_exists;
	
	if (other.toothNotNull())
	{
		dsn_tooth = std::make_unique<Tooth>(other.dsn_tooth->index);
		*dsn_tooth = *other.dsn_tooth;
	}
}

Dsn& Dsn::operator=(const Dsn& other)
{
	m_exists = other.m_exists;

	if (!other.toothNotNull()) return *this;

	dsn_tooth = std::make_unique<Tooth>(other.dsn_tooth->index);
	*dsn_tooth = *other.dsn_tooth;

	return *this;
}
