// COMP-3520 Assignment 1
// Adam Vandolder
// Student #104629080
// Implement Bresenham's line algorithm, and use it to draw lines using OpenGL.
// Should be compiled with -std=c++11 using g++.
// Due to use of unistd.h and usleep, it must be compiled/ran on Linux, or
// as I did, Windows Subsystem for Linux with an X11 server.
// Creates a window with various lines drawn with differing slopes,
// as well as a line from the center of the window that follows the mouse.

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <unistd.h>
#include <utility>

#include <GL/freeglut.h>
#include <GL/gl.h>

enum class Key: unsigned char {
  Escape = 27,
};

const int WIDTH = 500, HEIGHT = 500;
int mx = WIDTH / 2, my = HEIGHT / 2;

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

void handle_mouse_movement(int x, int y) {
  mx = x;
  my = HEIGHT - y;
}

void handle_key(unsigned char key, int x, int y) {
  switch (static_cast<Key>(key)) {
    case Key::Escape:
      std::exit(0);
  }
}

void show_screen() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw lines with various slopes.
  glColor3f(0, 0, 1);
  bresenham_line(0, 0, 250, 125);
  bresenham_line(0, 125, 250, 0);
  glColor3f(1, 0, 0);
  bresenham_line(0, 250, 125, 500);
  bresenham_line(0, 500, 125, 250);
  glColor3f(0, 1, 0);
  bresenham_line(500, 250, 0, 250);
  bresenham_line(250, 0, 250, 500);
  glColor3f(1, 1, 0);
  bresenham_line(250, 250, 500, 500);
  bresenham_line(500, 250, 250, 500);

  // Draw a line from the center of the window to the mouse.
  glColor3f(1, 1, 1);
  bresenham_line(WIDTH / 2, HEIGHT / 2, mx, my);

  glutSwapBuffers();

  usleep(10000);
  glutPostRedisplay();
}

int main (int argc, char **argv) {
  // Initialize the GLUT window.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Draw Line");

  // Initialize OpenGL.
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glPointSize(1.0);
  glMatrixMode(GL_PROJECTION);
  glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

  // Add the appropriate callbacks.
  glutDisplayFunc(show_screen);
  glutMotionFunc(handle_mouse_movement);
  glutPassiveMotionFunc(handle_mouse_movement);
  glutKeyboardFunc(handle_key);

  glutMainLoop();
  return 0;
}
