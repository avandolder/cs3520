#include <GL/freeglut.h>
#include <GL/gl.h>

void show_screen() {
  glClearColor(0.4, 0.4, 0.4, 0.4);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, 500, 500);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 500, 0.0, 500, 0.0, 1.0);

  glColor3f(1.0, 0.0, 1.0);
  glBegin(GL_QUADS);
    glVertex2f(100, 100);
    glVertex2f(200, 100);
    glVertex2f(200, 200);
    glVertex2f(100, 200);
  glEnd();

  glFlush();
}

int main (int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("OpenGL");
  glutDisplayFunc(show_screen);
  glutMainLoop();
  return 0;
}
