#include <list>
#include "Face.h"

/**
 * @brief Default constructor for the Face class.
 * Initializes the normal pointer with a new Eigen vector.
 */
Face::Face() : _normalPtr(std::make_shared<Eigen::Vector3d>()) {}

/**
 * @brief Constructs a Face object with a specified normal pointer.
 * @param normalPtr A shared pointer to the normal vector of the face.
 */
Face::Face(v3DPtr normalPtr) : _normalPtr(std::move(normalPtr)) {}

/**
 * @brief Adds a vertex pointer to the face.
 * @param vertexPtr A shared pointer to a vertex.
 */
void Face::push_back(const v3DPtr& vertexPtr) {
  _v.push_back(vertexPtr);  // Add vertex to the list
}

/**
 * @brief Triangulates the face into a list of triangles.
 * Converts the face into triangles. Currently, this method works
 * under the assumption that the face is convex.
 */
void Face::triangulate() {
  // Converts a face into a list of triangles
  std::list<v3DPtr> tempV(_v.begin(), _v.end());
  bool popFlag = true;
  v3DPtr v0, v1, v2;

  while (tempV.size() != 3) {
    if (popFlag) {
      v0 = tempV.front();
      tempV.pop_front();
    } else {
      v0 = tempV.back();
      tempV.pop_back();
    }
    v1 = tempV.front();
    v2 = tempV.back();
    _triangles.push_back({v0, v1, v2});
    popFlag = !popFlag;  // Alternate between front and back
  }

  // Handle the last three vertices
  v0 = tempV.front();
  tempV.pop_front();
  v1 = tempV.front();
  tempV.pop_front();
  v2 = tempV.front();
  _triangles.push_back({v0, v1, v2});
}

/**
 * @brief Retrieves the vertices of the face.
 * @return A vector of shared pointers to the vertices.
 */
std::vector<v3DPtr> Face::getVerts() {
  return _v;  // Return the list of vertex pointers
}

/**
 * @brief Retrieves the normal of the face.
 * @return A shared pointer to the normal vector.
 */
v3DPtr Face::getNorm() {
  return _normalPtr;  // Return the normal vector pointer
}

/**
 * @brief Checks if a ray intersects with a triangle using geometric methods.
 * @param orig The origin point of the ray.
 * @param dir The direction of the ray.
 * @param tri The triangle to test for intersection.
 * @return True if the ray intersects the triangle, otherwise false.
 */
bool Face::triRayIntersectGEO(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir, const triangle &tri) {
  double D = -_normalPtr->dot(*_v[0]);
  double t = -(_normalPtr->dot(orig) + D) / _normalPtr->dot(dir);
  const Eigen::Vector3d P = orig + t * dir;

  Eigen::Vector3d edge0 = *(tri[1]) - *(tri[0]);
  Eigen::Vector3d edge1 = *(tri[2]) - *(tri[1]);
  Eigen::Vector3d edge2 = *(tri[0]) - *(tri[2]);
  Eigen::Vector3d C0 = P - *(tri[0]);
  Eigen::Vector3d C1 = P - *(tri[1]);
  Eigen::Vector3d C2 = P - *(tri[2]);

  // Check if the point P is inside the triangle using the normal
  return _normalPtr->dot(edge0.cross(C0)) >= 0 &&
         _normalPtr->dot(edge1.cross(C1)) >= 0 &&
         _normalPtr->dot(edge2.cross(C2)) >= 0;
}

/**
 * @brief Checks for ray intersection with the face.
 * @param orig The origin point of the ray.
 * @param dir The direction of the ray.
 * @return True if the ray intersects any triangle of the face, otherwise false.
 */
bool Face::intersectGEO(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir) {
  if (_normalPtr->dot(orig - dir) == 0) {
    return false;  // Ray is parallel to the face
  }

  for (const triangle &tri : _triangles) {
    if (triRayIntersectGEO(orig, dir, tri)) {
      return true;  // Intersection found
    }
  }
  return false;  // No intersection
}

/**
 * @brief Checks if a ray intersects with a triangle using a more optimized method.
 * @param orig The origin point of the ray.
 * @param dir The direction of the ray.
 * @param tri The triangle to test for intersection.
 * @return True if the ray intersects the triangle, otherwise false.
 */
bool Face::triRayIntersectMT(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir, const triangle &tri) {
  Eigen::Vector3d AB = *(tri[1]) - *(tri[0]);
  Eigen::Vector3d AC = *(tri[2]) - *(tri[0]);
  Eigen::Vector3d pvec = dir.cross(AC);
  double det = AB.dot(pvec);

  if (abs(det) < EPSILON)  // If the ray and triangle are parallel
    return false;

  double invDet = 1 / det;

  Eigen::Vector3d tvec = orig - *(tri[0]);
  double u = tvec.dot(pvec) * invDet;
  if (u < 0 || u > 1)
    return false;

  Eigen::Vector3d qvec = tvec.cross(AB);
  double v = dir.dot(qvec) * invDet;
  if (v < 0 || u + v > 1)
    return false;

  double t = AC.dot(qvec) * invDet;
  _pIntersect = orig + t * dir;  // Calculate intersection point
  return true;
}

/**
 * @brief Checks for ray intersection with the face using a more optimized method.
 * @param orig The origin point of the ray.
 * @param dir The direction of the ray.
 * @return True if the ray intersects any triangle of the face, otherwise false.
 */
bool Face::intersectMT(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir)
{
  for (const triangle &tri : _triangles) {
    if (triRayIntersectMT(orig, dir, tri))
      return true;  // Intersection found
  }
  return false;  // No intersection
}

/**
 * @brief Retrieves the intersection point of the ray with the face.
 * @return The intersection point.
 */
Eigen::Vector3d &Face::getIntersect() {
  return _pIntersect;  // Return the intersection point
}
