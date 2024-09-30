#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Model.h"
#include "Canvas.h"

// Define constants for debugging and camera origin
#define DEBUG_CANVAS {22, 150}  // Dimensions for the debug canvas
#define CAMERA_ORIGIN 4, 4, 4    // Default camera origin coordinates

/**
 * @class Camera
 * @brief Represents a camera in a 3D rendering environment.
 *
 * The Camera class is responsible for defining the camera's position,
 * orientation, and light source. It handles ray tracing to render
 * the 3D model onto a canvas.
 */
class Camera {
 private:
  const Model& _model;              ///< Reference to the model being rendered
  Eigen::Vector3d _origin;          ///< The camera's position in 3D space
  Eigen::Vector3d _cPoint0;         ///< First control point for camera manipulation
  Eigen::Vector3d _cVec1;           ///< First direction vector for camera orientation
  Eigen::Vector3d _cVec2;           ///< Second direction vector for camera orientation
  Eigen::Vector3d _lightSource;     ///< Position of the light source
  std::vector<std::tuple<double, int, int>> _strokes; ///< Stores stroke data for rendering
  Canvas _canvas;                   ///< The canvas where the model will be drawn

 public:
  /**
   * @brief Constructs a Camera object with a reference model.
   * @param model The model to be rendered by the camera.
   */
  Camera(const Model& model);

  /**
   * @brief Constructs a Camera object with a specified origin.
   * @param model The model to be rendered by the camera.
   * @param origin The initial position of the camera.
   */
  Camera(const Model& model, Eigen::Vector3d origin);

  /**
   * @brief Retrieves the resolution of the canvas.
   * @return The current resolution of the canvas.
   */
  Canvas getResolution();

  /**
   * @brief Performs ray tracing to render the model.
   *
   * This function calculates the rays from the camera's position
   * through each pixel on the canvas to determine the color and
   * brightness of each pixel based on the model's geometry and light source.
   */
  void rayTrace();

  /**
   * @brief Prints the current state of the camera.
   *
   * This function outputs relevant information about the camera's
   * position and configuration to the console for debugging purposes.
   */
  void print();

  /**
   * @brief Retrieves the stroke character based on brightness.
   * @param brightness The brightness level to determine the stroke.
   * @return The character representing the stroke.
   */
  char getStroke(double brightness);

  /**
   * @brief Draws the model onto the canvas.
   *
   * This function renders the model onto the canvas by converting
   * the ray-traced information into visual strokes.
   */
  void draw();

  // TODO: add a change camera position option
};

#endif //_CAMERA_H_
