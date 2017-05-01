#pragma once

#include <map>

#include "Node.h"

class Waypoint : public Node
{
public:
	Waypoint(const Vector &pos, float radius, int flags);
	~Waypoint(void);

	int GetFlags(void) const { return m_flags; }
	float GetRadius(void) const { return m_radius; }
	int GetChildFlags(const shr_ptr<Waypoint> pPoint) const;

	void SetFlags(int flags) { m_flags = flags; }
	void SetRadius(float radius) { m_radius = radius; }
	void SetChildFlags(const shr_ptr<Waypoint> pPoint, int flags);

	void AddChild(const shr_ptr<Node> pNode) override;
	void PopChild(const shr_ptr<Node> pNode) override;

private:
	int m_flags;
	float m_radius;
	std::map<shr_ptr<Node>, int> m_childFlags;
};