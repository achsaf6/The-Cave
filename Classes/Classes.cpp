
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
    _v.push_back(vertex);
}

void Face::triangulate ()
{
  // converts a face into a list of triangles
  for (int i = 0 ; i < _v.size() - 2 ; i++ ){
    _triangles.push_back({_v[i],_v[i+1],_v[i+2]});
  }
}

std::vector<Eigen::Vector3d> Face::getVerts() {
    return _v;
}

Eigen::Vector3d Face::getNorm() {
    return _normal;
}
bool Face::intersect (const Eigen::Vector3d &orig, const Eigen::Vector3d &dir)
{
  if (_normal.dot(orig - dir) == 0){
    return false;
  }
  double D = -_normal.dot(_v[0]);
  double t = - (_normal.dot(orig) + D) / _normal.dot(dir);
  Eigen::Vector3d P = orig + t*dir;


  for (triangle tri: _triangles){
    Eigen::Vector3d edge0 = tri[1] - tri[0];
    Eigen::Vector3d edge1 = tri[2] - tri[1];
    Eigen::Vector3d edge2 = tri[0] - tri[2];
    Eigen::Vector3d C0 = P - tri[0];
    Eigen::Vector3d C1 = P - tri[1];
    Eigen::Vector3d C2 = P - tri[2];

    if (_normal.dot(edge0.cross(C0)) > 0 && _normal.dot(edge1.cross(C1)) > 0 &&
                                              _normal.dot(edge2.cross(C2)) > 0)
      return true;
  }
  return false;
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
          if (token == "#")
            break;
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
        f.triangulate();
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
bool Model::intersect (const Eigen::Vector3d &orig, const Eigen::Vector3d &dir,
                  Eigen::Vector3d& normal) const
{
  bool isIntersect = false;
  for (Face face : _faces){
    if (face.intersect(orig, dir)){
      normal = face.getNorm(); //TODO find the closest intersection not the not the last one
      isIntersect = true;
    }
  }
  return isIntersect;
}



//Canvas Class implementation

Canvas::Canvas (int rows, int cols): _rows(rows), _cols(cols){
  _aspectRatio = _cols/_rows;
  _strokes.assign(rows*cols, ' ');
}

float Canvas::rows () const
{
  return _rows;
}

float Canvas::cols () const
{
  return _cols;
}

float Canvas::getNDCx(int x)
{

  return 2*(x/_rows) - 1;
}

float Canvas::getNDCy(int y)
{
  return ((2*((y)/_cols) - 1)*_aspectRatio) /2.5; //TODO divide by char dimensions
}
void Canvas::draw (char c, int x, int y)
{
  _strokes[x*_cols + y] = c;
}
char& Canvas::operator[] (size_t index)
{
  return _strokes[index];
}
const char& Canvas::operator[] (size_t index) const
{
  return _strokes[index];
}

std::ostream& operator<<(std::ostream& os, const Canvas& c){
  std::string output = c._strokes;
  for (int i = c._rows ; i > 0 ; i--){
    output.insert(i*c._cols, "\n");
  }
  os << output;
  return os;
}





// Camera class implementation



//automatic camera position
Camera::Camera (const Model &model) : _model(model), _canvas (getResolution())
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



Canvas Camera::getResolution() {
//  TODO figure out why this behaves differently from 'run' to 'debug'
//   alternatively figure out if it matters (if I can compile )
//  TODO likely send this to a utils folder to run per operating system



//currently designed for linux


#ifdef DEBUG
std::cout << "DEBUG MODE" << std::endl;
return {23, 150};

#else
  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
    std::cerr << "Failed to get terminal size." << std::endl;
    return {-1,-1};
  }
  std::cout << "Rows: " << w.ws_row << std::endl;
  std::cout << "Cols: " << w.ws_col << std::endl;
  return {w.ws_row, w.ws_col};
#endif
}

void Camera::rayTrace()
{
  for (int i=0 ; i < _canvas.rows() ; i++){
    for (int j=0 ; j < _canvas.cols() ; j++){
      float s1 = _canvas.getNDCx (i);
      float s2 = _canvas.getNDCy (j);

      //This works for a sphere, not much else...
      Eigen::Vector3d dir = _cPoint0 + s1*_cVec1 + s2*_cVec2;
      Eigen::Vector3d normal;
      if (_model.intersect (_origin, dir, normal)){
        _canvas.draw('*', i, j);
      }
    }
  }
}

void Camera::print ()
{
  std::cout << _canvas << std::endl;
}



