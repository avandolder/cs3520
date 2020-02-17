#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <GL/freeglut.h>
#include <GL/gl.h>

struct Vertex {
  GLfloat coords[3] = {0.0, 0.0, 0.0};

  Vertex() {}
  Vertex(GLfloat x, GLfloat y, GLfloat z) : coords{x, y, z} {}

  inline GLfloat& operator[](int idx) { return coords[idx]; }
};

struct Polygon {
  Vertex origin {0.0, 0.0, 0.0};
  std::vector<Vertex> vertices;
  GLfloat color[3];

  inline Vertex& operator[](int idx) { return vertices[idx]; }
};

struct Object {
  Vertex origin {0.0, 0.0, 0.0};
  std::vector<Polygon> faces;

  static Object read(const std::string& path) {
    std::ifstream file(path);
    Object object;

    std::string type;
    Polygon face;
    while (file >> type) {
      if (type == "color") {
        file >> face.color[0];
        file >> face.color[1];
        file >> face.color[2];
      } else if (type == "face") {
        face = Polygon();
      } else if (type == "vertex") {
        Vertex v;
        file >> v[0];
        file >> v[1];
        file >> v[2];
        face.vertices.push_back(v);
      } else if (type == "origin") {
        file >> object.origin[0];
        file >> object.origin[1];
        file >> object.origin[2];
      } else if (type == "end") {
        object.faces.push_back(face);
      }
    }

    return object;
  }

  void render() {
    for (auto& face : faces) {
      glPushMatrix();
      glColor3f(face.color[0], face.color[1], face.color[2]);
      glTranslatef(origin[0], origin[1], origin[2]);
      glBegin(GL_POLYGON);
      for (auto& v : face.vertices) {
        glVertex3f(v[0], v[1], v[2]);
      }
      glEnd();
      glPopMatrix();
    }
  }
};
std::vector<Object> objects;

GLfloat width = 500.0, height = 500.0;
int fps = 60;
unsigned int frame_time = 1000 / fps;

Vertex eye;
GLfloat eye_radius = 8.0;
GLfloat eye_theta = M_PI / 2.0;
GLfloat eye_phi = 0.0;

bool key_down[256] = {false};

void tick(int v) {
  // Run tick again, at approximately 60 FPS.
  glutTimerFunc(frame_time, tick, 0);

  std::stringstream title;
  title << "Assignment 2 Radius:" << eye_radius << " Theta:" << eye_theta << " Phi:" << eye_phi;
  glutSetWindowTitle(title.str().c_str());

  // Calculated time elapsed since the last frame.
  int time = glutGet(GLUT_ELAPSED_TIME);
  static int prev_time = 0;
  double dt = (time - prev_time) / 1000.0;

  // Handle key presses.
  if (key_down['d']) {
    eye_phi = std::fmod(eye_phi + M_PI*dt/2, M_PI * 2);
  }
  if (key_down['a']) {
    eye_phi = std::fmod(eye_phi - M_PI*dt/2, M_PI * 2);
  }
  if (key_down['w']) {
    eye_theta = std::max(eye_theta - M_PI*dt/2, 0.001);
  }
  if (key_down['s']) {
    eye_theta = std::min(eye_theta + M_PI*dt/2, M_PI - 0.01);
  }

  glutPostRedisplay();

  prev_time = time;
}

void render() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (GLdouble)width / height, 1.0, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  eye[0] = eye_radius * std::sin(eye_theta) * std::cos(eye_phi);
  eye[2] = eye_radius * std::sin(eye_theta) * std::sin(eye_phi);
  eye[1] = eye_radius * std::cos(eye_theta);
  gluLookAt(eye[0], eye[1], eye[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto& obj : objects) {
    obj.render();
  }

  glutSwapBuffers();
}

void reshape(int w, int h) {
  width = w;
  height = h;
}

void handle_mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    auto xx = eye[0] + x * 2 / width - 1.0;
    auto yy = eye[1] + 2 * (height - y) / height - 1.0;
    std::cout << x * 2 / width - 1.0 << " " << 2 * (height - y) / height << std::endl;

    eye_phi = std::atan2(yy, xx);
    eye_theta = std::atan2(std::sqrt(xx*xx + yy*yy), eye_radius);
  }
}

void handle_key(unsigned char key, int x, int y) {
  if (key == 27) {
    // Quit on escape key.
    std::exit(0);
  }

  key_down[key] = true;
}

void handle_keyup(unsigned char key, int x, int y) {
  key_down[key] = false;
}

void menu(int option) {
  switch (option) {
    case 1:
      eye_radius += 1.0;
      break;
    case 2:
      eye_radius -= 1.0;
      break;
    case 3:
      std::exit(0);
  }
}

int main (int argc, char **argv) {
  // Initialize OpenGL and GLUT.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Assignment 3");
  glEnable(GL_DEPTH_TEST);

  // Add the appropriate callbacks.
  glutDisplayFunc(render);
  glutReshapeFunc(reshape);
  glutMouseFunc(handle_mouse);
  glutKeyboardFunc(handle_key);
  glutKeyboardUpFunc(handle_keyup);
  glutTimerFunc(1, tick, 0);

  glutCreateMenu(menu);
  glutAddMenuEntry("Increment Eye Distance", 1);
  glutAddMenuEntry("Decrement Eye Distance", 2);
  glutAddMenuEntry("Quit", 3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  // Load the object.
  objects.push_back(Object::read("house.obj"));

  glutMainLoop();
}
