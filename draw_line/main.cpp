#include <math.h>
#include <unistd.h>
#include <utility>

#include <GL/freeglut.h>
#include <GL/gl.h>

const int WIDTH = 500, HEIGHT = 500;
int points[2][2];

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
    double m = (y1 - y2) / (double)(x1 - x2);
    if (fabs(m) <= 1) {
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
  int dx = x2 - x1;
  int dy = y2 - y1;

  if (dx == 0) {
    // Vertical line
    for (; y <= y2; ++y) {
      glVertex2f(x, y);
    }
  } else if (dy == 0) {
    // Horizontal line
    for (; x <= x2; ++x) {
      glVertex2f(x, y);
    }
  } else if (dx == dy) {
    // Slope is 1.
    int stepx = (dx > 0) ? 1 : -1;
    int stepy = (dy > 0) ? 1 : -1;
    for (; x <= x2; x += stepx, y += stepy) {
      glVertex2f(x, y);
    }
  } else {
    int dx2 = 2 * dx;
    int dy2 = 2 * dy;
    double m = dy / (double)dx;
    int step = (m > 0) ? 1 : -1;

    if (fabs(m) < 1) {
      int diff = dy2 - dx2;
      int p = dy2 - dx;

      while (x <= x2) {
        glVertex2f(x, y);
        ++x;
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

      while (y <= y2) {
        glVertex2f(x, y);
        ++y;
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

void handle_mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    points[0][0] = points[1][0];
    points[0][1] = points[1][1];
    points[1][0] = x;
    points[1][1] = HEIGHT - y;
  }
}

void show_screen() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(1.0, 1.0, 1.0);
  dda_line(points[0][0], points[0][1], points[1][0], points[1][1]);

  glColor3f(0.0, 0.0, 1.0);
  bresenham_line(0, 0, 250, 125);
  glColor3f(1.0, 0, 0);
  bresenham_line(0, 250, 125, 500);
  glColor3f(0, 1, 0);
  bresenham_line(0, 250, 500, 250);
  bresenham_line(250, 0, 250, 500);
  glColor3f(1, 1, 0);
  bresenham_line(250, 250, 500, 500);

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
  glPointSize(1.0);
  glMatrixMode(GL_PROJECTION);
  glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

  // Add the appropriate callbacks.
  glutDisplayFunc(show_screen);
  glutMouseFunc(handle_mouse);

  glutMainLoop();
  return 0;
}
