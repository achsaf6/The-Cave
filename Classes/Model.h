#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>      // Include for std::shared_ptr
#include "Face.h"

// Define a shared pointer type for Eigen::Vector3d
typedef std::shared_ptr<Eigen::Vector3d> v3DPtr;

class Model {
 private:
  std::string _name;                          // Name of the model
  std::vector<v3DPtr> _vertices;              // Vertices of the model
  std::vector<v3DPtr> _vertexNormals;         // Normals for each vertex
  std::vector<Face> _faces;                    // Faces of the model
  std::unordered_map<v3DPtr, unsigned long> _vertexIndexMap; // Mapping from vertex pointers to their indices
  std::unordered_map<v3DPtr, unsigned long> _normalIndexMap; // Mapping from normal pointers to their indices
  Eigen::Vector3d _centerVector = Eigen::Vector3d(0, 0, 0); // Center of the model

 public:
  // Constructor to initialize the model from an object file, optionally centering it
  explicit Model(std::ifstream &objectFile, bool center = true);

  // Factory method to parse commands from the object file
  std::string factory(const std::string &command, std::istringstream &stream);

  // Export the model to an OBJ file format
  std::string toOBJ(const std::string &filePath = "");

  // Read the object file
  void readFile(std::ifstream &objectFile);

  // Center the model around the origin
  void centering();

  // Check for ray intersection with the model
  bool intersect(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir, Eigen::Vector3d &normal, Eigen::Vector3d &P) const;

  // Rotate the model around the Z-axis
  void rotate(double theta);
};

#endif //_MODEL_H_
