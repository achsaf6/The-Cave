#include <fstream>
#include <sstream>
#include <iostream>
#include "Model.h"

// Constructor that reads the object file and optionally centers the model
Model::Model(std::ifstream &objectFile, bool center) {
  readFile(objectFile);
  if (center) {
    centering();
  }
}

// Center the model around the origin
void Model::centering() {
  Eigen::Vector3d antiVect = -_centerVector / static_cast<float>(_vertices.size());
  for (v3DPtr &v : _vertices) {
    *v += antiVect;
  }
}

// Read the object file
void Model::readFile(std::ifstream &objectFile) {
  std::string line;
  while (std::getline(objectFile, line)) {
    std::istringstream command(line);
    std::string token;

    while (command >> token) {
      if (token == "#") break; // Ignore comments
      Model::factory(token, command);
    }
  }
}

// Factory method to handle different commands in the object file
std::string Model::factory(const std::string &command, std::istringstream &stream) {
  if (command == "o") {
    stream >> _name;
    return "Name";
  }
  if (command == "v") {
    float x, y, z;
    stream >> x >> y >> z;
    auto v = std::make_shared<Eigen::Vector3d>(x, y, z);
    _centerVector += *v;
    _vertices.push_back(v);
    _vertexIndexMap[v] = _vertices.size();
    return "Eigen::Vector3d";
  }
  if (command == "vn") {
    float x, y, z;
    stream >> x >> y >> z;
    auto v = std::make_shared<Eigen::Vector3d>(x, y, z);
    _vertexNormals.push_back(v);
    _normalIndexMap[v] = _vertexNormals.size();
    return "VertexNormals";
  }
  if (command == "f") {
    std::string faceVex;
    std::vector<int> vert, vertTex, vertNorm;

    while (stream >> faceVex) {
      std::istringstream faceStream(faceVex);
      std::string temp;

      // Parse vertex, texture, and normal indices
      std::getline(faceStream, temp, '/');
      vert.emplace_back(std::stoi(temp));
      std::getline(faceStream, temp, '/');
      vertTex.emplace_back(temp.empty() ? 0 : std::stoi(temp)); // Handle optional texture
      std::getline(faceStream, temp, ' ');
      vertNorm.emplace_back(std::stoi(temp));
    }

    Face f(_vertexNormals[vertNorm[0] - 1]); // Assuming at least one normal
    for (int index : vert) {
      f.push_back(_vertices[index - 1]); // Adjust for 0-based indexing
    }
    f.triangulate();
    _faces.push_back(f);
    return "Faces";
  }
  return "Ignored";
}

// String conversion for Eigen::Vector3d
std::string operator+(std::string lhs, const Eigen::Vector3d &rhs) {
  return lhs + std::to_string(rhs.x()) + " " + std::to_string(rhs.y()) + " " + std::to_string(rhs.z());
}

// Export the model to an OBJ file format
std::string Model::toOBJ(const std::string &filePath) {
  bool shouldExport = !filePath.empty();
  std::ostringstream fileStream;

  fileStream << "o " << _name << "\n";
  for (const auto &vertex : _vertices) {
    fileStream << "v " << *vertex << "\n";
  }
  for (const auto &normal : _vertexNormals) {
    fileStream << "vn " << *normal << "\n";
  }
  for (Face face : _faces) {
    fileStream << "f ";
    for (const auto &v : face.getVerts()) {
      fileStream << std::to_string(_vertexIndexMap[v]) << "//" << std::to_string(_normalIndexMap[face.getNorm()]) << " ";
    }
    fileStream << "\n";
  }

  if (shouldExport) {
    std::ofstream myFile(filePath);
    if (myFile.is_open()) {
      myFile << fileStream.str();
      myFile.close();
    } else {
      // Handle error: unable to open file
      std::cerr << "Error: Unable to open file " << filePath << std::endl;
    }
  }
  return fileStream.str();
}

// Ray intersection check with the model
bool Model::intersect(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir, Eigen::Vector3d &normal, Eigen::Vector3d &P) const {
  double minDist = INFINITY;
  bool flag = false;

  for (Face face : _faces) {
    if (face.intersectMT(orig, dir) && (face.getIntersect() - orig).norm() < minDist) {
      P = face.getIntersect();
      minDist = (P - orig).norm();
      flag = true;
      normal = *face.getNorm();
    }
  }
  return flag; // Return whether an intersection occurred
}

// Rotate the model around the Z-axis
void Model::rotate(double theta) {
  Eigen::Matrix3d rot;
  rot << cos(theta), -sin(theta), 0,
      sin(theta), cos(theta),  0,
      0,          0,           1;

  for (v3DPtr &v : _vertices) {
    *v = rot * (*v);
  }
  for (v3DPtr &n : _vertexNormals) {
    *n = rot * (*n);
  }
}
