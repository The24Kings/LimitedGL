#ifndef _GAME_DATA_HPP
#define _GAME_DATA_HPP

#include <vector>

#include "mesh.hpp"
#include "component_base.hpp"

struct object {
	std::vector<component*> m_components;

	object() {}

	~object() {
		deinit();

		for (auto c : m_components) {
			delete c;
		}
	}

	object(object&) = delete; // No copy constructor

	virtual bool init() { return true; }
	virtual void deinit() {}
	
	virtual void update(float dt) {
		for (auto c : m_components) {
			c->update(dt);
		}
	}

	component* addComponent(component* component) {
		component->m_object = this;

		m_components.push_back(component);

		return component;
	}
}; // obj_data

/**
 * Load an obj file into a mesh object
 *
 * @param baseDir The base directory of the obj file
 * @param filename The name of the obj file
 * @param mesh The mesh object to load the obj data into
 *
 * @return bool True if the obj file was loaded successfully; throw runtime_error on failure
 */
bool load_obj(const char* baseDir, const char* filename, mesh* mesh);

#endif // _GAME_DATA_HPP