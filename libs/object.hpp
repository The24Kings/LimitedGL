#ifndef _GAME_DATA_HPP
#define _GAME_DATA_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLEW/glew.h>
#include <vector>

#include "mesh.hpp"
#include "component_base.hpp"

struct object {
	std::vector<component*> m_components;

	object(object&) = delete; // No copy constructor

	virtual bool init() { return 0; }
	virtual void deinit() {}
	
	void update(float dt) {
		for (auto c : m_components) {
			c->update(dt);
		}
	}

	component* addComponent(component* component) {
		component->m_object = this;

		m_components.push_back(component);

		return component;
	}

	object() {}

	~object() { 
		deinit();

		for (auto c : m_components) {
			delete c;
		}
	}
}; // obj_data

bool load_obj(const char* baseDir, const char* filename, mesh* mesh);

#endif // _GAME_DATA_HPP