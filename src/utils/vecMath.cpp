/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "vecMath.h"

namespace
{
  constexpr float pi_hi =  3.14159274e+00f; // 0x1.921fb6p+01
  constexpr float pi_lo = -8.74227766e-08f; // -0x1.777a5cp-24
}

/**
 * Copied from libdragons fm_sinf implementation.
 * This was done to guarantee reproducible results with potential libdragon updates.
 */
float Math::sinf(float angle)
{
  float x = fm_fmodf(angle+PI, 2*PI) - PI;

  // Approximation of sine to 5 ULP with Chebyshev polynomials
  // http://mooooo.ooo/chebyshev-sine-approximation/
  float p, s;

  p = 0;
  s = x * x;
  // Execute only a portion of the series, depending on the approximation level.
  // This generate the most efficient code among similar approaches.
  p +=   2.5222900603e-6f,  p *= s;
  p += - 1.7350520647e-4f,  p *= s;
  p +=   6.6208802163e-3f,  p *= s;
  p += - 1.0132116824e-1f;
  x = x * ((x - pi_hi) - pi_lo) * ((x + pi_hi) + pi_lo) * p;
  return x;
}
