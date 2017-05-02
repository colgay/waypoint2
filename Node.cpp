#include "extdll.h"
#include "Node.h"

Node::Node(void)
{

}

Node::Node(const Vector &pos)
{
	m_position = pos;
}

Node::~Node()
{
}

bool Node::IsChildSet(const shr_ptr<Node> pNode) const
{
	return std::find(m_children.begin(), m_children.end(), pNode) != m_children.end();
}

void Node::AddChild(const shr_ptr<Node> pNode)
{
	m_children.push_back(pNode);
}

void Node::PopChild(const shr_ptr<Node> pNode)
{
	m_children.erase(std::remove(m_children.begin(), m_children.end(), pNode), m_children.end());
}