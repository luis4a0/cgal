/*!
\ingroup PkgSurfaceParameterizationConcepts
\cgalConcept

`Parameterizer_3` is a concept of parameterization object for a given type
of mesh, `TriangleMesh`, which must be a model of the `FaceGraph` concept.

Border parameterizers are also models of this concept but they only parameterize
the border of a given mesh.

Creation
--------------

Construction and destruction are undefined.

\cgalHasModel `CGAL::Surface_mesh_parameterization::Fixed_border_parameterizer_3<TriangleMesh, BorderParameterizer_3, SparseLinearAlgebraTraits_d>`
\cgalHasModel `CGAL::Surface_mesh_parameterization::ARAP_parameterizer_3<TriangleMesh, BorderParameterizer_3, SparseLinearAlgebraTraits_d>`
\cgalHasModel `CGAL::Surface_mesh_parameterization::Barycentric_mapping_parameterizer_3<TriangleMesh, BorderParameterizer_3, SparseLinearAlgebraTraits_d>`
\cgalHasModel `CGAL::Surface_mesh_parameterization::Discrete_authalic_parameterizer_3<TriangleMesh, BorderParameterizer_3, SparseLinearAlgebraTraits_d>`
\cgalHasModel `CGAL::Surface_mesh_parameterization::Discrete_conformal_map_parameterizer_3<TriangleMesh, BorderParameterizer_3, SparseLinearAlgebraTraits_d>`
\cgalHasModel `CGAL::Surface_mesh_parameterization::LSCM_parameterizer_3<TriangleMesh, BorderParameterizer_3, SparseLinearAlgebraTraits_d>`
\cgalHasModel `CGAL::Surface_mesh_parameterization::Mean_value_coordinates_parameterizer_3<TriangleMesh, BorderParameterizer_3, SparseLinearAlgebraTraits_d>`
\cgalHasModel `CGAL::Surface_mesh_parameterization::Circular_border_parameterizer_3<TriangleMesh>`
\cgalHasModel `CGAL::Surface_mesh_parameterization::Square_border_parameterizer_3<TriangleMesh>`
\cgalHasModel `CGAL::Surface_mesh_parameterization::Two_vertices_parameterizer_3<TriangleMesh>`

*/

class Parameterizer_3
{
public:

  /// \name Types
  /// @{

  /// A given polygon mesh type, TriangleMesh, which is a model of the `FaceGraph` concept.
  typedef unspecified_type TriangleMesh;
  typedef boost::graph_traits<TriangleMesh>::halfedge_descriptor halfedge_descriptor;
  /// @}


  /// \name Operations
  /// @{

  /// Assign a 2D position (i.e.\ a `(u, v)` pair) on the shape to (some of)
  /// the vertices of the mesh. Mark them as <I>parameterized</I>.
  ///
  /// \tparam VertexUVmap must be a property map that associates a %Point_2
  ///         (type deduced from `TriangleMesh` using the `Kernel_traits`)
  ///         to a `vertex_descriptor` (type deduced by the graph traits
  ///         of `TriangleMesh`).
  /// \tparam VertexIndexMap must be a property map that associates a unique integer index
  ///         to a `vertex_descriptor` (type deduced by the graph traits of `TriangleMesh`).
  /// \tparam VertexParameterizedMap must be a property map that associates a Boolean
  ///         to a `vertex_descriptor` (type deduced by the graph traits of `TriangleMesh`).
  ///
  /// \param mesh a triangulated surface.
  /// \param bhd a halfedge descriptor on the boundary of `mesh`.
  /// \param uvmap an instanciation of the class `VertexUVmap`.
  /// \param vimap an instanciation of the class `VertexIndexMap`.
  /// \param vpmap an instanciation of the class `VertexParameterizedMap`.
  ///
  /// \pre `mesh` must be a surface with one connected component.
  /// \pre `mesh` must be a triangular mesh.
  /// \pre The vertices must be indexed (`vimap` must be initialized)
  ///
  template<typename VertexUVMap, typename VertexIndexMap, typename VertexParameterizedMap>
  Error_code parameterize(const TriangleMesh& mesh,
                          halfedge_descriptor bhd,
                          VertexUVMap uvmap,
                          VertexIndexMap vimap,
                          VertexParameterizedMap vpmap);

  /// @}

}; /* end Parameterizer_3 */

