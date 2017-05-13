#include "extdll.h"
#include "Waypoint.h"

Waypoint::Waypoint(const Vector &pos, float radius, int flags)
{
	SetPos(pos);
	m_radius = radius;
	m_flags = flags;
}

Waypoint::~Waypoint(void)
{
}

int Waypoint::GetChildFlags(std::shared_ptr<Waypoint> pPoint) const
{
	std::map<std::shared_ptr<Node>, int>::const_iterator it = m_childFlags.find(pPoint);
	if (it == m_childFlags.cend())
		return 0;

	return m_childFlags.at(pPoint);
}

void Waypoint::SetChildFlags(const std::shared_ptr<Waypoint> pPoint, int flags)
{
	m_childFlags.at(pPoint) = flags;
}

void Waypoint::PopChild(std::shared_ptr<Node> pNode)
{
	m_childFlags.erase(pNode);
}