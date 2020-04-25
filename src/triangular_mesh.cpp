#include "triangulator.h"

inline List vec3_to_rcpp(const std::vector<vec3> input) {
  std::vector<double> x, y, z;
  for(size_t i = 0; i < input.size(); i++) {
    x.push_back((double)input[i].x);
    y.push_back((double)input[i].y);
    z.push_back((double)input[i].z);
  }
  NumericVector xx = wrap(x);
  NumericVector yy = wrap(y);
  NumericVector zz = wrap(z);
  xx.attr("dim") = Dimension(input.size(), 1);
  yy.attr("dim") = Dimension(input.size(), 1);
  zz.attr("dim") = Dimension(input.size(), 1);
  return(List::create(_["x"] = wrap(x), _["y"] = wrap(y), _["z"] = wrap(z)));
}

inline List ivec3_to_rcpp(const std::vector<ivec3> input) {
  std::vector<int> x, y, z;
  for(size_t i = 0; i < input.size(); i++) {
    x.push_back(input[i].x);
    y.push_back(input[i].y);
    z.push_back(input[i].z);
  }
  IntegerVector xx = wrap(x);
  IntegerVector yy = wrap(y);
  IntegerVector zz = wrap(z);
  xx.attr("dim") = Dimension(input.size(), 1);
  yy.attr("dim") = Dimension(input.size(), 1);
  zz.attr("dim") = Dimension(input.size(), 1);
  return(List::create(_["x"] = xx, _["y"] = yy, _["z"] = zz));
}

// [[Rcpp::export]]
List triangulate_matrix_rcpp(NumericMatrix heightmap, float maxError, int maxTriangles) {
  std::vector<float> data = as<std::vector<float> >(heightmap) ;
  std::vector<int> ind_x;
  std::vector<int> ind_y;

  Heightmap hm = Heightmap(heightmap.nrow(), heightmap.ncol(), data);
  Triangulator tri(hm);
  tri.Run(maxError, maxTriangles, maxTriangles, ind_x, ind_y);
  auto points = tri.Points();
  auto triangles = tri.Triangles();
  auto error = tri.Error();
  List verts = vec3_to_rcpp(points);
  List indices = ivec3_to_rcpp(triangles);
  return(List::create(_["vertices"] = verts, _["indices"] = indices, _["error"] = error));
}
