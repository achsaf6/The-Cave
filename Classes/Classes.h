#ifndef CLASSES_H
#define CLASSES_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>

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
    std::string toOBJ(std::string filePath, bool export_ = true);

    void readFile(std::ifstream &objectFile);

    void centering();
};

class Camera {
private:
    Vect3 _origin;
    Vect3 _dir;
    std::vector<std::vector<Vect3>> _canvas;

public:
    Camera(const Vect3& origin, const Vect3& dir = Vect3());
};

#endif // CLASSES_H
