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

int width = 500, height = 500;
int fps = 60;
unsigned int frame_time = 1000 / fps;

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

using Object = std::vector<Polygon>;

std::vector<Object> objects;

Object read_object(std::string path) {
  std::ifstream file(path);
  Object obj;

  std::string type;
  GLfloat color[3] = {0.0, 0.0, 0.0};
  Polygon face;
  while (file >> type) {
    if (type == "color") {
      file >> color[0];
      file >> color[1];
      file >> color[2];
    } else if (type == "face") {
      if (obj.size() > 0) {
        obj.push_back(face);
      }

      face = Polygon();
      std::copy(std::begin(color), std::end(color), std::begin(face.color));
    } else if (type == "vertex") {
      Vertex v;
      file >> v[0];
      file >> v[1];
      file >> v[2];
      face.vertices.push_back(v);
    }
  }

  obj.push_back(face);

  return obj;
}

void render_object(Object& obj) {
  for (auto& face : obj) {
    glColor3f(face.color[0], face.color[1], face.color[2]);
    glBegin(GL_POLYGON);
    for (auto& v : face.vertices) {
      glVertex2f(v[0], v[1]); //, v[2]);
    }
    glEnd();
  }
}

void tick(int v) {
  // Run tick again, at approximately 60 FPS.
  glutTimerFunc(frame_time, tick, 0);

  // Calculated time elapsed since the last frame.
  int time = glutGet(GLUT_ELAPSED_TIME);
  static int prev_time = 0;
  int elapsed_time = time - prev_time;

  std::stringstream title;
  title << "Assignment 2 FPS: " << 1000 / elapsed_time;
  glutSetWindowTitle(title.str().c_str());

  glutPostRedisplay();

  prev_time = time;
}

void render() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto& obj : objects) {
    render_object(obj);
  }

  glutSwapBuffers();
}

void reshape(int w, int h) {
  width = w;
  height = h;

  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
}

void handle_mouse(int button, int bstate, int x, int y) {
}

void handle_key(unsigned char key, int x, int y) {
  switch (key) {
    case 27: // Escape key
      std::exit(0);
  }
}

int main (int argc, char **argv) {
  // Initialize glut.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Assignment 2");

  // Initialize OpenGL.
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, width, 0.0, height);

  // Add the appropriate callbacks.
  glutDisplayFunc(render);
  glutReshapeFunc(reshape);
  glutMouseFunc(handle_mouse);
  glutKeyboardFunc(handle_key);
  glutTimerFunc(1, tick, 0);
  
  // Load the objects.
  objects.push_back(read_object("square.obj"));

  glutMainLoop();

  return 0;
}
