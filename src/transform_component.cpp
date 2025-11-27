#include "loaded_obj.hpp"
#include <transform_component.hpp>

void transform_component::update(float dt) {
	model *= glm::translate(glm::mat4(1.0f), position);
	model *= glm::mat4_cast(rotation);
	model *= glm::scale(glm::mat4(1.0f), scale);

	loaded_obj* obj = dynamic_cast<loaded_obj*>(m_object);
	if (obj && obj->m_render && obj->m_render->m_mat) {
		obj->m_render->m_mat->set_uniform("model", model);
	}
}

inline glm::mat4 transform_component::getPositionMatrix() const {
	return glm::translate(glm::mat4(1.0f), -position); // Note the negative sign for moving the world opposite to the camera position
}
