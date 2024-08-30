
#include "Classes.h"

// Vect3 class implementation
Vect3::Vect3() : _x(0), _y(0), _z(0) {}

Vect3::Vect3(float x, float y, float z) : _x(x), _y(y), _z(z) {}

Vect3::Vect3(const Vect3& other) : _x(other._x), _y(other._y), _z(other._z) {}

float Vect3::getX() const {
    return _x;
}

float Vect3::getY() const {
    return _y;
}

float Vect3::getZ() const {
    return _z;
}

bool Vect3::operator==(const Vect3& other) const {
    return _x == other._x && _y == other._y && _z == other._z;
}

Vect3 operator-(const Vect3 &lhs, const Vect3 &rhs) {
    return {lhs._x-rhs._x, lhs._y-rhs._y, lhs._z-rhs._z};
}

Vect3 operator-(const Vect3& rhs) {
    return Vect3::ZERO - rhs;
}

Vect3 operator+(const Vect3& lhs, const Vect3& rhs){
    return {lhs._x+rhs._x, lhs._y+rhs._y, lhs._z+rhs._z};
}

Vect3 operator/(const Vect3 &lhs, float rhs) {
    return {lhs._x/rhs, lhs._y/rhs, lhs._z/rhs};
}


Vect3& Vect3::operator+=(const Vect3& other){
    _x += other._x;
    _y += other._y;
    _z += other._z;
    return (*this);
}

std::string operator+(const std::string& lhs, const Vect3 rhs) {
    return lhs + std::to_string(rhs._x) + " " + std::to_string(rhs._y) + " " + std::to_string(rhs._z) + " \n";
}

std::ostream& operator<<(std::ostream& os, const Vect3& vertex) {
    os << "(" << vertex._x << ", " << vertex._y << ", " << vertex._z << ")";
    return os;
}
const Vect3 Vect3::ZERO = Vect3();

const Vect3 Vect3::ONE = Vect3(1,1,1);


namespace std {
    size_t hash<Vect3>::operator()(const Vect3& vertex) const {
        size_t hx = std::hash<float>()(vertex.getX());
        size_t hy = std::hash<float>()(vertex.getY());
        size_t hz = std::hash<float>()(vertex.getZ());
        return hx ^ (hy << 1) ^ (hz << 2); // Combine hash values
    }
}

// Face class implementation
Face::Face() : _normal(Vect3()) {}

Face::Face(Vect3& normal) : _normal(normal) {}

void Face::push_back(Vect3& vertex) {
    _verteces.push_back(vertex);
}

std::vector<Vect3> Face::getVerts() {
    return _verteces;
}

Vect3 Face::getNorm() {
    return _normal;
}

// Model class implementation
Model::Model(std::ifstream &objectFile, bool center) {
    readFile(objectFile);
    if (center)
        centering();
}


void Model::centering(){
    Vect3 antiVect = (Vect3::ZERO - _centerVector)/(float)_vertices.size();
    for (auto& v: _vertices){
        v += antiVect;
    }
}

void Model::readFile(std::ifstream &objectFile){
    std::string line;
    while (std::getline(objectFile, line)) {
        std::istringstream command(line);
        std::string token;
        std::string commandRead;
        while (command >> token) { //TODO can this be done in a formatted way?
            commandRead = Model::factory(token, command); // commandRead is for debugging
        }
    }
}

std::string Model::factory(const std::string& command, std::istringstream &stream) {
    if (command == "o") {
        stream >> _name;
        return "Name";
    }
    if (command == "v") {
        float x, y, z;
        stream >> x >> y >> z;
        Vect3 v(x, y, z);
        _centerVector += v;
        _vertices.push_back(v);
        _vertexIndexMap[v] = _vertices.size();
        return "Vect3";
    }
    if (command == "vn") {
        float x, y, z;
        stream >> x >> y >> z;
        Vect3 v(x, y, z);
        _vertexNormals.push_back(v);
        _normalIndexMap[v] = _vertexNormals.size();
        return "VertexNormals";
    }
//    f v1[/vt1][/vn1] v2[/vt2][/vn2] v3[/vt3][/vn3]... each vertex can come with vt and vn commands
//    f v1//vn1 ... ********** values can be negative ********
    if (command == "f") {
        std::string faceVex;
        std::vector<int> vert;
        std::vector<int> vertTex;
        std::vector<int> vertNorm;

        std::string temp;
        while (std::getline(stream, temp, '/')) {
            vert.emplace_back(std::stoi(temp));
            std::getline(stream, temp, '/');
            vertTex.emplace_back(std::stoi(temp));
            std::getline(stream, temp, ' ');
            vertNorm.emplace_back(std::stoi(temp));
        }

        Face f(_vertexNormals[vertNorm[0] - 1]);
        for (int index : vert) {
            f.push_back(_vertices[index - 1]);
        }
        _faces.push_back(f);
        return "Faces";
    }
    return "Ignored";
}

std::string Model::toOBJ(const std::string& filePath) {
    bool export_ = !(filePath.empty());
    std::string file = "o " + _name + "\n";
    for (const auto& vertex : _vertices) {
        file += "v " + vertex;
    }
    for (const auto& normal : _vertexNormals) {
        file += "vn " + normal;
    }
    for (Face& face : _faces) {
        file += "f ";

        for (const auto& v : face.getVerts())
            file += std::to_string(_vertexIndexMap[v]) + "//" + std::to_string(_normalIndexMap[face.getNorm()]) + " ";

        file += "\n";
    }
    if (export_) {
        std::ofstream MyFile(filePath);
        MyFile << file;
        MyFile.close();
    }
    return file;
}

// Camera class implementation


// get model
// find relative position
// Generate Canvas
//ray trace?


Canvas::Canvas (int rows, int cols): _rows(rows), _cols(cols){
  _strokes.assign(_rows*_cols, ' ');
}

//automatic camera position
Camera::Camera (const Model &model) : _canvas (getResolution())
{
  //TODO calc variance and find most aesthetic angle
  //For now all models are small so were gonna have a set distance
  _origin = {4,4,4};
  //TODO camera needs to point towards center?

}

//Custom camera position
Camera::Camera(Vect3& origin) : _origin(origin), _canvas (getResolution())
{
}


Canvas Camera::getResolution() {

//  TODO likely send this to a utils folder to run per operating system
  FILE* pipe = popen("echo $LINES && echo $COLUMNS", "r");
  if (!pipe) {
    std::cerr << "Failed to open pipe." << std::endl;
    return {-1,-1};
  }
  char buffer[128];
  fgets(buffer, sizeof(buffer), pipe);
  int rows = atoi(buffer);
  fgets(buffer, sizeof(buffer), pipe);
  int cols = atoi(buffer);
  pclose(pipe);
  return {rows, cols};
}
