#include "Voxel.hpp"

Voxel::Voxel():
    color(1.0,1.0,1.0,1.0),
    density(-1.0)
{}

Voxel::Voxel(glm::vec4 color, float density):
    color(color),
    density(density)
{}
