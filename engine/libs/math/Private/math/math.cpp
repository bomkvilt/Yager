#include "math/math.hpp"
#include <cmath>


namespace math
{
	float  Sqrt(float  x) { return std::sqrtf(x); }
	double Sqrt(double x) { return std::sqrt(x); }

	float  Cos(float  x) { return std::cosf(x); }
	double Cos(double x) { return std::cos(x); }
	float  Sin(float  x) { return std::sinf(x); }
	double Sin(double x) { return std::sin(x); }
	float  Tan(float  x) { return std::tanf(x); }
	double Tan(double x) { return std::tan(x); }

	float  Acos(float  x) { return std::acosf(x); }
	double Acos(double x) { return std::acos(x); }
	float  Asin(float  x) { return std::asinf(x); }
	double Asin(double x) { return std::asin(x); }
	float  Atan(float  x) { return std::atanf(x); }
	double Atan(double x) { return std::atan(x); }
	float  Atan2(float  x, float  y) { return std::atan2f(x, y); }
	double Atan2(double x, double y) { return std::atan2(x, y); }

	float  Abs(float  x) { return std::fabsf(x); }
	double Abs(double x) { return std::abs(x); }
	float  Max(float  x, float  y) { return std::fmaxf(x, y); }
	double Max(double x, double y) { return std::max(x, y); }
	float  Min(float  x, float  y) { return std::fminf(x, y); }
	double Min(double x, double y) { return std::min(x, y); }
}
