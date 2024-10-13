#pragma once


namespace common {

struct Vertex {
  Vertex() = default;

  Vertex(glm::vec3 p) : position(p) {}

  Vertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n) : position(p), texCoords(uv), normal(n) {}

  Vertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n, glm::vec4 c, glm::vec3 t, glm::vec3 bt)
      : position(p), texCoords(uv), normal(n), color(c), tangent(t), bitangent(bt) {}

  alignas(32) glm::vec3 position = glm::vec3(0.0f);
  alignas(32) glm::vec2 texCoords = glm::vec2(0.0f);
  alignas(32) glm::vec3 normal = glm::vec3(0.0f);
  alignas(32) glm::vec4 color = glm::vec4(0.0f);
  alignas(32) glm::vec3 tangent = glm::vec3(0.0f);
  alignas(32) glm::vec3 bitangent = glm::vec3(0.0f);
};

struct AnimatedVertex {
  AnimatedVertex() = default;

  AnimatedVertex(glm::vec3 p) : position(p) {}

  AnimatedVertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n) : position(p), texCoords(uv), normal(n) {}

  AnimatedVertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n, glm::vec4 c, glm::vec3 t, glm::vec3 bt)
      : position(p), texCoords(uv), normal(n), color(c), tangent(t), bitangent(bt) {}

  alignas(32) glm::vec3 position = glm::vec3(0.0f);
  alignas(32) glm::vec2 texCoords = glm::vec2(0.0f);
  alignas(32) glm::vec3 normal = glm::vec3(0.0f);
  alignas(32) glm::vec4 color = glm::vec4(0.0f);
  alignas(32) glm::vec3 tangent = glm::vec3(0.0f);
  alignas(32) glm::vec3 bitangent = glm::vec3(0.0f);
  alignas(32) glm::ivec4 boneIDs = glm::ivec4(0);
  alignas(32) glm::vec4 weights = glm::vec4(0.0f);
};

struct Triangle {
  Triangle() = default;
  Triangle(uint32_t a, uint32_t b, uint32_t c) {
    vertices[0] = a;
    vertices[1] = b;
    vertices[2] = c;
  }
  uint32_t vertices[3] = {};
  uint32_t id = 0;
};

template <typename VertexType> struct BaseMesh {
  using vertex_type = VertexType;
  void addTriangle(uint32_t a, uint32_t b, uint32_t c) {
    Indices.push_back(a);
    Indices.push_back(b);
    Indices.push_back(c);
  }

  void addQuad(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    Indices.push_back(a);
    Indices.push_back(b);
    Indices.push_back(d);
    Indices.push_back(b);
    Indices.push_back(c);
    Indices.push_back(d);
  }

  Triangle getTriangle(uint32_t index) const {
    uint32_t a = Indices.at((index * 3));
    uint32_t b = Indices.at((index * 3) + 1);
    uint32_t c = Indices.at((index * 3) + 2);

    Triangle tr(a, b, c);
    tr.id = index;
    return tr;
  }

  uint32_t numTriangles() const { return Indices.size() / 3; }

  std::vector<VertexType> Vertices;
  std::vector<uint32_t> Indices;
};

using Mesh = BaseMesh<common::Vertex>;
using AnimatedMesh = BaseMesh<common::AnimatedVertex>;

struct MeshMaterial {
  std::string albedo_path{};
  std::string metallic_path{};
  std::string normal_path{};
  std::string roughness_path{};

  glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
  float metallic = 0.0f;
  float roughness = 0.5f;
  float specular = 1.0f;
  uint32_t mask = 0xffffffff;
  std::string name;
};

struct MIN_MAX_POINTS {
  float MIN_X = 0.0f;
  float MIN_Y = 0.0f;
  float MIN_Z = 0.0f;

  float MAX_X = 0.0f;
  float MAX_Y = 0.0f;
  float MAX_Z = 0.0f;

  MIN_MAX_POINTS() = default;

  MIN_MAX_POINTS(glm::vec3 min, glm::vec3 max) {
    MIN_X = min.x;
    MIN_Y = min.y;
    MIN_Z = min.z;

    MAX_X = max.x;
    MAX_Y = max.y;
    MAX_Z = max.z;

    if (MIN_X > MAX_X) {
      float x = MAX_X;
      MAX_X = MIN_X;
      MIN_X = x;
    }

    if (MIN_Y > MAX_Y) {
      float y = MAX_Y;
      MAX_Y = MIN_Y;
      MIN_Y = y;
    }

    if (MIN_Z > MAX_Z) {
      float z = MAX_Z;
      MAX_Z = MIN_Z;
      MIN_Z = z;
    }
  }

  glm::vec3 getCenterOfMass() { return glm::vec3(MAX_X + MIN_X, MAX_Y + MIN_Y, MAX_Z + MIN_Z) * 0.5f; }

  glm::vec3 getSize() {

    float mx = MIN_X < MAX_X ? MIN_X : MAX_X;
    float my = MIN_Y < MAX_Y ? MIN_Y : MAX_Y;
    float mz = MIN_Z < MAX_Z ? MIN_Z : MAX_Z;

    float Mx = MIN_X > MAX_X ? MIN_X : MAX_X;
    float My = MIN_Y > MAX_Y ? MIN_Y : MAX_Y;
    float Mz = MIN_Z > MAX_Z ? MIN_Z : MAX_Z;

    return glm::vec3(glm::abs(Mx - mx), glm::abs(My - my), glm::abs(Mz - mz));
  }

  glm::vec3 Min() { return glm::vec3(MIN_X, MIN_Y, MIN_Z); }

  glm::vec3 Max() { return glm::vec3(MAX_X, MAX_Y, MAX_Z); }

  std::vector<glm::vec3> createBox() {
    std::vector<glm::vec3> points;
    points.resize(8);
    auto min = Min(), max = Max();
    points[0] = min;
    points[1] = glm::vec3(max.x, min.y, min.z);
    points[2] = glm::vec3(min.x, max.y, min.z);
    points[3] = glm::vec3(min.x, min.y, max.z);
    points[4] = glm::vec3(min.x, max.y, max.z);
    points[5] = glm::vec3(max.x, min.y, max.z);
    points[6] = glm::vec3(max.x, max.y, min.z);
    points[7] = max;

    return points;
  }

  void transform(glm::mat4 transform) {

    auto box = createBox();

    glm::vec3 min = glm::vec3(1000000.0f);
    glm::vec3 max = glm::vec3(-1000000.0f);

    for (auto &point : box) {
      point = glm::vec3(transform * glm::vec4(point, 1.0f));

      if (point.x < min.x)
        min.x = point.x;
      if (point.y < min.y)
        min.y = point.y;
      if (point.z < min.z)
        min.z = point.z;

      if (point.x > max.x)
        max.x = point.x;
      if (point.y > max.y)
        max.y = point.y;
      if (point.z > max.z)
        max.z = point.z;
    }

    MIN_X = min.x;
    MIN_Y = min.y;
    MIN_Z = min.z;

    MAX_X = max.x;
    MAX_Y = max.y;
    MAX_Z = max.z;
  }
};

struct BoundingBox {
  float FRONT_TOP_LEFT = 0.f;
  float FRONT_TOP_RIGHT = 0.f;
  float FRONT_BOTTOM_LEFT = 0.f;
  float FRONT_BOTTOM_RIGHT = 0.f;

  float BACK_TOP_LEFT = 0.;
  float BACK_TOP_RIGHT = 0.f;
  float BACK_BOTTOM_LEFT = 0.f;
  float BACK_BOTTOM_RIGHT = 0.f;
};
} // namespace common