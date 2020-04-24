#'@title Make Reduced Matrix Size
#'
#'@description Uses Delaney triangulation to reduce the size of the matrix
#'
#'@param heightmap A two-dimensional matrix, where each entry in the matrix is the elevation at that point.
#'All points are assumed to be evenly spaced.
#'@param maxError Default `0`.
#'@param maxTriangles Default `0`, which turns off this setting.
#'@export
triangulate_matrix = function(heightmap, maxError=0, maxTriangles = 0) {
  tri_info = triangulate_matrix_rcpp(heightmap, maxError, maxTriangles)
  ind = do.call(cbind,tri_info$indices) + 1
  verts = do.call(cbind,tri_info$vertices)
  full_vertices = verts[as.vector(t(ind)),]
  return(full_vertices)
}
