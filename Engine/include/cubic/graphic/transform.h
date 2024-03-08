#pragma once

#include <cubic/platform.h>

#include <glm/glm.hpp>

namespace cubic {

class CUBIC_API Transform {
 public:
  Transform() = default;

  ~Transform() = default;

  void SetPosition(const glm::vec3& position);

  void SetRotation(const glm::vec3& rotation);

  void SetScale(const glm::vec3& scale);

  const glm::vec3& GetPosition() const { return mPosition; }

  const glm::vec3& GetRotation() const { return mRotation; }

  const glm::vec3& GetScale() const { return mScale; }

  void Translate(const glm::vec3& translation);

  void Rotate(const glm::vec3& rotation);

  void Scale(const glm::vec3& scale);

  glm::mat4 GetModelMatrix() const;

  bool IsDirty() const { return mDirty; }

  void ClearDirty() { mDirty = false; }

 private:
  glm::vec3 mPosition = {};
  glm::vec3 mRotation = {};
  glm::vec3 mScale = {1.f, 1.f, 1.f};

  bool mDirty = true;
};

}  // namespace cubic