#include <cstring>
#include <sstream>
#include "Classes.h"

#include <fstream>
#include <tuple>
#include <sstream>
#include <vector>
#include <iostream>

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

o name -  object name of subsequent  (need to look into multiple objects if relevant)
g name - group name for subsequent faces
s i - shading??
usemtl name - all following faces will use the 'name' material
        mtllib name - .mtl files that contain materials
        usemtl name - use a previously defined material
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

    friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
        os << "(" << vertex._x << ", " << vertex._y << ", " << vertex._z << ")";
        return os;
    }
};

typedef std::vector<Vertex> Face;



class Model {

private:
    string name;
    std::vector<Vertex> vertices;
    std::vector<Vertex> vertexNormals;
    std::vector<Face> faces;

public:
    explicit Model(std::ifstream &objectFile) {
        string line;

        while(getline(objectFile, line)){
            std::istringstream command(line);
            string token;
            while (command >> token){
                Model::factory(token, command);
            }
        }

    }


    string factory(const string& command, std::istringstream &stream) {
        if (command == "o") {
            stream >> name;
            return "Name";
        }
        if (command == "v") {
            float x,y,z;
            stream >> x;
            stream >> y;
            stream >> z;
            vertices.emplace_back(x,y,z);
            return "Vertex";
        }
        if (command == "vn"){
            float x,y,z;
            stream >> x;
            stream >> y;
            stream >> z;
            vertexNormals.emplace_back(x,y,z);
            return "VertexNormals";
        }
//    f v1[/vt1][/vn1] v2[/vt2][/vn2] v3[/vt3][/vn3]... each vertext can come with vt and vn commands
//    f v1//vn1 ... ********** values can be negative ********
        if (command == "f"){
            string faceVex;
            std::vector<int> verteces;
            std::vector<int> vertecesTex;
            std::vector<int> vertecesNorm;

            string temp;
            while (std::getline(stream, temp, '/')){
                verteces.emplace_back(std::stoi(temp));
                std::getline(stream, temp, '/');
                vertecesTex.emplace_back(std::stoi(temp));
                std::getline(stream, temp);
                vertecesNorm.emplace_back(std::stoi(temp));
            }

            Face f;
            for (int index: verteces){
                f.push_back(vertices[index]);
            }
            faces.push_back(f);


            return "Faces";
        }
        return "Ignored";
    }

    void printModel() {
        std::cout << "Model: " << name << std::endl;

        std::cout << "Vertices:" << std::endl;
        for (const auto& vertex : vertices) {
            // Assuming Vertex class has a method to print its properties
            std::cout << vertex;
            // Example: vertex.print();
        }
        std::cout << std::endl;

        std::cout << "Vertex Normals:" << std::endl;
        for (const auto& normal : vertexNormals) {
            // Assuming Vertex class has a method to print its properties
            std::cout << normal;
            // Example: normal.print();
        }
        std::cout << std::endl;


        std::cout << "Faces:" << std::endl;
        for (const auto& face : faces) {
            // Assuming Face class has a method to print its properties
            for (const auto& v: face){
                std::cout << v << " ";
            }
            std::cout << std::endl;
            // Example: face.print();
        }
    }
};

class Camera {
private:
    Vertex origin;
    std::vector<std::vector<Vertex>> canvas;

public:
};












