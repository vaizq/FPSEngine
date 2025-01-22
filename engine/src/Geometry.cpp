//
// Created by vaige on 12.3.2024.
//

#include "Geometry.hpp"

Mesh Geometry::makeSphere(int nVertices)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    constexpr float R = 1.0f;
    const int nHeight = (int) std::sqrt((float) nVertices);
    const int nRim = nHeight;

    const float delta = 2 * R / nHeight;
    const float deltaAngle = glm::radians(360.0f) / nRim;

    for (int yi = 0; yi <= nHeight; ++yi) {
        const float y = yi * delta - R;
        const float r = std::sqrt(R * R - y * y);
        for (int ai = 0; ai < nRim; ++ai) {
            float angle = ai * deltaAngle;
            float x = r * std::cos(angle);
            float z = r * std::sin(angle);
            glm::vec3 pos{x, y, z};
            vertices.emplace_back(pos, glm::normalize(pos));
        }
    }

    // Create first indices
    for (int i = 1; i < nRim; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    {
        indices.push_back(0);
        indices.push_back(nRim);
        indices.push_back(1);
    }

    // Create rectangles
    const int topIndex = vertices.size() - 1;
    for (int i = 1; i < topIndex; ++i) {
        int upper = i + nRim;
        int right = i + 1;
        int rightUpper = upper + 1;

        indices.push_back(i);
        indices.push_back(upper);
        indices.push_back(right);

        indices.push_back(right);
        indices.push_back(upper);
        indices.push_back(rightUpper);
    }

    // Connect upmost rim to the top point
    const int first = topIndex - nRim;
    for (int i = first; i < topIndex - 1; ++i) {
        indices.push_back(i);
        indices.push_back(topIndex);
        indices.push_back(i + 1);
    }
    {
        indices.push_back(topIndex - 1);
        indices.push_back(topIndex);
        indices.push_back(first);
    }

    return Mesh{vertices, indices, {}};
}

Mesh Geometry::makeBox(const glm::vec3 &size)
{
    const float dx = size.x / 2.0f;
    const float dy = size.y / 2.0f;
    const float dz = size.z / 2.0f;

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;


    auto makeVertex = [&vertices](const glm::vec3 &pos)
    {
        vertices.emplace_back(pos, glm::normalize(pos), glm::vec2{});
    };

    /*
                6----------7
               /|         /|
              / |        / |
             2----------3  |
             |  |       |  |
             |  |       |  |
             |  4-------|--5
             | /        | /
             |/         |/
             0----------1
    */

    makeVertex({-dx, -dy, dz});
    makeVertex({dx, -dy, dz});
    makeVertex({-dx, dy, dz});
    makeVertex({dx, dy, dz});
    makeVertex({-dx, -dy, -dz});
    makeVertex({dx, -dy, -dz});
    makeVertex({-dx, dy, -dz});
    makeVertex({dx, dy, -dz});

    // front
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

    // right
    indices.push_back(5);
    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(5);
    indices.push_back(3);
    indices.push_back(7);

    // top
    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(3);
    indices.push_back(6);
    indices.push_back(7);

    // bottom
    indices.push_back(5);
    indices.push_back(4);
    indices.push_back(0);
    indices.push_back(5);
    indices.push_back(0);
    indices.push_back(1);

    // left
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(6);
    indices.push_back(0);
    indices.push_back(6);
    indices.push_back(2);

    // back
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(7);
    indices.push_back(4);
    indices.push_back(7);
    indices.push_back(6);


    return Mesh{std::move(vertices), std::move(indices), {}};
}

Mesh Geometry::makeBoxWireframe(const glm::vec3 &size)
{
    const float dx = size.x / 2.0f;
    const float dy = size.y / 2.0f;
    const float dz = size.z / 2.0f;

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    auto makeVertex = [&vertices](const glm::vec3 &pos)
    {
        vertices.emplace_back(pos, glm::normalize(pos), glm::vec2{});
    };

    /*
                6----------7
               /|         /|
              / |        / |
             2----------3  |
             |  |       |  |
             |  |       |  |
             |  4-------|--5
             | /        | /
             |/         |/
             0----------1
    */

    makeVertex({-dx, -dy, dz});
    makeVertex({dx, -dy, dz});
    makeVertex({-dx, dy, dz});
    makeVertex({dx, dy, dz});
    makeVertex({-dx, -dy, -dz});
    makeVertex({dx, -dy, -dz});
    makeVertex({-dx, dy, -dz});
    makeVertex({dx, dy, -dz});

    // front
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(0);

    // right
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(3);
    indices.push_back(7);
    indices.push_back(5);
    indices.push_back(7);

    // bottom
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(5);
    indices.push_back(4);
    indices.push_back(0);
    indices.push_back(4);

    // left
    indices.push_back(0);
    indices.push_back(4);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(4);
    indices.push_back(6);

    // back
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(5);
    indices.push_back(7);
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(4);
    indices.push_back(6);


    return Mesh{std::move(vertices), std::move(indices), {}};
}

Mesh Geometry::makePlane(float width, float height, std::vector<Texture> textures)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    constexpr glm::vec3 up{0.0f, 1.0f, 0.0f};
    const float dx = width / 2.0f;
    const float dz = height / 2.0f;


    vertices.emplace_back(glm::vec3{-dx, 0.f, dz},
                          glm::normalize(up),
                          glm::vec2{0.0f, 0.0f});

    vertices.emplace_back(glm::vec3{-dx, 0.f, -dz},
                          glm::normalize(up),
                          glm::vec2{0.0f, 1.0f});

    vertices.emplace_back(glm::vec3{dx, 0.f, -dz},
                          glm::normalize(up),
                          glm::vec2{1.0f, 1.0f});

    vertices.emplace_back(glm::vec3{dx, 0.f, dz},
                          glm::normalize(up),
                          glm::vec2{1.0f, 0.0f});

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    return Mesh{std::move(vertices), std::move(indices), std::move(textures)};
}

