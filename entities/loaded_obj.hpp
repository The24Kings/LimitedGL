#ifndef _BASE_OBJECTS_HPP
#define _BASE_OBJECTS_HPP

#include <glm/glm.hpp>
#include <stdexcept>

#include "object.hpp"
#include "shader.hpp"

/**
* @brief A loaded object from an OBJ file with a texture
*/
class loaded_obj : public object {
public:
	std::string texture_file;
	std::string object_file;
	std::string objBaseDir;

	render_component* m_render;

	/**
	* Create a new loaded_obj object
	*
	* @param of The object file
	* @param tf The texture file
	* @param shader Object shader
	*/
	loaded_obj(std::string of, std::string tf, shader* shader) : object_file(of), texture_file(tf), objBaseDir(object_file.substr(0, object_file.find('/'))) {
		m_render = (render_component*)addComponent(new render_component(shader));

		m_render->m_mat->m_tex = new texture();
	}

	bool init() override {
		if (!load_obj(objBaseDir.c_str(), object_file.c_str(), m_render->m_mesh)) {
			printf(RED("Failed to load obj file\n").c_str());
			return false;
		}

		// Load and Bind textures
		if (!load_texture(texture_file.c_str(), m_render->m_mat->m_tex)) {
			printf(RED("Failed to load texture: %s\n").c_str(), texture_file);
			return false;
		}

		return true;
	} // init

	void deinit() override {
		// Cleanup texture
		if (m_render->m_mat->m_tex) {
			delete m_render->m_mat->m_tex;
		}
	}
}; // ~loaded_obj

#endif // _BASE_OBJECTS_HPP
