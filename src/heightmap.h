#include <Rcpp.h>
using namespace Rcpp;

#include <string>
#include <utility>
#include <vector>

struct ivec2 {
  int x;
  int y;
};

class Heightmap {
public:
  Heightmap(const std::vector<float> &data);
  
  Heightmap(
    const int width,
    const int height,
    const std::vector<float> &data);
  
  int Width() const {
    return m_Width;
  }
  
  int Height() const {
    return m_Height;
  }
  
  float At(const int x, const int y) const {
    return m_Data[y * m_Width + x];
  }
  
  float At(const ivec2 p) const {
    return m_Data[p.y * m_Width + p.x];
  }
  
  std::pair<ivec2, float> FindCandidate(
      const ivec2 p0,
      const ivec2 p1,
      const ivec2 p2) const;
  
private:
  int m_Width;
  int m_Height;
  std::vector<float> m_Data;
};