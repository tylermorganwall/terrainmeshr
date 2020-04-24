#include <memory>
#include <vector>

#include "heightmap.h"

struct vec3 {
  float x, y, z;
};

struct ivec3 {
  int x, y, z;
};

class Triangulator {
public:
  Triangulator(const Heightmap &heightmap);
  
  void Run(
      const float maxError,
      const int maxTriangles,
      const int maxPoints,
      const std::vector<int> ind_x,
      const std::vector<int> ind_y);
  
  int NumPoints() const {
    return m_Points.size();
  }
  
  int NumTriangles() const {
    return m_Queue.size();
  }
  
  float Error() const;
  
  std::vector<vec3> Points() const;
  
  std::vector<ivec3> Triangles() const;
  
private:
  void Flush();
  
  void Step();
  
  int AddPoint(const ivec2 point);
  
  int AddTriangle(
      const int a, const int b, const int c,
      const int ab, const int bc, const int ca,
      int e);
  
  void Legalize(const int a);
  
  void QueuePush(const int t);
  int QueuePop();
  int QueuePopBack();
  void QueueRemove(const int t);
  bool QueueLess(const int i, const int j) const;
  void QueueSwap(const int i, const int j);
  void QueueUp(const int j0);
  bool QueueDown(const int i0, const int n);
  
  Heightmap m_Heightmap;
  
  std::vector<ivec2> m_Points;
  
  std::vector<int> m_Triangles;
  std::vector<int> m_Halfedges;
  
  std::vector<ivec2> m_Candidates;
  std::vector<float> m_Errors;
  std::vector<int> m_QueueIndexes;
  
  std::vector<int> m_Queue;
  
  std::vector<int> m_Pending;
};
