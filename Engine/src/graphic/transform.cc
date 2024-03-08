#include <cubic/graphic/transform.h>

#include <glm/gtc/matrix_transform.hpp>

namespace cubic {

void Transform::SetPosition(const glm::vec3& position) {
  mPosition = position;
  mDirty = true;
}

void Transform::SetRotation(const glm::vec3& rotation) {
  mRotation = rotation;
  mDirty = true;
}

void Transform::SetScale(const glm::vec3& scale) {
  mScale = scale;
  mDirty = true;
}

void Transform::Translate(const glm::vec3& translation) {
  mPosition += translation;
  mDirty = true;
}

void Transform::Rotate(const glm::vec3& rotation) {
  mRotation += rotation;
  mDirty = true;
}

void Transform::Scale(const glm::vec3& scale) {
  mScale += scale;
  mDirty = true;
}

glm::mat4 Transform::GetModelMatrix() const {
  glm::mat4 model = glm::mat4(1.0f);

  // rotate
  model = glm::rotate(model, mRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, mRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, mRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  // translate
  model = glm::translate(model, mPosition);

  // scale
  model = glm::scale(model, mScale);

  return model;
}

}  // namespace cubic