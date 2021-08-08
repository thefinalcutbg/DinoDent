#include "ToothContainer.h"
#include <stdexcept>

ToothContainer::ToothContainer()
{
    m_teeth = new std::array<Tooth, teethNum>{};
}

ToothContainer::ToothContainer(const ToothContainer& other)
{
    m_teeth = new std::array<Tooth, teethNum>;

    for (int i = 0; i < teethNum; i++)
    {
        m_teeth->at(i) = other.m_teeth->at(i);
    }

}

ToothContainer::ToothContainer(ToothContainer&& other) noexcept
{
    m_teeth = other.m_teeth;
    other.m_teeth = nullptr;
}

ToothContainer::~ToothContainer()
{
    if (m_teeth) delete m_teeth;
}

Tooth& ToothContainer::operator[](int index)
{
    if (m_teeth == nullptr)
        throw std::invalid_argument("container has been moved");

    if(index < teethNum || index < 0)
        throw std::invalid_argument("index out of range");

    return m_teeth->at(index);
}
