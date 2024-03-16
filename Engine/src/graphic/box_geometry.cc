#include <cubic/core/log.h>
#include <cubic/graphic/geometry.h>

#include <array>

namespace cubic {

enum axis_e { AXIS_X = 0, AXIS_Y = 1, AXIS_Z = 2 };

using Vector = std::array<float, 3>;

std::shared_ptr<Geometry> Geometry::CreateBox(float width, float height, float depth, uint32_t widthSegment,
                                              uint32_t heightSegment, uint32_t depthSegment) {
  if (!std::isfinite(width) || !std::isfinite(height) || !std::isfinite(depth) || widthSegment == 0 ||
      heightSegment == 0 || depthSegment == 0) {
    CUB_ERROR("[BoxGeometry]: invalid initial parameters !!");
    return nullptr;
  }

  std::vector<float> vertexData{};
  std::vector<uint32_t> indexData{};

  uint32_t numberOfVertices = 0;

  auto build_panel = [&vertexData, &indexData, &numberOfVertices](axis_e u, axis_e v, axis_e w, int32_t udir,
                                                                  int32_t vdir, float width, float height, float depth,
                                                                  uint32_t gridX, uint32_t gridY) {
    auto segmentWidth = width / gridX;
    auto segmentHeight = height / gridY;

    auto widthHalf = width / 2;
    auto heightHalf = height / 2;
    auto depthHalf = depth / 2;

    auto gridX1 = gridX + 1;
    auto gridY1 = gridY + 1;

    Vector vector{};

    uint32_t vertexCounter = 0;

    for (uint32_t iy = 0; iy < gridY1; iy++) {
      auto y = iy * segmentHeight - heightHalf;

      for (uint32_t ix = 0; ix < gridX1; ix++) {
        auto x = ix * segmentWidth - widthHalf;

        vector[u] = x * udir;
        vector[v] = y * vdir;
        vector[w] = depthHalf;

        vertexData.emplace_back(vector[0]);
        vertexData.emplace_back(vector[1]);
        vertexData.emplace_back(vector[2]);

        vector[u] = 0;
        vector[v] = 0;
        vector[w] = depth > 0 ? 1 : -1;

        vertexData.emplace_back(vector[0]);
        vertexData.emplace_back(vector[1]);
        vertexData.emplace_back(vector[2]);

        // uvs
        vertexData.emplace_back(ix / static_cast<float>(gridX));
        vertexData.emplace_back(1.f - iy / static_cast<float>(gridY));

        vertexCounter++;
      }
    }

    for (uint32_t iy = 0; iy < gridY; iy++) {
      for (uint32_t ix = 0; ix < gridX; ix++) {
        auto a = numberOfVertices + ix + gridX1 * iy;
        auto b = numberOfVertices + ix + gridX1 * (iy + 1);
        auto c = numberOfVertices + (ix + 1) + gridX1 * (iy + 1);
        auto d = numberOfVertices + (ix + 1) + gridX1 * iy;

        indexData.push_back(a);
        indexData.push_back(b);
        indexData.push_back(d);

        indexData.push_back(b);
        indexData.push_back(c);
        indexData.push_back(d);
      }
    }

    numberOfVertices += vertexCounter;
  };

  build_panel(AXIS_Z, AXIS_Y, AXIS_X, -1, -1, depth, height, width, depthSegment, heightSegment);
  build_panel(AXIS_Z, AXIS_Y, AXIS_X, 1, -1, depth, height, -width, depthSegment, heightSegment);
  build_panel(AXIS_X, AXIS_Z, AXIS_Y, 1, 1, width, depth, height, widthSegment, depthSegment);
  build_panel(AXIS_X, AXIS_Z, AXIS_Y, 1, -1, width, depth, -height, widthSegment, depthSegment);
  build_panel(AXIS_X, AXIS_Y, AXIS_Z, 1, -1, width, height, depth, widthSegment, heightSegment);
  build_panel(AXIS_X, AXIS_Y, AXIS_Z, -1, -1, width, height, -depth, widthSegment, heightSegment);

  std::vector<BufferAttribute> attrs{};

  attrs.emplace_back(BufferAttribute{"position", VertexAttribute{VertexFormat::kFloat32x3, 0, 0}});
  attrs.emplace_back(BufferAttribute{"normal", VertexAttribute{VertexFormat::kFloat32x3, 3 * sizeof(float), 1}});
  attrs.emplace_back(BufferAttribute{"uv", VertexAttribute{VertexFormat::kFloat32x2, 6 * sizeof(float), 2}});

  return std::make_shared<Geometry>(std::move(vertexData), std::move(indexData), std::move(attrs));
}

}  // namespace cubic