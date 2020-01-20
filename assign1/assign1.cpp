// COMP-3520 Assignment 1
// Adam Vandolder
// Student #104629080
// Implement Bresenham's line algorithm, and use it to draw lines using OpenGL.
// Creates a window with various lines drawn with differing slopes,
// as well as a line from the center of the window that follows the mouse.

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>

#include <GLFW/glfw3.h>

const int WIDTH = 500, HEIGHT = 500;

void bresenham_line(int x1, int y1, int x2, int y2) {
  // Draw a line from (x1, y1) to (x2, y2) using Bresenham's algorithm.
  glBegin(GL_POINTS);

  const int dx = x2 - x1;
  const int dy = y2 - y1;
  const int dx2 = std::abs(2 * dx);
  const int dy2 = std::abs(2 * dy);
  const double m = (double)dy / (double)dx;
  const int step = (m > 0) ? 1 : -1;

  if (dx == 0) {
    // Vertical line.
    for (int y = std::min(y1, y2); y <= std::max(y1, y2); ++y) {
      glVertex2f(x1, y);
    }
  } else if (dy == 0) {
    // Horizontal line.
    for (int x = std::min(x1, x2); x <= std::max(x1, x2); ++x) {
      glVertex2f(x, y1);
    }
  } else if (std::abs(m) < 1) {
    // Slope is between -1 and 1, so step by x.
    const int diff = dy2 - dx2;
    int p = dy2 - dx;

    if (x1 > x2) {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }

    for (int x = x1, y = y1; x <= x2; ++x) {
      glVertex2f(x, y);
      if (p < 0) {
        p += dy2;
      } else {
        y += step;
        p += diff;
      }
    }
  } else {
    // Slope is outside of -1 to 1, so step by y.
    const int diff = dx2 - dy2;
    int p = dx2 - dy;

    if (y1 > y2) {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }

    for (int x = x1, y = y1; y <= y2; ++y) {
      glVertex2f(x, y);
      if (p < 0) {
        p += dx2;
      } else {
        x += step;
        p += diff;
      }
    }
  }

  glEnd();
}

void handle_key(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    std::exit(EXIT_SUCCESS);
  }
}

void draw_lines(double mx, double my) {
  // Draw Xs with various slopes.
  glColor3f(0, 0, 1);
  bresenham_line(0, 0, WIDTH / 2, HEIGHT / 4);
  bresenham_line(0, HEIGHT / 4, WIDTH / 2, 0);
  glColor3f(1, 0, 0);
  bresenham_line(0, HEIGHT / 2, WIDTH / 4, HEIGHT);
  bresenham_line(0, HEIGHT, WIDTH / 4, HEIGHT / 2);
  glColor3f(0, 1, 0);
  bresenham_line(WIDTH, HEIGHT / 2, 0, HEIGHT / 2);
  bresenham_line(WIDTH / 2, 0, WIDTH /2, HEIGHT);
  glColor3f(1, 1, 0);
  bresenham_line(WIDTH / 2, HEIGHT / 2, WIDTH, HEIGHT);
  bresenham_line(WIDTH, HEIGHT / 2, WIDTH / 2, HEIGHT);

  // Draw a line from the center of the window to the mouse.
  glColor3f(1, 1, 1);
  bresenham_line(WIDTH / 2, HEIGHT / 2, mx, HEIGHT - my);
}

int main (int argc, char **argv) {
  if (!glfwInit()) {
    return EXIT_FAILURE;
  }

  auto window = glfwCreateWindow(WIDTH, HEIGHT, "Draw Lines", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, handle_key);

  double mx = WIDTH / 2, my = HEIGHT / 2;

  while (!glfwWindowShouldClose(window)) {
    // Set up OpenGL to render the current frame.
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPointSize(1);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, WIDTH, 0, HEIGHT, 0, 1);

    // Render various lines using Bresenham's algorithm.
    glfwGetCursorPos(window, &mx, &my);
    draw_lines(mx, my);

    glfwSwapBuffers(window);
    glfwWaitEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
