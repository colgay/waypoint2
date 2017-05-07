#pragma once

#include <algorithm>
#include <vector>
#include <memory>

class Waypoint;

class Map
{
public:
	Map(void);
	~Map(void);

	std::shared_ptr<Waypoint> CreateWaypoint(const Vector &pos, float radius, int flags);
	void RemoveWaypoint(std::shared_ptr<Waypoint> pPoint);

	int GetIndex(const std::shared_ptr<Waypoint> pPoint) const;
	const std::shared_ptr<Waypoint> GetWaypointAt(int index) const;
	std::vector<std::shared_ptr<Waypoint>> &GetWaypoints(void) { return m_waypoints; }
	const std::shared_ptr<Waypoint> GetNearestWaypoint(const Vector &pos, float maxRadius) const;

	void Clear(void) { m_waypoints.clear(); }

private:
	std::vector<std::shared_ptr<Waypoint>> m_waypoints;
};