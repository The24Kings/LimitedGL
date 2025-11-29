#ifndef _COMPONENT_BASE_HPP
#define _COMPONENT_BASE_HPP

struct object; // Forward decl

struct component {
	object* m_object;

	virtual void update(float dt) {}

	component() : m_object(nullptr) {}
};

#endif // _COMPONENT_BASE_HPP