#ifndef CLASSES_H
#define CLASSES_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

class Vect3 {
private:
    float _x;
    float _y;
    float _z;

public:
    Vect3();
    Vect3(float x, float y, float z);
    Vect3(const Vect3& other);

    static const Vect3 ZERO;
    static const Vect3 ONE;

    float getX() const;
    float getY() const;
    float getZ() const;

    bool operator==(const Vect3& other) const;
    friend Vect3 operator-(const Vect3& lhs, const Vect3& rhs);
    friend Vect3 operator-(const Vect3& rhs);
    friend Vect3 operator+(const Vect3& lhs, const Vect3& rhs);
    friend Vect3 operator/(const Vect3& lhs, float rhs);
    Vect3& operator+=(const Vect3& other);
    friend std::string operator+(const std::string& lhs, const Vect3 rhs);
    friend std::ostream& operator<<(std::ostream& os, const Vect3& vertex);
};

namespace std {
    template <>
    struct hash<Vect3> {
        size_t operator()(const Vect3& vertex) const;
    };
}

class Face {
private:
    std::vector<Vect3> _verteces;
    Vect3 _normal;

public:
    Face();
    Face(Vect3& normal);

    void push_back(Vect3& vertex);
    std::vector<Vect3> getVerts();
    Vect3 getNorm();
};

class Model {
private:
    std::string _name;
    std::vector<Vect3> _vertices;
    std::vector<Vect3> _vertexNormals;
    std::vector<Face> _faces;
    std::unordered_map<Vect3, unsigned long> _vertexIndexMap;
    std::unordered_map<Vect3, unsigned long> _normalIndexMap;
    Vect3 _centerVector = Vect3::ZERO;

public:
    explicit Model(std::ifstream &objectFile, bool center=true);

    std::string factory(const std::string& command, std::istringstream &stream);
    std::string toOBJ(const std::string& filePath="");

    void readFile(std::ifstream &objectFile);

    void centering();
};

class Canvas{
 private:
    int _rows;
    int _cols;
    std::string _strokes; //Like in painting

 public:
  Canvas(int rows, int cols);
};

class Camera {
private:
    Vect3 _origin;
    Canvas _canvas;
    void init();
public:
  Camera(Vect3& origin);
  Camera(const Model& model);
  Canvas getResolution();
};

#endif // CLASSES_H
