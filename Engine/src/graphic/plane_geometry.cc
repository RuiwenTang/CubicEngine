#include <cubic/core/log.h>
#include <cubic/graphic/geometry.h>

#include <cmath>

namespace cubic {

std::shared_ptr<Geometry> Geometry::CreatePlane(float width, float height, uint32_t widthSegment,
                                                uint32_t heightSegment) {
  if (!std::isfinite(width) || !std::isfinite(height) || widthSegment == 0 || heightSegment == 0) {
    CUB_ERROR("[PlaneGeometry]: invalid initial parameters !!");
    return {};
  }

  std::vector<float> vertexData{};
  std::vector<uint32_t> indexData{};

  auto width_half = width / 2.f;
  auto height_half = height / 2.f;

  auto grid_x = std::floor(widthSegment);
  auto grid_y = std::floor(heightSegment);

  auto grid_x_1 = grid_x + 1;
  auto grid_y_1 = grid_y + 1;

  auto segment_width = width / grid_x;
  auto segment_height = height / grid_y;

  for (uint32_t iy = 0; iy < static_cast<uint32_t>(grid_y_1); iy++) {
    auto y = iy * segment_height - height_half;

    for (uint32_t ix = 0; ix < static_cast<uint32_t>(grid_x_1); ix++) {
      auto x = ix * segment_width - width_half;

      // vertex
      vertexData.emplace_back(x);
      vertexData.emplace_back(-y);
      vertexData.emplace_back(0);
      // normals
      vertexData.emplace_back(0);
      vertexData.emplace_back(0);
      vertexData.emplace_back(1.f);
      // uv
      vertexData.emplace_back(ix / grid_x);
      vertexData.emplace_back(1.f - (iy / grid_y));
    }
  }

  for (uint32_t iy = 0; iy < static_cast<uint32_t>(grid_y); iy++) {
    for (uint32_t ix = 0; ix < static_cast<uint32_t>(grid_x); ix++) {
      auto a = static_cast<uint32_t>(ix + grid_x_1 * iy);
      auto b = static_cast<uint32_t>(ix + grid_x_1 * (iy + 1));
      auto c = static_cast<uint32_t>((ix + 1) + grid_x_1 * (iy + 1));
      auto d = static_cast<uint32_t>((ix + 1) + grid_x_1 * iy);

      indexData.emplace_back(a);
      indexData.emplace_back(b);
      indexData.emplace_back(d);

      indexData.emplace_back(b);
      indexData.emplace_back(c);
      indexData.emplace_back(d);
    }
  }

  std::vector<BufferAttribute> attribute{};

  attribute.emplace_back(BufferAttribute{"position", VertexAttribute{VertexFormat::kFloat32x3, 0, 0}});
  attribute.emplace_back(BufferAttribute{"normal", VertexAttribute{VertexFormat::kFloat32x3, 3 * sizeof(float), 1}});
  attribute.emplace_back(BufferAttribute{"uv", VertexAttribute{VertexFormat::kFloat32x2, 6 * sizeof(float), 2}});

  return std::make_shared<Geometry>(std::move(vertexData), std::move(indexData), std::move(attribute));
}

}  // namespace cubic
