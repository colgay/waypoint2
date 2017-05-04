#pragma once

#include <vector>
#include <memory>
#include <algorithm>

class Waypoint;

class Node
{
public:
	Node(void);
	Node(const Vector &pos);
	virtual ~Node(void);

	const std::shared_ptr<Node> GetParent(void) const { return m_pParent; }
	const std::vector<std::shared_ptr<Node>> &GetChildren(void) const { return m_children; }
	const Vector &GetPos(void) const { return m_position; }
	const std::shared_ptr<Waypoint> GetWaypoint(void) const { return m_pWaypoint; }
	void SetParent(const std::shared_ptr<Node> pNode) { m_pParent = pNode; }
	void SetPos(const Vector &pos) { m_position = pos; }
	void SetWaypoint(const std::shared_ptr<Waypoint> pPoint) { m_pWaypoint = pPoint; }
	bool IsChildSet(const std::shared_ptr<Node> pNode) const;
	virtual bool AddChild(std::shared_ptr<Node> pNode);
	virtual void PopChild(std::shared_ptr<Node> pNode);

private:
	std::vector<std::shared_ptr<Node>> m_children;
	std::shared_ptr<Node> m_pParent;
	Vector m_position;
	std::shared_ptr<Waypoint> m_pWaypoint;
};