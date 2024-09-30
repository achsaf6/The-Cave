
#include "Classes.h"

#include <utility>

#define DEBUG_CANVAS {22, 150}
#define CHAR_DIM 4
#define CAMERA_ORIGIN 4,4,4
#define EPSILON 0.0000000000000000000001

namespace std {
    size_t hash<Eigen::Vector3d>::operator()(const Eigen::Vector3d& vertex) const {
        size_t hx = std::hash<float>()(vertex.x());
        size_t hy = std::hash<float>()(vertex.y());
        size_t hz = std::hash<float>()(vertex.z());
        return hx ^ (hy << 1) ^ (hz << 2); // Combine hash values
    }
}

// Face class implementation
Face::Face() : _normalPtr(std::make_shared<Eigen::Vector3d>()) {}

Face::Face(v3DPtr normalPtr) : _normalPtr(std::move(normalPtr)) {}

void Face::push_back(const v3DPtr& vertexPtr) {
    _v.push_back(vertexPtr);
}

//TODO make this work for non-convex shapes
void Face::triangulate ()
{
  // converts a face into a list of triangles
  std::list<v3DPtr> tempV(_v.begin(), _v.end());
  bool popFlag = true;
  v3DPtr v0;
  v3DPtr v1;
  v3DPtr v2;
  while (tempV.size() != 3){
    if (popFlag){
      v0 = tempV.front();
      tempV.pop_front();
    } else {
      v0 = tempV.back();
      tempV.pop_back();
    }
    v1 = tempV.front();
    v2 = tempV.back();
    _triangles.push_back({v0, v1, v2});
    popFlag = !popFlag;
  }
  v0 = tempV.front();
  tempV.pop_front();
  v1 = tempV.front();
  tempV.pop_front();
  v2 = tempV.front();
  _triangles.push_back({v0, v1, v2});
}

std::vector<v3DPtr> Face::getVerts() {
    return _v;
}

v3DPtr Face::getNorm() {
    return _normalPtr;
}

bool Face::triRayIntersectGEO(const Eigen::Vector3d &orig, const
Eigen::Vector3d &dir, const triangle& tri){
  double D = -_normalPtr->dot(*_v[0]);
  double t = - (_normalPtr->dot(orig) + D) / _normalPtr->dot(dir);
  const Eigen::Vector3d P = orig + t*dir;

  Eigen::Vector3d edge0 = *(tri[1]) - *(tri[0]);
  Eigen::Vector3d edge1 = *(tri[2]) - *(tri[1]);
  Eigen::Vector3d edge2 = *(tri[0]) - *(tri[2]);
  Eigen::Vector3d C0 = P - *(tri[0]);
  Eigen::Vector3d C1 = P - *(tri[1]);
  Eigen::Vector3d C2 = P - *(tri[2]);

  return _normalPtr->dot(edge0.cross(C0)) >= 0 && _normalPtr->dot(edge1.cross
  (C1)) >=
                                                 0 && _normalPtr->dot(edge2
                                                 .cross(C2)) >= 0;
}


//TODO figure out why sometimes back facing shapes are shown
bool Face::intersectGEO (const Eigen::Vector3d &orig, const Eigen::Vector3d
&dir)
{
  if (_normalPtr->dot(orig - dir) == 0){
    return false;
  }

  for (const triangle& tri: _triangles)
  {
    if (triRayIntersectGEO (orig, dir, tri))
      return true;
  }
  return false;
}

bool Face::triRayIntersectMT(const Eigen::Vector3d &orig, const
Eigen::Vector3d &dir, const triangle& tri){
  Eigen::Vector3d AB = *(tri[1]) - *(tri[0]);
  Eigen::Vector3d AC = *(tri[2]) - *(tri[0]);
  Eigen::Vector3d pvec = dir.cross(AC);
  double det = AB.dot(pvec);

  if (abs(det) < EPSILON) //if the ray and triangle are parallel
    return false;

  double invDet = 1 / det;

  Eigen::Vector3d tvec = orig - *(tri[0]);
  double u = tvec.dot(pvec) * invDet;
  if (u < 0 || u > 1)
    return false;

  Eigen::Vector3d qvec = tvec.cross(AB);
  double v = dir.dot(qvec) * invDet;
  if (v < 0 || u + v > 1)
    return false;
  double t = AC.dot(qvec) * invDet;
  _pIntersect = orig + t*dir;
  return true;
}

