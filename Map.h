#pragma once

#include "Waypoint.h"

class Map
{
public:
	Map();
	~Map();

	shr_ptr<Waypoint> CreateWaypoint(const Vector &pos, float radius, int flags);
	void RemoveWaypoint(shr_ptr<Waypoint> pPoint);

	int GetIndex(const shr_ptr<Waypoint> pPoint) const;
	const shr_ptr<Waypoint> GetWaypointAt(int index) const;
	const std::vector<shr_ptr<Waypoint>> &GetWaypoints(void) const { return m_waypoints; }
	const shr_ptr<Waypoint> GetNearestWaypoint(const Vector &pos, float maxRadius) const;

	void Clear(void) { m_waypoints.clear(); }

private:
	std::vector<shr_ptr<Waypoint>> m_waypoints;
};