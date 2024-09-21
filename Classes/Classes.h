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
#include <list>
#include <memory>
#include <chrono>
#include <thread>
#include <termios.h>
#include <fcntl.h>

namespace std {
    template <>
    struct hash<Eigen::Vector3d> {
        size_t operator()(const Eigen::Vector3d& vertex) const;
    };
}


typedef std::shared_ptr<Eigen::Vector3d> v3DPtr;
typedef std::array<v3DPtr, 3> triangle;

class Face {
private:
    std::vector<v3DPtr> _v;
    std::vector<triangle> _triangles;
    v3DPtr _normalPtr;
    Eigen::Vector3d _pIntersect;

public:
    Face();
    Face(v3DPtr normal);
    void push_back(const v3DPtr& vertex);
    std::vector<v3DPtr> getVerts();
    v3DPtr getNorm();
    bool intersectGEO(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir);
    bool triRayIntersectGEO (const Eigen::Vector3d &orig, const Eigen::Vector3d
    &dir, const triangle &tri);
    void triangulate ();
    bool intersectMT (const Eigen::Vector3d &orig, const Eigen::Vector3d &dir);
    bool triRayIntersectMT (const Eigen::Vector3d &orig, const Eigen::Vector3d
    &dir, const triangle &tri);
  Eigen::Vector3d &getIntersect ();
};


class Model {
private:
    std::string _name;
    std::vector<v3DPtr> _vertices;
    std::vector<v3DPtr> _vertexNormals;
    std::vector<Face> _faces;
    std::unordered_map<v3DPtr, unsigned long> _vertexIndexMap;
    std::unordered_map<v3DPtr, unsigned long> _normalIndexMap;
    Eigen::Vector3d _centerVector = Eigen::Vector3d(0,0,0);

public:
    explicit Model(std::ifstream &objectFile, bool center=true);
    std::string factory(const std::string& command, std::istringstream &stream);
    std::string toOBJ(const std::string& filePath="");
    void readFile(std::ifstream &objectFile);
    void centering();
    bool intersect (const Eigen::Vector3d &orig, const Eigen::Vector3d &dir, Eigen::Vector3d &normal, Eigen::Vector3d &P) const;
    void rotate(double theta);
};

class Canvas{
 private:
  float _aspectRatio;
  //Like in painting
  float _rows;
  float _cols;
  std::string _strokes;
 public:
  Canvas(int rows, int cols);
  float rows() const;
  float cols() const;
  float getNDCx(int x);
  float getNDCy(int y);
  void draw(char c, int x, int y);
  void draw(char c, int i);
  void clear();

  char& operator[] (size_t index);
  const char& operator[] (size_t index) const;
  friend std::ostream& operator<<(std::ostream& os, Canvas& c);

};

class Camera {
private:
  const Model& _model;
  Eigen::Vector3d _origin;
  Eigen::Vector3d _cPoint0;
  Eigen::Vector3d _cVec1;
  Eigen::Vector3d _cVec2;
  Eigen::Vector3d _lightSource;
  std::vector<std::tuple<double, int, int>> _strokes;
  Canvas _canvas;

public:
  Camera(const Model& model);
  Camera(const Model& model, Eigen::Vector3d origin);
  Canvas getResolution();
  void rayTrace();
  void print();
  //TODO add a change camera position option
  char getStroke (double brightness);
  void draw ();
};

#endif // CLASSES_H
