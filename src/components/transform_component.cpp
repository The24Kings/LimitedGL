#include "loaded_obj.hpp"
#include <transform_component.hpp>

void transform_component::update(float dt) {
	glm::mat4 model = getModelMatrix();

	loaded_obj* obj = dynamic_cast<loaded_obj*>(m_object);
	if (obj && obj->m_render && obj->m_render->m_mat) {
		obj->m_render->m_mat->set_uniform("model", model);
	}
}
