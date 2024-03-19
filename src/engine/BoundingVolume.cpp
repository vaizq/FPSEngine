//
// Created by vaige on 19.3.2024.
//

#include "BoundingVolume.h"
#include <nlohmann/json.hpp>
#include "Util.h"

nlohmann::json BoundingVolume::serialize(const BoundingVolume &bounds)
{
    nlohmann::json j{};

    j["transform"] = Transform::serialize(bounds.transform);
    j["shape"] = std::visit([](auto&& bs)-> nlohmann::json {
        using T = std::decay_t<decltype(bs)>;
        if constexpr (std::is_same_v<T, Sphere>) {
            return nlohmann::json{
                    {"type", "Sphere"},
                    {"radius", bs.radius}
            };
        }
        else if constexpr (std::is_same_v<T, Cuboid>) {
            return nlohmann::json{
                    {"type", "Cuboid"},
                    {"size", nlohmann::json{
                            {"x", bs.size.x },
                            {"y", bs.size.y},
                            {"z", bs.size.z}
                    }}
            };
        }
        else {
            static_assert(Util::always_false_v<T>, "Non exhaustive visitor");
        }
    }, bounds.shape);

    return j;
}

BoundingVolume BoundingVolume::deserialize(const nlohmann::json& j)
{
    BoundingVolume bv;
    bv.transform = Transform::deserialize(j["transform"]);
    const auto& s = j["shape"];
    if (s["type"] == "Sphere") {
        bv.shape = Sphere{s["radius"]};
    }
    else if (s["type"] == "Cuboid") {
        const auto& size = s["size"];
        bv.shape = Cuboid{glm::vec3{size["x"], size["y"], size["z"]}};
    }

    return bv;
}