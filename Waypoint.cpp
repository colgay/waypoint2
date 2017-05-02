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

int Waypoint::GetChildFlags(shr_ptr<Waypoint> pPoint) const
{
	return m_childFlags.at(pPoint);
}

void Waypoint::SetChildFlags(const shr_ptr<Waypoint> pPoint, int flags)
{
	m_childFlags.at(pPoint) = flags;
}

void Waypoint::AddChild(const shr_ptr<Node> pNode)
{
	//m_childFlags[pNode] = 0;
}

void Waypoint::PopChild(const shr_ptr<Node> pNode)
{
	m_childFlags.erase(pNode);
}