#include "extdll.h"
#include "Node.h"

Node::Node(void)
{

}

Node::Node(const Vector &pos)
{
	m_position = pos;
}

Node::~Node(void)
{
}

bool Node::IsChildSet(const std::shared_ptr<Node> pNode) const
{
	return std::find(m_children.begin(), m_children.end(), pNode) != m_children.end();
}

bool Node::AddChild(const std::shared_ptr<Node> pNode)
{
	if (this->IsChildSet(pNode))
		return false;

	m_children.push_back(pNode);
	return true;
}

void Node::PopChild(const std::shared_ptr<Node> pNode)
{
	m_children.erase(std::remove(m_children.begin(), m_children.end(), pNode), m_children.end());
}