#ifndef COMMON_HPP
#define COMMON_HPP

#include <memory>

// ------| macro
#include "macro.hpp"

// ------| math
#include "math/math.hpp"
#include "math/math_common.hpp"
#include "math/color.hpp"
#include "math/quat.hpp"
#include "math/vector.hpp"
#include "math/vector2.hpp"
#include "math/transform.hpp"


using FTransform = TTransform<FReal>;
using FVector2   = TVector2<FReal>;
using FVector    = TVector<FReal>;
using FQuat      = TQuat<FReal>;


#endif //!COMMON_HPP
