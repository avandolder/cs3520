#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
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
};
std::vector<Object> objects;

GLfloat width = 500.0, height = 500.0;
int fps = 60;
unsigned int frame_time = 1000 / fps;
Vertex eye(0.0, 0.0, -5.0);

Object read_object(std::string path) {
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

void render_object(Object& obj) {
  for (auto& face : obj.faces) {
    glPushMatrix();
    glColor3f(face.color[0], face.color[1], face.color[2]);
    glTranslatef(obj.origin[0], obj.origin[1], obj.origin[2]);
    glBegin(GL_POLYGON);
    for (auto& v : face.vertices) {
      glVertex3f(v[0], v[1], v[2]);
    }
    glEnd();
    glPopMatrix();
  }
}

void tick(int v) {
  // Run tick again, at approximately 60 FPS.
  glutTimerFunc(frame_time, tick, 0);

  std::stringstream title;
  title << "Assignment 2 X:" << eye[0] << " Y:" << eye[1] << " Z:" << eye[2];
  glutSetWindowTitle(title.str().c_str());

  glutPostRedisplay();
}

void render() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (GLdouble)width / height, 1.0, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye[0], eye[1], eye[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto& obj : objects) {
    render_object(obj);
  }

  glutSwapBuffers();
}

void reshape(int w, int h) {
  width = w;
  height = h;
}

void handle_mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    eye[0] += x / width;
    eye[1] += (height - y) / height;
  }
}

void handle_key(unsigned char key, int x, int y) {
  switch (key) {
    case 27: // Escape key
      std::exit(0);
  }
}

void menu(int option) {
  switch (option) {
    case 1:
      eye[2] += 1.0;
      break;
    case 2:
      eye[2] -= 1.0;
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
  glutTimerFunc(1, tick, 0);

  glutCreateMenu(menu);
  glutAddMenuEntry("Increment Eye Z", 1);
  glutAddMenuEntry("Decrement Eye Z", 2);
  glutAddMenuEntry("Quit", 3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  // Load the object.
  objects.push_back(read_object("square.obj"));

  glutMainLoop();
}
