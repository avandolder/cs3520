#include <cmath>
#include <unistd.h>
#include <utility>

#include <GL/freeglut.h>
#include <GL/gl.h>

const int WIDTH = 500, HEIGHT = 500;
int mx = WIDTH / 2, my = HEIGHT / 2;

void dda_line(int x1, int y1, int x2, int y2) {
  glBegin(GL_POINTS);

  if (x1 == x2) {
    if (y1 > y2) {
      std::swap(y1, y2);
    }

    for (int y = y1; y <= y2; y++) {
      glVertex2f(x1, y);
    }
  } else {
    const double m = (y1 - y2) / (double)(x1 - x2);
    if (std::abs(m) <= 1) {
      if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
      }

      for (double x = x1, y = y1; x <= x2; x++, y += m) {
        glVertex2f(x, y);
      }
    } else {
      if (y1 > y2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
      }

      for (double x = x1, y = y1; y <= y2; x += 1/m, y++) {
        glVertex2f(x, y);
      }
    }
  }

  glEnd();
}

void bresenham_line(int x1, int y1, int x2, int y2) {
  glBegin(GL_POINTS);

  int x = x1, y = y1;
  const int dx = x2 - x1;
  const int dy = y2 - y1;

  if (dx == 0) {
    // Vertical line.
    for (; y <= y2; ++y) {
      glVertex2f(x, y);
    }
  } else if (dy == 0) {
    // Horizontal line.
    for (; x <= x2; ++x) {
      glVertex2f(x, y);
    }
  } else if (dx == dy) {
    // Slope is +/- 1.
    const int stepx = (dx > 0) ? 1 : -1;
    const int stepy = (dy > 0) ? 1 : -1;
    for (; x <= x2; x += stepx, y += stepy) {
      glVertex2f(x, y);
    }
  } else {
    const int dx2 = 2 * dx;
    const int dy2 = 2 * dy;
    const int step = (dx < 0) == (dy < 0) ? 1 : -1;

    if (dy < dx) {
      // If -1 < slope < 1.
      int diff = dy2 - dx2;
      int p = dy2 - dx;

      for (; x <= x2; ++x) {
        glVertex2f(x, y);
        if (p < 0) {
          p += dy2;
        } else {
          y += step;
          p += diff;
        }
      }
    } else {
      int diff = dx2 - dy2;
      int p = dx2 - dy;

      for (; y <= y2; ++y) {
        glVertex2f(x, y);
        if (p < 0) {
          p += dx2;
        } else {
          x += step;
          p += diff;
        }
      }
    }
  }

  glEnd();
}

void handle_mouse_movement(int x, int y) {
  mx = x;
  my = HEIGHT - y;
}

void show_screen() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw various lines using Bresenham's algorithm.
  glColor3f(0.0, 0.0, 1.0);
  bresenham_line(0, 0, 250, 125);
  glColor3f(1.0, 0, 0);
  bresenham_line(0, 250, 125, 500);
  glColor3f(0, 1, 0);
  bresenham_line(0, 250, 500, 250);
  bresenham_line(250, 0, 250, 500);
  glColor3f(1, 1, 0);
  bresenham_line(250, 250, 500, 500);

  // Draw a line from the center of the window to the mouse using the DDA algorithm.
  glColor3f(1.0, 1.0, 1.0);
  dda_line(WIDTH / 2, HEIGHT / 2, mx, my);

  glutSwapBuffers();

  usleep(10000);
  glutPostRedisplay();
}

int main (int argc, char **argv) {
  // Initialize glut.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Draw Line");

  // Initialize OpenGL.
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glPointSize(2.0);
  glMatrixMode(GL_PROJECTION);
  glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

  // Add the appropriate callbacks.
  glutDisplayFunc(show_screen);
  glutMotionFunc(handle_mouse_movement);
  glutPassiveMotionFunc(handle_mouse_movement);

  glutMainLoop();
  return 0;
}
