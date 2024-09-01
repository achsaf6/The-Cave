
#include "Classes.h"



namespace std {
    size_t hash<Eigen::Vector3d>::operator()(const Eigen::Vector3d& vertex) const {
        size_t hx = std::hash<float>()(vertex.x());
        size_t hy = std::hash<float>()(vertex.y());
        size_t hz = std::hash<float>()(vertex.z());
        return hx ^ (hy << 1) ^ (hz << 2); // Combine hash values
    }
}

// Face class implementation
Face::Face() : _normal(Eigen::Vector3d()) {}

Face::Face(Eigen::Vector3d& normal) : _normal(normal) {}

void Face::push_back(Eigen::Vector3d& vertex) {
    _verteces.push_back(vertex);
}

std::vector<Eigen::Vector3d> Face::getVerts() {
    return _verteces;
}

Eigen::Vector3d Face::getNorm() {
    return _normal;
}

// Model class implementation
Model::Model(std::ifstream &objectFile, bool center) {
    readFile(objectFile);
    if (center)
        centering();
}


void Model::centering(){
    Eigen::Vector3d antiVect = (Eigen::Vector3d(0,0,0) - _centerVector)/(float)
        _vertices
        .size();
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
        Eigen::Vector3d v(x, y, z);
        _centerVector += v;
        _vertices.push_back(v);
        _vertexIndexMap[v] = _vertices.size();
        return "Eigen::Vector3d";
    }
    if (command == "vn") {
        float x, y, z;
        stream >> x >> y >> z;
        Eigen::Vector3d v(x, y, z);
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

std::string operator+(std::string lhs, const Eigen::Vector3d& rhs) {
  return lhs + std::to_string (rhs.x()) + " " + std::to_string (rhs.z()) + " " +
  std::to_string (rhs.z());
}

std::string Model::toOBJ(const std::string& filePath) {
    bool export_ = !(filePath.empty());
    std::string file = "o " + _name + "\n";
    for (const auto& vertex : _vertices) {
        file += "v " + vertex + "\n";
    }
    for (const auto& normal : _vertexNormals) {
        file += "vn " + normal + "\n";
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
  _origin = Eigen::Vector3d(4,4,4);

  Eigen::Vector3d normal = _origin;
  normal.normalize();


//  we use these three points to "move" across the canvas
//  i.e. P0 + 4 * v1 - 2 * v2 = from p1 move 4 units with v1 and 2 units
//  with v2

  _cPoint0 = _origin - normal;
//  TODO find the vector that aligns with "up"
  _cVec1 = normal.unitOrthogonal();
  _cVec2 = normal.cross(_cVec1);
}

//Custom camera position
Camera::Camera(Eigen::Vector3d& origin) : _origin(origin), _canvas (getResolution())
{
  // camera always points to (0,0,0)

  Eigen::Vector3d normal = _origin;
  normal.normalize();

  _cPoint0 = _origin - normal;
  _cVec1 = normal.unitOrthogonal();
  _cVec2 = normal.cross(_cVec1);
}


Canvas Camera::getResolution() {

//  TODO likely send this to a utils folder to run per operating system

//currently designed for linux
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
