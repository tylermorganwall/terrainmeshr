#'@title Make Reduced Matrix Size
#'
#'@description Uses Delaney triangulation to reduce the size of the matrix
#'
#'@param heightmap A two-dimensional matrix, where each entry in the matrix is the elevation at that point.
#'All points are assumed to be evenly spaced.
#'@param maxError Default `0`.
#'@param maxTriangles Default `0`, which turns off this setting.
#'@keywords internal
triangulate_matrix_na = function(heightmap, maxError=0, maxTriangles = 0) {
  testmat = matrix(0, ncol=ncol(heightmap), nrow=nrow(heightmap))
  edge_indices = do.call(rbind, get_edge_indices(heightmap, is.na(heightmap)))
  unique_edge_indices = unique(edge_indices)
  unique_edge_indices = unique_edge_indices[order(unique_edge_indices[,1],unique_edge_indices[,2]),]
  create_edge_indices = function(min_edge, max_edge) {
    edge1 = seq(min_edge[1], max_edge[1])
    mat_top = matrix(c(edge1, rep(min_edge[2],length(edge1))), ncol=2)
    mat_bot = matrix(c(edge1, rep(max_edge[2],length(edge1))), ncol=2)
    if(min_edge[2]+1 <=  max_edge[2]-1) {
      edge2 = seq(min_edge[2]+1, max_edge[2]-1)
      mat_left  = matrix(c(rep(min_edge[1], length(edge2)), edge2), ncol=2)
      mat_right = matrix(c(rep(max_edge[1], length(edge2)), edge2), ncol=2)
      return(rbind(mat_top, mat_bot, mat_left, mat_right))
    } else {
      return(rbind(mat_top, mat_bot))
    }
  }
  polymin = list()
  polymax = list()
  counter = 1
  returnval = list()
  returnval$min = do.call(rbind,polymin)
  returnval$max = do.call(rbind,polymax)
  return(returnval)
}
