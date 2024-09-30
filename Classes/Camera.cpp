#include <iostream>
#include "Camera.h"

// Constructor that initializes the Camera with a model and a specified origin.
Camera::Camera(const Model &model, Eigen::Vector3d origin)
    : _model(model), _canvas(getResolution())
{
  // Set the camera's origin and light source based on the specified origin.
  _origin = Eigen::Vector3d(std::move(origin));
  _lightSource = Eigen::Vector3d(_origin[1], -_origin[0], _origin[2]);
  Eigen::Vector3d normal = _origin.normalized();

  // Define the camera's control points for navigating the canvas.
  _cPoint0 = _origin - normal;
  Eigen::Vector3d up(0, 0, 1);
  _cVec1 = (up - up.dot(normal) * normal).normalized();  // First direction vector
  _cVec2 = (normal.cross(_cVec1)).normalized();          // Second direction vector
}

// Default constructor that initializes the Camera with a predefined origin.
Camera::Camera(const Model &model)
    : Camera(model, Eigen::Vector3d(CAMERA_ORIGIN))
{}

// Gets the resolution of the canvas based on the terminal size or debug settings.
Canvas Camera::getResolution() {
  // Debug mode configuration
#ifdef DEBUG
  std::cout << "DEBUG MODE" << std::endl;
  return DEBUG_CANVAS;
#else
  // Get terminal size for non-debug mode
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        std::cerr << "Failed to get terminal size." << std::endl;
        return {-1, -1};  // Return invalid size if there's an error
    }
    std::cout << "Rows: " << w.ws_row << std::endl;
    std::cout << "Cols: " << w.ws_col << std::endl;
    return {w.ws_row, w.ws_col};  // Return terminal size
#endif
}

// Performs ray tracing to render the model onto the canvas.
void Camera::rayTrace() {
  for (int i = 0; i < _canvas.rows(); i++) {
    for (int j = 0; j < _canvas.cols(); j++) {
      float s1 = _canvas.getNDCy(i);  // Normalized Device Coordinate Y
      float s2 = _canvas.getNDCx(j);  // Normalized Device Coordinate X

      // Calculate the direction of the ray
      Eigen::Vector3d dir = (_cPoint0 + s1 * _cVec1 + s2 * _cVec2) - _origin;

      Eigen::Vector3d normal;
      Eigen::Vector3d P;
      // Check for intersection with the model
      if (_model.intersect(_origin, dir, normal, P)) {
        Eigen::Vector3d ince = -dir.normalized();  // Incoming direction
        Eigen::Vector3d refr = (_lightSource - P).normalized();  // Light direction
        Eigen::Vector3d inter = ince / 2 + refr / 2;  // Average vector for shading
        _strokes.emplace_back(normal.dot(inter), i, j);  // Store stroke data
      } else {
        _strokes.emplace_back(-INFINITY, i, j);  // No intersection
      }
    }
  }
  draw();  // Render the strokes onto the canvas
}

// Prints the current state of the canvas for debugging.
void Camera::print() {
  std::cout << _canvas << std::endl;
}

// Draws the strokes onto the canvas based on brightness levels.
void Camera::draw() {
  auto minMax = std::minmax_element(_strokes.begin(), _strokes.end(),
                                    [](const auto& a, const auto& b) { return std::get<0>(a) < std::get<0>(b); });

  double brightest = -INFINITY;
  double darkest = INFINITY;

  // Determine the brightest and darkest strokes
  for (const auto& stroke : _strokes) {
    double shine = std::get<0>(stroke);
    if (shine == INFINITY || shine == -INFINITY)
      continue;
    if (shine < darkest)
      darkest = shine;
    if (shine > brightest)
      brightest = shine;
  }

  double range = brightest - darkest;  // Range of brightness
  std::string brush = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()"
                      "1{}[]?-_+~<>i!lI;:,^`'.";  // Characters for rendering

  // Draw the strokes onto the canvas
  for (const auto& stroke : _strokes) {
    double shine = std::get<0>(stroke);
    if (shine == -INFINITY) {
      _canvas.draw(' ', std::get<1>(stroke), std::get<2>(stroke));  // Empty space for no intersection
    } else {
      double inter = (brightest - shine) / range;  // Normalize shine
      int brushIndex = inter * brush.size();  // Map to brush index
      _canvas.draw(brush[brushIndex], std::get<1>(stroke), std::get<2>(stroke));  // Draw character
    }
  }
}
