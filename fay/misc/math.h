#pragma once

#include <glm.h>

namespace fay
{

#ifdef FAY_MATH

//

#else

using vec3f = glm::vec3;
using vec4f = glm::vec4;
using vec3 = vec3f;
using vec4 = vec4f;
...
#endif

} // namespace fay


