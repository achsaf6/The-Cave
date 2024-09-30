#include "Canvas.h"

/**
 * @brief Constructs a Canvas object with specified dimensions.
 * @param rows The number of rows in the canvas.
 * @param cols The number of columns in the canvas.
 */
Canvas::Canvas(int rows, int cols) : _rows(rows), _cols(cols) {
  _aspectRatio = _cols / (_rows * CHAR_DIM);  // Calculate aspect ratio based on dimensions
  _strokes.assign(rows * cols, ' ');          // Initialize the canvas with empty spaces
}

/**
 * @brief Returns the number of rows in the canvas.
 * @return The number of rows.
 */
float Canvas::rows() const {
  return _rows;
}

/**
 * @brief Returns the number of columns in the canvas.
 * @return The number of columns.
 */
float Canvas::cols() const {
  return _cols;
}

/**
 * @brief Converts a canvas x-coordinate to Normalized Device Coordinates (NDC).
 * @param x The x-coordinate on the canvas.
 * @return The NDC x-coordinate.
 */
float Canvas::getNDCx(int x) {
  return (2 * (static_cast<float>(x) / _cols) - 1) / _aspectRatio;  // Scale to NDC and account for aspect ratio
}

/**
 * @brief Converts a canvas y-coordinate to Normalized Device Coordinates (NDC).
 * @param y The y-coordinate on the canvas.
 * @return The NDC y-coordinate.
 */
float Canvas::getNDCy(int y) {
  return -((2 * (static_cast<float>(y) / _rows) - 1)) / CHAR_DIM;  // Scale to NDC, flipping the y-axis
}

/**
 * @brief Draws a character at the specified (x, y) position on the canvas.
 * @param c The character to draw.
 * @param x The x-coordinate (row) where the character will be drawn.
 * @param y The y-coordinate (column) where the character will be drawn.
 */
void Canvas::draw(char c, int x, int y) {
  _strokes[x * _cols + y] = c;  // Place character in the appropriate position
}

/**
 * @brief Draws a character at the specified index on the canvas.
 * @param c The character to draw.
 * @param i The index in the stroke array where the character will be drawn.
 */
void Canvas::draw(char c, int i) {
  _strokes[i] = c;  // Place character in the specified index
}

/**
 * @brief Clears the canvas by resetting all strokes to empty spaces.
 */
void Canvas::clear() {
  _strokes.assign(_rows * _cols, ' ');  // Reset canvas to empty state
}

/**
 * @brief Overloads the subscript operator to access canvas strokes.
 * @param index The index of the stroke to access.
 * @return A reference to the stroke character at the specified index.
 */
char& Canvas::operator[](size_t index) {
  return _strokes[index];  // Return reference for modification
}

/**
 * @brief Overloads the subscript operator to access canvas strokes (const version).
 * @param index The index of the stroke to access.
 * @return A constant reference to the stroke character at the specified index.
 */
const char& Canvas::operator[](size_t index) const {
  return _strokes[index];  // Return const reference for read-only access
}

/**
 * @brief Overloads the output stream operator to print the canvas.
 * @param os The output stream.
 * @param c The canvas to output.
 * @return The output stream with the canvas contents.
 */
std::ostream& operator<<(std::ostream& os, Canvas& c) {
  std::string output = c._strokes;  // Create a copy of strokes for output
  for (int i = c._rows; i > 0; i--) {
    output.insert(i * c._cols, "\n");  // Insert newline characters for row formatting
  }
  c.clear();  // Clear the canvas after outputting
  os << output;  // Write the formatted output to the stream
  return os;  // Return the output stream
}
