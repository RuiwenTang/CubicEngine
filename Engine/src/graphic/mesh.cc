#include <cubic/graphic/mesh.h>

namespace cubic {

Mesh::Mesh(std::shared_ptr<Geometry> geom, std::shared_ptr<Material> material)
    : mGeometry(std::move(geom)), mMaterial(std::move(material)) {}

}  // namespace cubic
