#include "function2d.h"

Function2D::Function2D()
{
    normal = glm::vec2(0.0f);
    origPoint = glm::vec2(0.0f);
}

Function2D::Function2D(glm::vec2 norm, glm::vec2 pt)
{
    normal = norm;
    origPoint = pt;
}
