#include <cmath>
#include <cstdlib>
#include <sstream>

#include <GL/freeglut.h>
#include <GL/gl.h>

#include "state.h"

int width = 500, height = 500;
int fps = 60;
unsigned int frame_time = 1000 / fps;
State* state = new PlayState(width, height);

void tick(int v) {
  // Run tick again, at approximately 60 FPS.
  glutTimerFunc(frame_time, tick, 0);

  // Calculated time elapsed since the last frame.
  int time = glutGet(GLUT_ELAPSED_TIME);
  static int prev_time = 0;
  int elapsed_time = time - prev_time;

  std::stringstream title;
  title << "FPS: " << 1000 / elapsed_time;
  glutSetWindowTitle(title.str().c_str());

  state->tick(elapsed_time / 1000.0);
  glutPostRedisplay();

  prev_time = time;
}

void render() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  state->render();

  glutSwapBuffers();
}

void reshape(int w, int h) {
  width = w;
  height = h;

  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);

  state->reshape(w, h);
}

void handle_mouse(int button, int bstate, int x, int y) {
  state->handle_mouse(button, bstate, x, y);
}

void handle_key(unsigned char key, int x, int y) {
  state->handle_key(key, x, y);
}

int main (int argc, char **argv) {
  // Initialize glut.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Draw Line");

  // Initialize OpenGL.
  glPointSize(1.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, 0, height, -1, 1);

  // Add the appropriate callbacks.
  glutDisplayFunc(render);
  glutReshapeFunc(reshape);
  glutMouseFunc(handle_mouse);
  glutKeyboardFunc(handle_key);
  glutTimerFunc(1, tick, 0);

  glutMainLoop();

  return 0;
}
