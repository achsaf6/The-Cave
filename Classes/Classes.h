#ifndef CLASSES_H
#define CLASSES_H

#include <string>
#include <vector>
#include <unordered_map>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <Eigen/Dense>


namespace std {
    template <>
    struct hash<Eigen::Vector3d> {
        size_t operator()(const Eigen::Vector3d& vertex) const;
    };
}

class Face {
private:
    std::vector<Eigen::Vector3d> _verteces;
    Eigen::Vector3d _normal;

public:
    Face();
    Face(Eigen::Vector3d& normal);

    void push_back(Eigen::Vector3d& vertex);
    std::vector<Eigen::Vector3d> getVerts();
    Eigen::Vector3d getNorm();
};

class Model {
private:
    std::string _name;
    std::vector<Eigen::Vector3d> _vertices;
    std::vector<Eigen::Vector3d> _vertexNormals;
    std::vector<Face> _faces;
    std::unordered_map<Eigen::Vector3d, unsigned long> _vertexIndexMap;
    std::unordered_map<Eigen::Vector3d, unsigned long> _normalIndexMap;
    Eigen::Vector3d _centerVector = Eigen::Vector3d(0,0,0);

public:
    explicit Model(std::ifstream &objectFile, bool center=true);

    std::string factory(const std::string& command, std::istringstream &stream);
    std::string toOBJ(const std::string& filePath="");

    void readFile(std::ifstream &objectFile);

    void centering();
};

class Canvas{
 private:
    float _rows;
    float _cols;
    float _aspectRatio;
    std::string _strokes; //Like in painting

 public:
  Canvas(int rows, int cols);
  float rows() const;
  float cols() const;
  float getNDCx(int x);
  float getNDCy(int y);
  void draw(char c, int x, int y);

  char& operator[] (size_t index);
  const char& operator[] (size_t index) const;
  friend std::ostream& operator<<(std::ostream& os, const Canvas& c);

};

class Camera {
private:
  Eigen::Vector3d _origin;
  Eigen::Vector3d _cPoint0;
  Eigen::Vector3d _cVec1;
  Eigen::Vector3d _cVec2;
  Canvas _canvas;

public:
  Camera(const Model& model);
  Camera(Eigen::Vector3d& origin);
  Canvas getResolution();
  bool solveQuadratic(const float &a, const float &b, const float &c,
                      float &x0, float &x1);
  void rayTrace(int radius);
  void print();

};

#endif // CLASSES_H
