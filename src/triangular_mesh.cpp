#define R_NO_REMAP

#include <R.h>
#include <R_ext/Rdynload.h>
#include <Rinternals.h>

#include <vector>

#include "triangulator.h"

inline SEXP vec3_to_list(const std::vector<vec3> &input, int &protects) {
  const R_xlen_t n = static_cast<R_xlen_t>(input.size());
  SEXP x = PROTECT(Rf_allocVector(REALSXP, n));
  ++protects;
  SEXP y = PROTECT(Rf_allocVector(REALSXP, n));
  ++protects;
  SEXP z = PROTECT(Rf_allocVector(REALSXP, n));
  ++protects;

  double *xd = REAL(x);
  double *yd = REAL(y);
  double *zd = REAL(z);
  for (R_xlen_t i = 0; i < n; ++i) {
    xd[i] = static_cast<double>(input[i].x);
    yd[i] = static_cast<double>(input[i].y);
    zd[i] = static_cast<double>(input[i].z);
  }

  SEXP list = PROTECT(Rf_allocVector(VECSXP, 3));
  ++protects;
  SET_VECTOR_ELT(list, 0, x);
  SET_VECTOR_ELT(list, 1, y);
  SET_VECTOR_ELT(list, 2, z);

  SEXP names = PROTECT(Rf_allocVector(STRSXP, 3));
  ++protects;
  SET_STRING_ELT(names, 0, Rf_mkChar("x"));
  SET_STRING_ELT(names, 1, Rf_mkChar("y"));
  SET_STRING_ELT(names, 2, Rf_mkChar("z"));
  Rf_setAttrib(list, R_NamesSymbol, names);

  return list;
}

inline SEXP ivec3_to_list(const std::vector<ivec3> &input, int &protects) {
  const R_xlen_t n = static_cast<R_xlen_t>(input.size());
  SEXP x = PROTECT(Rf_allocVector(INTSXP, n));
  ++protects;
  SEXP y = PROTECT(Rf_allocVector(INTSXP, n));
  ++protects;
  SEXP z = PROTECT(Rf_allocVector(INTSXP, n));
  ++protects;

  int *xi = INTEGER(x);
  int *yi = INTEGER(y);
  int *zi = INTEGER(z);
  for (R_xlen_t i = 0; i < n; ++i) {
    xi[i] = input[i].x;
    yi[i] = input[i].y;
    zi[i] = input[i].z;
  }

  SEXP list = PROTECT(Rf_allocVector(VECSXP, 3));
  ++protects;
  SET_VECTOR_ELT(list, 0, x);
  SET_VECTOR_ELT(list, 1, y);
  SET_VECTOR_ELT(list, 2, z);

  SEXP names = PROTECT(Rf_allocVector(STRSXP, 3));
  ++protects;
  SET_STRING_ELT(names, 0, Rf_mkChar("x"));
  SET_STRING_ELT(names, 1, Rf_mkChar("y"));
  SET_STRING_ELT(names, 2, Rf_mkChar("z"));
  Rf_setAttrib(list, R_NamesSymbol, names);

  return list;
}

extern "C" SEXP C_triangulate_matrix(SEXP heightmapSEXP,
                                     SEXP maxErrorSEXP,
                                     SEXP maxTrianglesSEXP) {
  if (!Rf_isMatrix(heightmapSEXP)) {
    Rf_error("`heightmap` must be a numeric matrix");
  }

  int protect_count = 0;
  SEXP hm_real = heightmapSEXP;
  if (!Rf_isReal(heightmapSEXP)) {
    hm_real = PROTECT(Rf_coerceVector(heightmapSEXP, REALSXP));
    ++protect_count;
  }

  const int nrow = Rf_nrows(heightmapSEXP);
  const int ncol = Rf_ncols(heightmapSEXP);
  const double *hm_data = REAL(hm_real);
  std::vector<float> data(static_cast<size_t>(nrow) * static_cast<size_t>(ncol));
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = static_cast<float>(hm_data[i]);
  }

  const float maxError = static_cast<float>(Rf_asReal(maxErrorSEXP));
  const int maxTriangles = Rf_asInteger(maxTrianglesSEXP);

  std::vector<int> ind_x;
  std::vector<int> ind_y;

  Heightmap hm(nrow, ncol, data);
  Triangulator tri(hm);
  tri.Run(maxError, maxTriangles, maxTriangles, ind_x, ind_y);

  const std::vector<vec3> points = tri.Points();
  const std::vector<ivec3> triangles = tri.Triangles();
  const double error = tri.Error();

  SEXP verts = PROTECT(vec3_to_list(points, protect_count));
  ++protect_count;
  SEXP indices = PROTECT(ivec3_to_list(triangles, protect_count));
  ++protect_count;

  SEXP out = PROTECT(Rf_allocVector(VECSXP, 3));
  ++protect_count;
  SET_VECTOR_ELT(out, 0, verts);
  SET_VECTOR_ELT(out, 1, indices);
  SET_VECTOR_ELT(out, 2, Rf_ScalarReal(error));

  SEXP names = PROTECT(Rf_allocVector(STRSXP, 3));
  ++protect_count;
  SET_STRING_ELT(names, 0, Rf_mkChar("vertices"));
  SET_STRING_ELT(names, 1, Rf_mkChar("indices"));
  SET_STRING_ELT(names, 2, Rf_mkChar("error"));
  Rf_setAttrib(out, R_NamesSymbol, names);

  UNPROTECT(protect_count);
  return out;
}

static const R_CallMethodDef callTable[] = {
  {"C_triangulate_matrix", (DL_FUNC) &C_triangulate_matrix, 3},
  {NULL, NULL, 0}
};

extern "C" void R_init_terrainmeshr(DllInfo *dll) {
  R_registerRoutines(dll, NULL, callTable, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
