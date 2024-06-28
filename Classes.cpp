#include <cstring>
#include <sstream>
#include "Classes.h"

#include <fstream>
#include <tuple>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include <unordered_map>

using std::string;





/*
ModelObject
    - find center of mass and put it in (0,0,0)
    - all information from .obj
    - rotational functions


*/


/*
TODO obj file:
each line is it's own command
(blank line) - can be ignored
# - comment can be ignored
vt u v [w] - vertex texture command can be ignored

o _name -  object _name of subsequent  (need to look into multiple objects if relevant)
g _name - group _name for subsequent _faces
s i - shading??
usemtl _name - all following _faces will use the '_name' material
        mtllib _name - .mtl files that contain materials
        usemtl _name - use a previously defined material
        vp ____ - free form geometry command ???
l v1 v2 v3 ... - polyline
*/





class Vertex {
private:
    float _x;
    float _y;
    float _z;


public:
    Vertex(float x, float y, float z) {
        _x = x;
        _y = y;
        _z = z;
    }

    float getX() const{
        return _x;
    }

    float getY() const{
        return _y;
    }

    float getZ() const{
        return _z;
    }

    bool operator==(const Vertex& other) const {
        return _x == other._x && _y == other._y && _z == other._z;
    }

    friend string operator+(const string& lhs, const Vertex rhs){
        return lhs + std::to_string(rhs._x) + " " + std::to_string(rhs._y) + " " + std::to_string(rhs._z) + " \n";
    }

    friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
        os << "(" << vertex._x << ", " << vertex._y << ", " << vertex._z << ")";
        return os;
    }

};


namespace std {
    template <>
    struct hash<Vertex> {
        size_t operator()(const Vertex& vertex) const {
            size_t hx = std::hash<float>()(vertex.getX());
            size_t hy = std::hash<float>()(vertex.getY());
            size_t hz = std::hash<float>()(vertex.getZ());
            return hx ^ (hy << 1) ^ (hz << 2); // Combine hash values
        }
    };
}


typedef std::vector<Vertex> Face;



class Model {

private:
    string _name;
    std::vector<Vertex> _vertices;
    std::vector<Vertex> _vertexNormals;
    std::vector<Face> _faces;
    std::unordered_map<Vertex, unsigned long> _vertexIndexMap;


public:
     explicit Model(std::ifstream &objectFile) {
        string line;

        while(getline(objectFile, line)){
            std::istringstream command(line);
            string token;
            string commandRead;
            while (command >> token){
                commandRead = Model::factory(token, command); // commandRead is for debugging
            }
        }

    }


    string factory(const string& command, std::istringstream &stream) {
        if (command == "o") {
            stream >> _name;
            return "Name";
        }
        if (command == "v") {
            float x,y,z;
            stream >> x;
            stream >> y;
            stream >> z;
            Vertex v(x,y,z);
            _vertices.push_back(v);
            _vertexIndexMap[v] = _vertices.size();
            return "Vertex";
        }
        if (command == "vn"){
            float x,y,z;
            stream >> x;
            stream >> y;
            stream >> z;
            _vertexNormals.emplace_back(x, y, z);
            return "VertexNormals";
        }
//    f v1[/vt1][/vn1] v2[/vt2][/vn2] v3[/vt3][/vn3]... each vertext can come with vt and vn commands
//    f v1//vn1 ... ********** values can be negative ********
        if (command == "f"){
            string faceVex;
            std::vector<int> vert;
            std::vector<int> vertTex;
            std::vector<int> vertNorm;

            string temp;
            while (std::getline(stream, temp, '/')){
                vert.emplace_back(std::stoi(temp));
                std::getline(stream, temp, '/');
                vertTex.emplace_back(std::stoi(temp));
                std::getline(stream, temp, ' ');
                vertNorm.emplace_back(std::stoi(temp));
            }

            Face f;
            for (int index: vert){
                f.push_back(_vertices[index-1]);
            }
            _faces.push_back(f);
            return "Faces";
        }
        return "Ignored";
    }

    string toOBJ(string filePath, bool export_ =true) {

        string file  = "o " + _name + "\n";
        for (const auto& vertex : _vertices) {
            file += "v " + vertex;
        }
        for (const auto& normal : _vertexNormals) {
            file += "vn " + normal;
        }
        for (const auto& face : _faces) {
            file += "f ";
            for (const auto& v: face){
                file += std::to_string(_vertexIndexMap[v]) + " ";
            }
            file += "\n";
        }

        if (export_){
            std::ofstream MyFile(filePath);
            MyFile << file;
            MyFile.close();
        }
        return file;
    }
};

class Camera {
private:
    Vertex origin;
    std::vector<std::vector<Vertex>> canvas;

public:
};












