#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string>

#define CHAR_DIM 4  // Constant defining character dimensions for NDC calculations

/**
 * @brief The Canvas class represents a drawable canvas for rendering characters.
 */
class Canvas {
 private:
  float _aspectRatio;  // Aspect ratio of the canvas
  float _rows;         // Number of rows in the canvas
  float _cols;         // Number of columns in the canvas
  std::string _strokes; // Stores the characters (strokes) drawn on the canvas

 public:
  /**
   * @brief Constructs a Canvas object with specified dimensions.
   * @param rows The number of rows in the canvas.
   * @param cols The number of columns in the canvas.
   */
  Canvas(int rows, int cols);

  /**
   * @brief Returns the number of rows in the canvas.
   * @return The number of rows.
   */
  float rows() const;

  /**
   * @brief Returns the number of columns in the canvas.
   * @return The number of columns.
   */
  float cols() const;

  /**
   * @brief Converts a canvas x-coordinate to Normalized Device Coordinates (NDC).
   * @param x The x-coordinate on the canvas.
   * @return The NDC x-coordinate.
   */
  float getNDCx(int x);

  /**
   * @brief Converts a canvas y-coordinate to Normalized Device Coordinates (NDC).
   * @param y The y-coordinate on the canvas.
   * @return The NDC y-coordinate.
   */
  float getNDCy(int y);

  /**
   * @brief Draws a character at the specified (x, y) position on the canvas.
   * @param c The character to draw.
   * @param x The x-coordinate (row) where the character will be drawn.
   * @param y The y-coordinate (column) where the character will be drawn.
   */
  void draw(char c, int x, int y);

  /**
   * @brief Draws a character at the specified index on the canvas.
   * @param c The character to draw.
   * @param i The index in the stroke array where the character will be drawn.
   */
  void draw(char c, int i);

  /**
   * @brief Clears the canvas by resetting all strokes to empty spaces.
   */
  void clear();

  /**
   * @brief Overloads the subscript operator to access canvas strokes.
   * @param index The index of the stroke to access.
   * @return A reference to the stroke character at the specified index.
   */
  char& operator[](size_t index);

  /**
   * @brief Overloads the subscript operator to access canvas strokes (const version).
   * @param index The index of the stroke to access.
   * @return A constant reference to the stroke character at the specified index.
   */
  const char& operator[](size_t index) const;

  /**
   * @brief Overloads the output stream operator to print the canvas.
   * @param os The output stream.
   * @param c The canvas to output.
   * @return The output stream with the canvas contents.
   */
  friend std::ostream& operator<<(std::ostream& os, Canvas& c);
};

#endif //_CANVAS_H_