bool Face::intersectMT(const Eigen::Vector3d &orig, const Eigen::Vector3d &dir){
  for (const triangle& tri: _triangles)
  {

      bool flag = false;
      Eigen::Vector3d v(-1,-1,-1);
      if (v.dot(dir) > 1.5)
        flag = true;

    if (triRayIntersectMT (orig, dir, tri))
      return true;
  }
  return false;
}
Eigen::Vector3d &Face::getIntersect ()
{
  return _pIntersect;
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
    for (v3DPtr v: _vertices){
        *v += antiVect;
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
        std::shared_ptr<Eigen::Vector3d> v =
            std::make_shared<Eigen::Vector3d>(x, y, z);
        _centerVector += *v;
        _vertices.push_back(v);
        _vertexIndexMap[v] = _vertices.size();
        return "Eigen::Vector3d";
    }
    if (command == "vn") {
        float x, y, z;
        stream >> x >> y >> z;
      std::shared_ptr<Eigen::Vector3d> v =
          std::make_shared<Eigen::Vector3d>(x, y, z);
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
    for (const auto vertex : _vertices) {
        file += "v " + *vertex + "\n";
    }
    for (const auto& normal : _vertexNormals) {
        file += "vn " + *normal + "\n";
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
bool Model::intersect (const Eigen::Vector3d &orig, const Eigen::Vector3d
&dir, Eigen::Vector3d &normal, Eigen::Vector3d &P) const
{
  double minDist = INFINITY;
  bool flag = false;
  for (Face face : _faces){
    if (face.intersectMT (orig, dir) && (face.getIntersect() - orig).norm() < minDist){
      P = face.getIntersect();
      minDist = (P - orig).norm();
      flag = true;
      normal = *face.getNorm();
    }
  }
  return minDist != INFINITY;
}

void Model::rotate (double theta)
{
  float cosTheta = cos(theta);
  float sinTheta = sin(theta);
  Eigen::Matrix3d rot;

  //rotate around the Z axis;
  rot << cosTheta, -sinTheta, 0,
         sinTheta, cosTheta,  0,
         0,        0,         1;

  for (v3DPtr v : _vertices){
    *v = rot * (*v);
  }

  for (v3DPtr n : _vertexNormals) {
    *n = rot * (*n);
  }

}



//Canvas Class implementation

Canvas::Canvas (int rows, int cols): _rows(rows), _cols(cols){
  _aspectRatio = _cols/(_rows*CHAR_DIM);
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
  return (2*(x/_cols) - 1) / _aspectRatio;
}

float Canvas::getNDCy(int y)
{
  return -((2*((y)/_rows) - 1)) / CHAR_DIM; //TODO divide by char dimensions
}

void Canvas::draw (char c, int x, int y)
{
  _strokes[x*_cols + y] = c;
}

void Canvas::draw (char c, int i)
{
  _strokes[i] = c;
}

void Canvas::clear ()
{
  _strokes.assign(_rows*_cols, ' ');
}

char& Canvas::operator[] (size_t index)
{
  return _strokes[index];
}

const char& Canvas::operator[] (size_t index) const
{
  return _strokes[index];
}

std::ostream& operator<<(std::ostream& os, Canvas& c){
  std::string output = c._strokes;
  for (int i = c._rows ; i > 0 ; i--){
    output.insert(i*c._cols, "\n");
  }
  c.clear();
  os << output;
  return os;
}






// Camera class implementation

Camera::Camera (const Model &model, Eigen::Vector3d origin) : _model(model),
_canvas (getResolution())
{
  //For now all models are small so were gonna have a set distance
  _origin = Eigen::Vector3d(std::move(origin));
  _lightSource = Eigen::Vector3d(_origin[1], -_origin[0], _origin[2]);
  Eigen::Vector3d normal = _origin.normalized();


//  we use these three points to "move" across the canvas
//  i.e. P0 + 4 * v1 - 2 * v2 = from p1 move 4 units with v1 and 2 units
//  with v2

  _cPoint0 = _origin - normal;
  Eigen::Vector3d up(0, 0, 1);
  _cVec1 = (up - up.dot(normal) * normal).normalized();
  _cVec2 = (normal.cross(_cVec1)).normalized();
}


//TODO calc variance and find most aesthetic angle
//Default camera position
Camera::Camera (const Model &model) : Camera (model, Eigen::Vector3d(CAMERA_ORIGIN))
{}




Canvas Camera::getResolution() {
//  TODO figure out why this behaves differently from 'run' to 'debug'
//   alternatively figure out if it matters (if I can compile )
//  TODO likely send this to a utils folder to run per operating system



//currently designed for linux


#ifdef DEBUG
std::cout << "DEBUG MODE" << std::endl;
return DEBUG_CANVAS;

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
      float s1 = _canvas.getNDCy(i);
      float s2 = _canvas.getNDCx(j);

      Eigen::Vector3d dir = (_cPoint0 + s1*_cVec1 + s2*_cVec2) - _origin;

      Eigen::Vector3d normal;
      Eigen::Vector3d P;
      if (_model.intersect(_origin, dir, normal, P)){
        Eigen::Vector3d ince = -dir.normalized();
        Eigen::Vector3d refr = (_lightSource - P).normalized();
        Eigen::Vector3d inter = ince/2 + refr/2;
        _strokes.emplace_back(normal.dot (inter), i, j) ;
      } else {
        _strokes.emplace_back(-INFINITY, i, j);
      }
    }
  }
  draw();
}

void Camera::print ()
{
  std::cout << _canvas << std::endl;
}

void Camera::draw ()
{
  auto minMax = std::minmax_element (_strokes.begin (), _strokes.end (), []
  (const auto& a, const auto& b) {return std::get<0>(a) < std::get<0>(b);});

  double brightest = -INFINITY;
  double darkest = INFINITY;


  for (std::tuple<double, int, int> stroke : _strokes){
    double shine = std::get<0>(stroke);
    if ( shine == INFINITY || shine == -INFINITY)
      continue;
    if (shine < darkest)
      darkest = shine;
    if (shine > brightest)
      brightest = shine;
  }

  double range = brightest - darkest;
  std::string brush = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()"
             "1{}[]?-_+~<>i!lI;:,^`'.";
  for (int i = 0 ; i < _strokes.size() ; i++){
    double shine = std::get<0>(_strokes[i]);
    if (shine == -INFINITY){
      _canvas.draw(' ',std::get<1>(_strokes[i]), std::get<2>(_strokes[i]));
    } else {
      double inter = (brightest - shine) / range;
      int brushIndex = inter * brush.size ();
      _canvas.draw (brush[brushIndex], std::get<1> (_strokes[i]), std::get<2>
          (_strokes[i]));
    }
  }
}


