#'@title Triangulate a Height Map
#'
#'@description Uses Delaney triangulation to approximate a rectangular height field (in
#'matrix form) with constraints (either maximum allowable error, or a maximum number of triangles).
#'Increasing the error limit will result in a courser approximation, but fewer triangles in the model.
#'For many models (particularly those with large, flat regions or smooth changes in height),
#'this can result in significant reductions in model size with no perceptual loss in
#'terrain surface quality.
#'
#'@param heightmap A two-dimensional matrix, where each entry in the matrix is the elevation at that point.
#'All points are assumed to be evenly spaced.
#'@param maxError Default `0.0001`. Maximum error allowed in triangulating the height map.
#'@param maxTriangles Default `0`, which turns off this setting (and only uses the `max_error` arg).
#'Otherwise, specifies the maximum number of triangles when triangulating the height map.
#'@param y_up Default `TRUE`. Which axis is "upwards" in the return matrix. If `FALSE`, `z` is up.
#'@param start_index Default `1`. The offset to the first `x` and `z` indices.
#'@param verbose Default `FALSE`. Prints reduction in number of triangles/max error.
#'@return Returns a matrix of vertices and IDs for each triangle.
#'@export
#'@examples
#' #Let's triangulate the built-in `volcano` dataset.
#'
#' #Helper function to plot polygons over an `image()` plot.
#' plot_polys = function(tri_matrix) {
#'   #reverse orienation for `image`
#'   tri_matrix[,3] = max(tri_matrix[,3])-tri_matrix[,3]+1
#'   for(i in seq_len(nrow(tri_matrix)/3)) {
#'     polypath(tri_matrix[(3*(i-1)+1):(3*i), c(1,3)])
#'   }
#' }
#'
#' #Here, we don't accept any error, but still triangulate
#' tris = triangulate_matrix(volcano, maxError = 0, verbose = TRUE)
#' image(x=1:nrow(volcano), y = 1:ncol(volcano), volcano)
#' plot_polys(tris)
#'
#' #Let's increase the allowable error:
#' tris = triangulate_matrix(volcano, maxError = 1, verbose = TRUE)
#' image(x=1:nrow(volcano), y = 1:ncol(volcano), volcano)
#' plot_polys(tris)
#'
#' #Increase it again
#' tris = triangulate_matrix(volcano, maxError = 10, verbose = TRUE)
#' image(x=1:nrow(volcano), y = 1:ncol(volcano), volcano)
#' plot_polys(tris)
#'
#' #Here, we set an allowable number of triangles instead, using exactly 20 triangles:
#' tris = triangulate_matrix(volcano, maxTriangles = 20, verbose = TRUE)
#' image(x=1:nrow(volcano), y = 1:ncol(volcano), volcano)
#' plot_polys(tris)
#'
#' #The output of this function can be passed directly to `rgl::triangles3d()` for plotting in 3D.
triangulate_matrix = function(heightmap, maxError=0.0001, maxTriangles = 0, y_up = TRUE,
                              start_index = 1, verbose = FALSE) {
  tri_info = triangulate_matrix_rcpp(heightmap, maxError, maxTriangles)
  ind = do.call(cbind,tri_info$indices) + 1
  verts = do.call(cbind,tri_info$vertices)
  error = tri_info$error
  full_vertices = verts[as.vector(t(ind)),]
  full_vertices[,1] = full_vertices[,1] + start_index
  full_vertices[,2] = full_vertices[,2] + start_index
  ids = seq_len(nrow(full_vertices)/3)
  id_vec = unlist(lapply(ids,rep,3))
  if(y_up) {
    full_vertices = full_vertices[,c(1,3,2)]
  }
  full_vertices = cbind(full_vertices,id_vec)
  if(verbose) {
    base_triangles =ncol(heightmap) * nrow(heightmap) * 2
    message(sprintf("%0.1f%% reduction: Number of triangles reduced from %d to %d. Error: %f",
                    (1-(nrow(full_vertices)/3)/base_triangles)*100,
                    base_triangles, nrow(full_vertices)/3, error))
  }
  colnames(full_vertices) = c("x","y","z","id")
  return(full_vertices)
}