Mesh Geometry::makeGrid(int width, int depth, std::vector<Texture> textures) {
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    constexpr glm::vec3 up{0.0f, 1.0f, 0.0f};

    for (int z = 0; z <= depth; ++z) {
        for (int x = 0; x <= width; ++x) {
            vertices.emplace_back(glm::vec3{x, 0.0f, z},
                                  up,
                                  glm::vec2{x, z});

            if (z != depth && x != width) {
                const unsigned tl = x + z * (width + 1);
                const unsigned tr = tl + 1;
                const unsigned bl = tl + (width + 1);
                const unsigned br = bl + 1;

                indices.push_back(tl);
                indices.push_back(bl);
                indices.push_back(br);

                indices.push_back(tl);
                indices.push_back(br);
                indices.push_back(tr);
            }
        }
    }

    return Mesh{std::move(vertices), std::move(indices), std::move(textures)};
}


Mesh Geometry::makeCoordinateMesh()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    vertices.emplace_back(glm::vec3{-1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{0.f, -1.f, 0.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{0.f, 1.f, 0.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{0.f, 0.f, -1.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{0.f, 0.f, 1.f}, glm::vec3{}, glm::vec2{});

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(4);
    indices.push_back(5);

    return Mesh{std::move(vertices), std::move(indices), {}};
}

Mesh Geometry::makeLine(glm::vec3 from, glm::vec3 to) {
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    vertices.emplace_back(from, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(to, glm::vec3{}, glm::vec2{});

    indices.push_back(0);
    indices.push_back(1);

    return Mesh{std::move(vertices), std::move(indices), {}};
}


Mesh Geometry::makeXAxis()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    vertices.emplace_back(glm::vec3{-1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});


    indices.push_back(0);
    indices.push_back(1);

    return Mesh{std::move(vertices), std::move(indices), {}};
}

Mesh Geometry::makeYAxis()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    vertices.emplace_back(glm::vec3{0.f, -1.f, 0.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{0.f, 1.f, 0.f}, glm::vec3{}, glm::vec2{});

    indices.push_back(0);
    indices.push_back(1);

    return Mesh{std::move(vertices), std::move(indices), {}};
}

Mesh Geometry::makeZAxis()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    vertices.emplace_back(glm::vec3{0.f, 0.f, -1.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{0.f, 0.f, 1.f}, glm::vec3{}, glm::vec2{});

    indices.push_back(0);
    indices.push_back(1);

    return Mesh{std::move(vertices), std::move(indices), {}};
}

Mesh Geometry::makeCrosshire()
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    vertices.emplace_back(glm::vec3{-1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{1.f, 0.f, 0.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{0.f, -1.f, 0.f}, glm::vec3{}, glm::vec2{});
    vertices.emplace_back(glm::vec3{0.f, 1.f, 0.f}, glm::vec3{}, glm::vec2{});

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

    return Mesh{std::move(vertices), std::move(indices), {}};
}

Mesh Geometry::makePerlinTerrain(Perlin2D& perlin, int gridSize, glm::vec3 scale, std::vector<Texture> textures)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    const int width = perlin.getWidth() - 1;
    const int height = perlin.getHeight() - 1;

    float dx = 1.0f / static_cast<float>(gridSize);
    float dy = 1.0f / static_cast<float>(gridSize);

    for (int zi = 0; zi <= height * gridSize; ++zi) {
        for (int xi = 0; xi <= width * gridSize; ++xi)
        {
            float x = dx * static_cast<float>(xi);
            float z = dy * static_cast<float>(zi);
            float y = perlin(glm::vec2(x, z));
            vertices.emplace_back(glm::vec3(x, y, z) * scale, glm::vec3{0.0f}, glm::vec2{x, z});
        }
    }

    // Each quad has an index
    // Each vertex has an index
    // Each vertex has a xi and zi

    int totalWidth = width * gridSize;
    int totalHeight = height * gridSize;

    for (int qi = 0; qi < totalWidth * totalHeight; ++qi) {
        int zi = qi / totalWidth;
        int xi = qi % totalWidth;
        int ai = zi * (totalWidth + 1) + xi; // lower left
        int bi = ai + totalWidth + 1; // upper left
        int ci = bi + 1; // upper right
        int di = ai + 1; // lower right

    /*
           bi ------ ci
           |         |
           |         |
           |         |
           ai ------ di
    */

        const auto& ad = vertices[di].position - vertices[ai].position;
        const auto& ab = vertices[bi].position - vertices[ai].position;
        const auto& cb = vertices[ai].position - vertices[ci].position;
        const auto& cd = vertices[di].position - vertices[ci].position;

        const auto n0 = glm::cross(ab, ad);
        const auto n1 = glm::cross(cd, cb);

        vertices[ai].normal += n0;
        vertices[bi].normal += n0 + n1;
        vertices[di].normal += n0 + n1;
        vertices[ci].normal += n1;

        indices.push_back(ai);
        indices.push_back(bi);
        indices.push_back(di);

        indices.push_back(di);
        indices.push_back(bi);
        indices.push_back(ci);
    }

    for (auto& vertex : vertices) {
        vertex.normal = glm::normalize(vertex.normal);
    }

    return Mesh{std::move(vertices), std::move(indices), std::move(textures)};
}
