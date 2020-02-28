// COMP-3520 Assignment #3
// Adam Vandolder
// Student #104629080
//
// Render a house from an object file and allow the user to move the camera.
// Click on the screen with the left mouse button to shift the X/Y position
// of the camera. Click the the right mouse button to bring up a context menu
// that allows you to adjust the Z position. Use the WASD keys to rotate the
// house object.

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <GL/freeglut.h>
#include <GL/gl.h>

// Suppress unused parameter warnings from gcc. Useful since the GLUT callback
// functions require various parameters that are often unnecessary.
#ifdef __GNUC__
#  define UNUSED __attribute__((unused))
#else
#  define UNUSED
#endif

// A Vertex represents a single (X, Y, Z) coordinate.
struct Vertex {
  GLdouble coords[3] = {0.0, 0.0, 0.0};

  Vertex() {}
  Vertex(GLdouble x, GLdouble y, GLdouble z) : coords{x, y, z} {}

  inline GLdouble& operator[](int idx) { return coords[idx]; }
};

// A Polygon is (potentially coloured) surface made up of vertices.
struct Polygon {
  std::vector<Vertex> vertices;
  GLdouble color[3];

  // Create a polygon based on data from an input stream.
  static Polygon read(std::istream& in) {
    Polygon p;

    while (!in.eof()) {
      if ((in >> std::ws).peek() == '#') {
        // If the first non-whitespace character is a #, the line is a comment
        // so skip it.
        for (char c = in.get(); c != '\n'; c = in.get()) {}
        continue;
      }

      std::string command;
      in >> command;
      if (command == "vertex") {
        Vertex v;
        in >> v[0];
        in >> v[1];
        in >> v[2];
        p.vertices.push_back(v);
      } else if (command == "color") {
        in >> p.color[0];
        in >> p.color[1];
        in >> p.color[2];
      } else if (command == "end") {
        break;
      }
    }

    return p;
  }

  inline Vertex& operator[](int idx) { return vertices[idx]; }
};

// An Object is a collection of polygon with a defined origin and rotation.
struct Object {
  Vertex origin;
  Vertex rotation_axis;
  GLdouble rotation_angle = 0.0;
  std::vector<Polygon> faces;

  static Object read(const std::string& path) {
    std::ifstream file(path);
    Object object;

    while (!file.eof()) {
      if ((file >> std::ws).peek() == '#') {
        // If the first non-whitespace character is a #, the line is a comment
        // so skip it.
        for (char c = file.get(); c != '\n'; c = file.get()) {}
        continue;
      }

      std::string type;
      file >> type;
      if (type == "origin") {
        file >> object.origin[0];
        file >> object.origin[1];
        file >> object.origin[2];
      } else if (type == "rotation") {
        file >> object.rotation_angle;
        file >> object.rotation_axis[0];
        file >> object.rotation_axis[1];
        file >> object.rotation_axis[2];
      } else if (type == "face") {
        object.faces.push_back(Polygon::read(file));
      }
    }

    return object;
  }

  void render() {
    glPushMatrix();
    glTranslated(origin[0], origin[1], origin[2]);
    glRotated(rotation_angle,
              rotation_axis[0],
              rotation_axis[1],
              rotation_axis[2]);

    for (auto& face : faces) {
      glColor3d(face.color[0], face.color[1], face.color[2]);
      glBegin(GL_POLYGON);
      for (auto& v : face.vertices) {
        glVertex3d(v[0], v[1], v[2]);
      }
      glEnd();
    }

    glPopMatrix();
  }
} house = Object::read("house.obj");

GLdouble width = 500.0, height = 500.0;
int fps = 60;
unsigned int frame_time = 1000 / fps;
bool key_down[256] = {false};
Vertex eye {0.0, 0.0, 8.0};
GLdouble horz_rot, vert_rot;

void tick(UNUSED int v) {
  // Run tick again, at approximately 60 FPS.
  glutTimerFunc(frame_time, tick, 0);

  std::stringstream title;
  title << "Assignment 2 (X:" << eye[0] << " Y:" << eye[1] << " Z:" << eye[2]
        << ")";
  glutSetWindowTitle(title.str().c_str());

  // Calculated time elapsed since the last frame.
  int time = glutGet(GLUT_ELAPSED_TIME);
  static int prev_time = 0;
  double dt = (time - prev_time) / 1000.0;

  // Rotate the house object, wrapping around at 360 degrees to prevent
  // overflow.
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
  // Set up the view.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (GLdouble)width / height, 1.0, 100.0);

  // Set up the camera to look at the origin.
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
    // Normalize the pixel coordinates to the [-1, 1] interval.
    GLdouble dx = (x * 2.0) / width - 1.0;
    GLdouble dy = ((height - y) * 2.0) / height - 1.0;
    // Then shift the X/Y position of the camera.
    eye[0] += dx;
    eye[1] += dy;
  }
}

void handle_key(unsigned char key, UNUSED int x, UNUSED int y) {
  if (key == 27) {
    // Quit on escape key.
    std::exit(0);
  }

  // Use a key_down array so that held keys will be repeated.
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
