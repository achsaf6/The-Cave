#ifndef _FACE_H_
#define _FACE_H_

#include <memory>
#include <vector>
#include <Eigen/Dense>

#define EPSILON 0.00000001  // A small value to handle floating-point comparisons

typedef std::shared_ptr<Eigen::Vector3d> v3DPtr;  // Shared pointer type for a 3D vector
typedef std::array<v3DPtr, 3> triangle;  // Array type to represent a triangle as three vertices

/**
 * @brief Class representing a geometric face in 3D space.
 * This class stores vertices, normal vector, and provides methods for
 * triangulation and ray intersection testing.
 */
class Face {
 private:
  std::vector<v3DPtr> _v;  ///< List of vertices making up the face
  std::vector<triangle> _triangles;  ///< List of triangles derived from the face
  v3DPtr _normalPtr;  ///< Pointer to the normal vector of the face
  Eigen::Vector3d _pIntersect;  ///< Point of intersection, if any

 public:
  /**
   * @brief Default constructor for the Face class.
   * Initializes the normal pointer with a new Eigen vector.
   */
  Face();

  /**
   * @brief Constructs a Face object with a specified normal pointer.
   * @param normal A shared pointer to the normal vector of the face.
   */
  Face(v3DPtr normal);

  /**
   * @brief Adds a vertex pointer to the face.
   * @param vertex A shared pointer to a vertex.
   */
  void push_back(const v3DPtr& vertex);

  /**
   * @brief Retrieves the vertices of the face.
   * @return A vector of shared pointers to the vertices.
   */
  std::vector<v3DPtr> getVerts();

  /**
   * @brief Retrieves the normal of the face.
   * @return A shared pointer to the normal vector.
   */
  v3DPtr getNorm();

  /**
   * @brief Checks for ray intersection with the face using geometric methods.
   * @param orig The origin point of the ray.
   * @param dir The direction of the ray.
   * @return True if the ray intersects any triangle of the face, otherwise false.
   */
  bool intersectGEO(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir);

  /**
   * @brief Checks if a ray intersects with a triangle using geometric methods.
   * @param orig The origin point of the ray.
   * @param dir The direction of the ray.
   * @param tri The triangle to test for intersection.
   * @return True if the ray intersects the triangle, otherwise false.
   */
  bool triRayIntersectGEO(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir, const triangle &tri);

  /**
   * @brief Triangulates the face into a list of triangles.
   * Converts the face into triangles. Currently, this method works
   * under the assumption that the face is convex.
   */
  void triangulate();

  /**
   * @brief Checks for ray intersection with the face using a more optimized method.
   * @param orig The origin point of the ray.
   * @param dir The direction of the ray.
   * @return True if the ray intersects any triangle of the face, otherwise false.
   */
  bool intersectMT(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir);

  /**
   * @brief Checks if a ray intersects with a triangle using a more optimized method.
   * @param orig The origin point of the ray.
   * @param dir The direction of the ray.
   * @param tri The triangle to test for intersection.
   * @return True if the ray intersects the triangle, otherwise false.
   */
  bool triRayIntersectMT(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir, const triangle &tri);

  /**
   * @brief Retrieves the intersection point of the ray with the face.
   * @return The intersection point.
   */
  Eigen::Vector3d &getIntersect();
};

#endif //_FACE_H_
