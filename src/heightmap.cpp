#include "heightmap.h"
#include <algorithm>

inline ivec2 min2(ivec2 a, ivec2 b, ivec2 c) {
  return(ivec2{std::min(std::min(a.x, b.x), c.x), std::min(std::min(a.y, b.y), c.y)});
}

inline ivec2 max2(ivec2 a, ivec2 b, ivec2 c) {
  return(ivec2{std::max(std::max(a.x, b.x), c.x), std::max(std::max(a.y, b.y), c.y)});
}

inline bool equals(ivec2 a, ivec2 b) {
  return(a.x == b.x && a.y == b.y);
}

Heightmap::Heightmap(const int width, const int height, const std::vector<float>& mat) {
  m_Width = width;
  m_Height = height;
  m_Data = mat;
}

std::pair<ivec2, float> Heightmap::FindCandidate(
    const ivec2 p0,
    const ivec2 p1,
    const ivec2 p2) const {
  const auto edge = [](
    const ivec2 a, const ivec2 b, const ivec2 c)
  {
    return (b.x - c.x) * (a.y - c.y) - (b.y - c.y) * (a.x - c.x);
  };
  
  
  // triangle bounding box
  const ivec2 min = min2(p0, p1, p2);
  const ivec2 max = max2(p0, p1, p2);
  
  // forward differencing variables
  int w00 = edge(p1, p2, min);
  int w01 = edge(p2, p0, min);
  int w02 = edge(p0, p1, min);
  const int a01 = p1.y - p0.y;
  const int b01 = p0.x - p1.x;
  const int a12 = p2.y - p1.y;
  const int b12 = p1.x - p2.x;
  const int a20 = p0.y - p2.y;
  const int b20 = p2.x - p0.x;
  
  // pre-multiplied z values at vertices
  const float a = edge(p0, p1, p2);
  const float z0 = At(p0) / a;
  const float z1 = At(p1) / a;
  const float z2 = At(p2) / a;
  
  // iterate over pixels in bounding box
  float maxError = 0;
  ivec2 maxPoint{0, 0};
  for (int y = min.y; y <= max.y; y++) {
    // compute starting offset
    int dx = 0;
    if (w00 < 0 && a12 != 0) {
      dx = std::max(dx, -w00 / a12);
    }
    if (w01 < 0 && a20 != 0) {
      dx = std::max(dx, -w01 / a20);
    }
    if (w02 < 0 && a01 != 0) {
      dx = std::max(dx, -w02 / a01);
    }
    
    int w0 = w00 + a12 * dx;
    int w1 = w01 + a20 * dx;
    int w2 = w02 + a01 * dx;
    
    bool wasInside = false;
    
    for (int x = min.x + dx; x <= max.x; x++) {
      // check if inside triangle
      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        wasInside = true;
        
        // compute z using barycentric coordinates
        const float z = z0 * w0 + z1 * w1 + z2 * w2;
        const float dz = std::fabs(z - At(x, y));
        if (dz > maxError) {
          maxError = dz;
          maxPoint = ivec2{x, y};
        }
      } else if (wasInside) {
        break;
      }
      
      w0 += a12;
      w1 += a20;
      w2 += a01;
    }
    
    w00 += b12;
    w01 += b20;
    w02 += b01;
  }
  
  if (equals(maxPoint, p0) || equals(maxPoint,p1) || equals(maxPoint, p2)) {
    maxError = 0;
  }
  return std::make_pair(maxPoint, maxError);
}