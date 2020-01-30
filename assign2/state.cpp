#include <cstdlib>

#include <GL/freeglut.h>
#include <GL/gl.h>

#include "key.h"
#include "state.h"

void PlayState::tick(float dt) {
  if (prey_active && predator_active) {
  }
}

void PlayState::render() {
  // Draw the path.
  glColor3f(1, 1, 1);
  glBegin(GL_LINE_LOOP);
    glVertex2f(100, 100);
    glVertex2f(width - 100, 100);
    glVertex2f(width - 100, height - 100);
    glVertex2f(100, height - 100);
  glEnd();
  glBegin(GL_LINE_STRIP);
    glVertex2f(width - 25, height / 2 - 25);
    glVertex2f(width - 50, height / 2 - 25);
    glVertex2f(width - 50, 50);
    glVertex2f(50, 50);
    glVertex2f(50, height - 50);
    glVertex2f(width - 50, height - 50);
    glVertex2f(width - 50, height / 2 + 25);
    glVertex2f(width - 25, height / 2 + 25);
  glEnd();

  if (prey_active) {
    render_prey();
  }
  if (predator_active) {
    render_predator();
  }
}

void PlayState::render_prey() {
  glPushMatrix();
  glTranslatef(prey_x, prey_y, 0);
  glRotatef(prey_angle, 0, 0, 1);
  glColor3f(0, 1, 0);

  glBegin(GL_POLYGON);
    glVertex2f(-20, -20);
    glVertex2f( 20, -20);
    glVertex2f( 20,  20);
    glVertex2f(-20,  20);
  glEnd();

  glPopMatrix();
}

void PlayState::render_predator() {
  glPushMatrix();
  glTranslatef(predator_x, predator_y, 0);
  glRotatef(predator_angle, 0, 0, 1);
  glColor3f(1, 0, 0);

  glBegin(GL_POLYGON);
    glVertex2f(-20, -20);
    glVertex2f( 20, -20);
    glVertex2f( 0,   20);
  glEnd();

  glPopMatrix();
}

void PlayState::handle_key(Key key, int x, int y) {
  switch (key) {
    case Key::Escape:
      std::exit(0);
    case Key::Space:
      break;
  }
}

void PlayState::handle_mouse(int button, int bstate, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && bstate == GLUT_DOWN) {
    if (!prey_active) {
      prey_active = true;
      prey_x = x;
      prey_y = height - y;
      prey_angle = 0;
    } else if (!predator_active) {
      predator_active = true;
      predator_x = x;
      predator_y = height - y;
      predator_angle = 0;
    }
  }
}
