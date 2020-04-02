// COMP-3520 Project
// Adam Vandolder
// Student #104629080

#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <random>
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

template<size_t N>
using Vec = std::array<GLfloat, N>;

// A Polygon is coloured surface made up of vertices.
struct Polygon {
  Vec<3> color;
  std::vector<Vec<3>> vertices;

  Polygon() = default;

  // Create a polygon based on data from an input stream.
  Polygon(std::istream& in) {
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
        Vec<3> v;
        in >> v[0];
        in >> v[1];
        in >> v[2];
        vertices.push_back(v);
      } else if (command == "color") {
        in >> color[0];
        in >> color[1];
        in >> color[2];
      } else if (command == "end") {
        break;
      }
    }
  }

  // Create a polygon from a given color and list of vertices.
  Polygon(std::initializer_list<Vec<3>> vertices, Vec<3> color)
    : color{color}, vertices{vertices} {
  }

  inline auto operator[](int idx) -> Vec<3>& { return vertices[idx]; }
};

// An Object is a collection of polygons with a defined origin and rotation.
struct Object {
  Vec<3> origin;
  Vec<3> rotation_axis;
  GLfloat rotation_angle = 0.0;
  std::vector<Polygon> faces;

  Object() = default;

  Object(const std::string& path) {
    std::ifstream file(path);

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
        file >> origin[0];
        file >> origin[1];
        file >> origin[2];
      } else if (type == "rotation") {
        file >> rotation_angle;
        file >> rotation_axis[0];
        file >> rotation_axis[1];
        file >> rotation_axis[2];
      } else if (type == "face") {
        faces.emplace_back(file);
      }
    }
  }

  auto render() -> void {
    glPushMatrix();
    glTranslatef(origin[0], origin[1], origin[2]);
    glRotatef(rotation_angle,
              rotation_axis[0],
              rotation_axis[1],
              rotation_axis[2]);

    for (auto& face : faces) {
      glColor3fv(face.color.data());
      glBegin(GL_POLYGON);
      for (auto& v : face.vertices) {
        glNormal3fv(v.data());
        glVertex3f(v[0], v[1], v[2]);
      }
      glEnd();
    }

    glPopMatrix();
  }
};

struct Light {
  Light(GLenum light_id,
        Vec<4> ambient,
        Vec<4> diffuse,
        Vec<4> specular,
        Vec<4> position)
      : light_id{light_id},
        enabled{false} {
    glLightfv(light_id, GL_AMBIENT, ambient.data());
    glLightfv(light_id, GL_DIFFUSE, diffuse.data());
    glLightfv(light_id, GL_SPECULAR, specular.data());
    glLightfv(light_id, GL_POSITION, position.data());
  }
  
  auto enable() -> void {
    glEnable(light_id);
    enabled = true;
  }

  auto disable() -> void {
    glDisable(light_id);
    enabled = false;
  }

  auto toggle() -> bool {
    if (enabled) {
      disable();
    } else {
      enable();
    }
    return enabled;
  }

 private:
  GLenum light_id;
  bool enabled;
};

GLfloat width = 500.0, height = 500.0;
int fps = 60;
unsigned int frame_time = 1000 / fps;

// Use a key_down array so that held keys will be repeated.
bool key_down[256] = {false};

Vec<3> eye {10.0, 100.0, 10.0};
GLfloat horz_rot, vert_rot;

GLfloat global_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat global_emission[] = {0.0, 0.0, 0.0, 1.0};
GLfloat global_ambient[] = {0.8, 0.8, 0.8, 1.0};

std::vector<Object> world;
std::vector<Light> lights;
Object vehicle;
Object pedestrian;

auto create_building(Vec<3> origin, Vec<2> size, Vec<3> color) -> Object {
  // Buildings are made up of 5 polygons, the 4 walls and the roof.
  Object building;
  building.origin = origin;

  building.faces.push_back(Polygon({
      {size[0], size[1], size[0]},
      {size[0], 0, size[0]},
      {size[0], 0, 0},
      {size[0], size[1], 0},
    },
    color
  ));

  building.faces.push_back(Polygon({
      {size[0], size[1], size[0]},
      {size[0], 0, size[0]},
      {0, 0, size[0]},
      {0, size[1], size[0]},
    },
    color
  ));

  building.faces.push_back(Polygon({
      {0, size[1], size[0]},
      {0, 0, size[0]},
      {0, 0, 0},
      {0, size[1], 0},
    },
    color
  ));

  building.faces.push_back(Polygon({
      {size[0], size[1], 0},
      {size[0], 0, 0},
      {0, 0, 0},
      {0, size[1], 0},
    },
    color
  ));

  building.faces.push_back(Polygon({
      {size[0], size[1], size[0]},
      {size[0], size[1], 0},
      {0, size[1], 0},
      {0, size[1], size[0]},
    },
    color
  ));

  return building;
}

