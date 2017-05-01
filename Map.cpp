#include "Map.h"
#include "extdll.h"


Map::Map()
{
}

Map::~Map()
{
}

shr_ptr<Waypoint> Map::CreateWaypoint(const Vector &pos, float radius, int flags)
{
	shr_ptr<Waypoint> pPoint = std::make_shared<Waypoint>(pos, radius, flags);
	m_waypoints.push_back(pPoint);
}

void Map::RemoveWaypoint(shr_ptr<Waypoint> pPoint)
{
	std::vector<shr_ptr<Waypoint>>::iterator it = std::remove(m_waypoints.begin(), m_waypoints.end(), pPoint);
	m_waypoints.erase(it);
}

const shr_ptr<Waypoint> Map::GetWaypointAt(int index) const
{
	return (index < 0 || index >= m_waypoints.size()) ? nullptr : m_waypoints.at(index);
}

int Map::GetIndex(const shr_ptr<Waypoint> pPoint) const
{
	std::vector<shr_ptr<Waypoint>>::const_iterator it = std::find(m_waypoints.cbegin(), m_waypoints.cend(), pPoint);

	return std::distance(m_waypoints.cbegin(), it);
}

const shr_ptr<Waypoint> Map::GetNearestWaypoint(const Vector &pos, float maxRadius) const
{
	shr_ptr<Waypoint> pPoint, pResult = nullptr;
	float dist, minDist = maxRadius;

	for (std::vector<shr_ptr<Waypoint>>::const_iterator it = m_waypoints.cbegin(); it != m_waypoints.end(); ++it)
	{
		pPoint = *it;

		dist = (pPoint->GetPos() - pos).Length();
		if (dist < minDist)
		{
			pResult = pPoint;
			minDist = dist;
		}
	}

	return pResult;
}