#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <GL/freeglut.h>
#include <GL/gl.h>

// Suppress unused parameter warnings from gcc.
#ifdef __GNUC__
#  define UNUSED __attribute__((unused))
#else
#  define UNUSED
#endif

struct Vertex {
  GLdouble coords[3] = {0.0, 0.0, 0.0};

  Vertex() {}
  Vertex(GLdouble x, GLdouble y, GLdouble z) : coords{x, y, z} {}

  inline GLdouble& operator[](int idx) { return coords[idx]; }
};

struct Polygon {
  std::vector<Vertex> vertices;
  GLdouble color[3];

  inline Vertex& operator[](int idx) { return vertices[idx]; }
};

struct Object {
  Vertex origin;
  Vertex rotation_axis;
  GLdouble rotation_angle = 0.0;

  std::vector<Polygon> faces;

  static Object read(const std::string& path) {
    std::ifstream file(path);
    Object object;

    std::string line;
    Polygon face;
    while (std::getline(file, line)) {
      if (line[0] == '#') {
        // Skip comment lines.
        continue;
      }

      std::istringstream str(line);
      std::string type;
      str >> type;
      if (type == "color") {
        str >> face.color[0];
        str >> face.color[1];
        str >> face.color[2];
      } else if (type == "face") {
        face = Polygon();
      } else if (type == "vertex") {
        Vertex v;
        str >> v[0];
        str >> v[1];
        str >> v[2];
        face.vertices.push_back(v);
      } else if (type == "origin") {
        str >> object.origin[0];
        str >> object.origin[1];
        str >> object.origin[2];
      } else if (type == "rotation") {
        str >> object.rotation_angle;
        str >> object.rotation_axis[0];
        str >> object.rotation_axis[1];
        str >> object.rotation_axis[2];
      } else if (type == "end") {
        object.faces.push_back(face);
      }
    }

    return object;
  }

  void render() {
    for (auto& face : faces) {
      glPushMatrix();
      glColor3d(face.color[0], face.color[1], face.color[2]);
      glTranslated(origin[0], origin[1], origin[2]);
      glRotated(rotation_angle, rotation_axis[0], rotation_axis[1], rotation_axis[2]);

      glBegin(GL_POLYGON);
      for (auto& v : face.vertices) {
        glVertex3d(v[0], v[1], v[2]);
      }
      glEnd();
      
      glPopMatrix();
    }
  }
} house = Object::read("house.obj");

GLdouble width = 500.0, height = 500.0;
int fps = 60;
unsigned int frame_time = 1000 / fps;

Vertex eye {0.0, 0.0, 8.0};
GLdouble horz_rot, vert_rot;

bool key_down[256] = {false};

void tick(UNUSED int v) {
  // Run tick again, at approximately 60 FPS.
  glutTimerFunc(frame_time, tick, 0);

  std::stringstream title;
  title << "Assignment 2 X:" << eye[0] << " Y:" << eye[1] << " Z:" << eye[2];
  glutSetWindowTitle(title.str().c_str());

  // Calculated time elapsed since the last frame.
  int time = glutGet(GLUT_ELAPSED_TIME);
  static int prev_time = 0;
  double dt = (time - prev_time) / 1000.0;

  // Handle key presses.
  if (key_down['d']) {
    horz_rot = std::fmod(horz_rot + 360*dt/2, 360);
  }
  if (key_down['a']) {
    horz_rot = std::fmod(horz_rot - 360*dt/2, 360);
  }
  if (key_down['w']) {
    vert_rot = std::fmod(vert_rot + 360*dt/2, 360);
  }
  if (key_down['s']) {
    vert_rot = std::fmod(vert_rot - 360*dt/2, 360);
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

  gluLookAt(eye[0], eye[1], eye[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glRotated(horz_rot, 0.0, 1.0, 0.0);
  glRotated(vert_rot, 1.0, 0.0, 0.0);

  house.render();

  glutSwapBuffers();
}

void reshape(int w, int h) {
  width = w;
  height = h;
}

void handle_mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    GLdouble dx = (x * 2.0) / width - 1.0;
    GLdouble dy = ((height - y) * 2.0) / height - 1.0;
    eye[0] += dx;
    eye[1] += dy;
  }
}

void handle_key(unsigned char key, UNUSED int x, UNUSED int y) {
  if (key == 27) {
    // Quit on escape key.
    std::exit(0);
  }

  key_down[key] = true;
}

void handle_keyup(unsigned char key, UNUSED int x, UNUSED int y) {
  key_down[key] = false;
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
  glutKeyboardUpFunc(handle_keyup);
  glutTimerFunc(1, tick, 0);

  // Create the context menu.
  glutCreateMenu(menu);
  glutAddMenuEntry("Increment Eye Z", 1);
  glutAddMenuEntry("Decrement Eye Z", 2);
  glutAddMenuEntry("Quit", 3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutMainLoop();
}