auto create_plane(Vec<3> origin, Vec<2> size, Vec<3> color) -> Object {
  Object plane;
  plane.origin = origin;
  plane.faces.push_back(Polygon({
      {0, 0, 0},
      {size[0], 0, 0},
      {size[0], 0, size[1]},
      {0, 0, size[1]},
    },
    color
  ));

  return plane;
}

auto tick(UNUSED int v) -> void {
  // Run tick again, at approximately 60 FPS.
  glutTimerFunc(frame_time, tick, 0);

  // Calculate time elapsed since the last frame.
  int time = glutGet(GLUT_ELAPSED_TIME);
  static int prev_time = 0;
  double dt = (time - prev_time) / 1000.0;
  prev_time = time;

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

  if (key_down[27]) {
    // Quit on escape key.
    std::exit(0);
  }

  glutPostRedisplay();
}

auto render() -> void {
  // Set up the view.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, width / height, 1.0, 1000.0);

  // Set up the camera to look at the origin.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye[0], eye[1], eye[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glRotatef(horz_rot, 0.0, 1.0, 0.0);
  glRotatef(vert_rot, 1.0, 0.0, 0.0);
  for (auto& object : world) {
    object.render();
  }
  vehicle.render();
  pedestrian.render();

  glutSwapBuffers();
}

auto reshape(int w, int h) -> void {
  width = w;
  height = h;
}

auto handle_mouse(int button, int state, int x, int y) -> void {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    // Normalize the pixel coordinates to the [-1, 1] interval.
    GLfloat dx = (x * 2.0) / width - 1.0;
    GLfloat dy = ((height - y) * 2.0) / height - 1.0;
    // Then shift the X/Y position of the camera.
    eye[0] += dx;
    eye[1] += dy;
  }
}

auto handle_key(unsigned char key, UNUSED int x, UNUSED int y) -> void {
  key_down[key] = true;
}

auto handle_keyup(unsigned char key, UNUSED int x, UNUSED int y) -> void {
  key_down[key] = false;
}

auto menu(int option) -> void {
  switch (option) {
    case 1:
      eye[2] += 5.0;
      break;
    case 2:
      eye[2] -= 5.0;
      break;
    case 3:
    case 4:
    case 5:
      lights[option - 3].toggle();
      break;
    case 6:
      std::exit(0);
  }
}

auto main (int argc, char **argv) -> int {
  // Initialize OpenGL and GLUT.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("COMP3520 Project");

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
  glutAddMenuEntry("Toggle Light 0", 3);
  glutAddMenuEntry("Toggle Light 1", 4);
  glutAddMenuEntry("Toggle Light 2", 5);
  glutAddMenuEntry("Quit", 6);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  // Set up the global lighting.
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  // Set up the material.
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, global_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, global_emission);
  glEnable(GL_COLOR_MATERIAL);

  // Initialize the lights.
  lights.emplace_back(GL_LIGHT0,
                      Vec<4>{0.0, 0.0, 0.0, 1.0}, // ambient
                      Vec<4>{0.5, 0.5, 0.5, 1.0}, // diffuse
                      Vec<4>{0.5, 0.5, 0.5, 1.0}, // specular
                      Vec<4>{10.0, 10.0, 10.0, 1.0}); // position

  lights.emplace_back(GL_LIGHT1,
                      Vec<4>{0.0, 0.0, 0.0, 1.0},
                      Vec<4>{1.0, 0.2, 0.2, 1.0},
                      Vec<4>{1.0, 0.2, 0.2, 1.0},
                      Vec<4>{2.0, 2.0, -10.0, 1.0});

  lights.emplace_back(GL_LIGHT2,
                      Vec<4>{0.0, 0.0, 0.0, 1.0},
                      Vec<4>{0.8, 0.8, 0.2, 1.0},
                      Vec<4>{0.8, 0.8, 0.2, 1.0},
                      Vec<4>{-100.0, 100.0, 0.0, 1.0});

  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  for (auto& light : lights) {
    // light.enable();
  }

  // Add grass/ground.
  world.push_back(create_plane({-100.0, 0.0, -100.0}, {200.0, 200.0}, {0.2, 1.0, 0.2}));
  // Add the road.
  world.push_back(create_plane({-100.0, 0.01, -5.0}, {200.0, 10.0}, {0.3, 0.3, 0.3}));
  // Add sidewalks.
  world.push_back(create_plane({-100.0, 0.02, -8.0}, {200.0, 3.0}, {0.6, 0.6, 0.6}));
  world.push_back(create_plane({-100.0, 0.02, 5.0}, {200.0, 3.0}, {0.6, 0.6, 0.6}));

  // Create a RNG for generating random buildings.
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 10);
  std::uniform_real_distribution<> color_dis(0.0, 1.0);
  int x = 50;
  // Add buildings next to the road.
  for (int i = 0; i < 10; i++) {
    int bw = 10 + dis(gen);
    int bh = 15 + 2 * dis(gen);
    x -= bw;

    float r = color_dis(gen);
    float g = color_dis(gen);
    float b = color_dis(gen);

    world.push_back(create_building({x, 0.0, 8.0}, {bw, bh}, {r, g, b}));
  }

  glutMainLoop();
}
