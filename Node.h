#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "Header.h"
#include "Waypoint.h"

class Node
{
public:
	Node(void) {};
	Node(const Vector &pos);
	virtual ~Node(void);

	const shr_ptr<Node> GetParent(void) const { return m_pParent; }
	const std::vector<shr_ptr<Node>> &GetChildren(void) const { return m_children; }
	const Vector &GetPos(void) const { return m_position; }
	void SetParent(const shr_ptr<Node> pNode) { m_pParent = pNode; }
	void SetPos(const Vector &pos) { m_position = pos; }
	bool IsChildSet(const shr_ptr<Node> pNode) const;
	virtual void AddChild(shr_ptr<Node> pNode);
	virtual void PopChild(shr_ptr<Node> pNode);

private:
	std::vector<shr_ptr<Node>> m_children;
	shr_ptr<Node> m_pParent;
	Vector m_position;
	//Waypoint *m_pWaypoint;
};