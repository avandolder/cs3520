#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <utility>

#include <GL/freeglut.h>
#include <GL/gl.h>

#include "state.h"

// Convert a point on the screen to a location on the path.
float PlayState::point_to_path(int x, int y) {
  // Use rectangle-point collision to make sure point is on the path.
  bool inside_outer_loop = x >= 50 && x <= width - 50 &&
                           y >= 50 && y <= height - 50;
  bool inside_inner_loop = x >= 100 && x <= width - 100 &&
                           y >= 100 && y <= height - 100;
  if (inside_inner_loop || !inside_outer_loop) {
    return -1;
  }

  // Clamp points to within the center of the path.
  x = std::min(std::max(x, 75), width - 75);
  y = std::min(std::max(y, 75), height - 75);

  if (x >= 50 && x <= 100) {
    // Point is on the left segment of the path.
    return 0.375 + 0.25 * (float)(height - y - 75) / (height - 150);
  } else if (x >= width - 100 && x <= width - 50) {
    if (y >= height / 2) {
      // Point is on the upper-half of the right segment of the path.
      return 0.25 * (float)(y - height/2) / (height - 150);
    } else {
      // Point is on the lower-half of the right segment of the path.
      return 0.875 + 0.125 * (float)(y - 75) / (height/2 - 75);
    }
  } else if (y >= 50 && y <= 100) {
    // Point is on the bottom segment of the path.
    return 0.625 + 0.25 * (float)(x - 75) / (width - 150);
  } else if (y >= height - 100 && y <= height - 50) {
    // Point is on the top segment of the path.
    return 0.125 + 0.25 * (float)(width - x - 75) / (width - 150);
  }

  return 0;
}

// Convert between a location on the path and a point on the screen.
std::pair<int, int> PlayState::path_to_point(float loc) {
  if (0 <= loc && loc < 0.125) {
    return {width - 75, ((height - 150) / 2) * loc / 0.125 + height / 2};
  } else if (loc < 0.375) {
    return {(width - 150) * (0.375 - loc) / 0.25 + 75, height - 75};
  } else if (loc < 0.625) {
    return {75, (height - 150) * (0.625 - loc) / 0.25 + 75};
  } else if (loc < 0.875) {
    return {(width - 150) * (loc - 0.625) / 0.25 + 75, 75};
  } else if (loc <= 1) {
    return {width - 75, ((height - 150) / 2) * (loc - 0.875) / 0.125 + 75};
  }
  return {0, 0};
}

float PlayState::get_facing_angle(float loc) {
  if (0 <= loc && loc < 0.125) {
    return 0;
  } else if (loc < 0.375) {
    return 90;
  } else if (loc < 0.625) {
    return 180;
  } else if (loc < 0.875) {
    return 270;
  } else if (loc <= 1) {
    return 0;
  }

  return 0;
}

void PlayState::tick(float dt) {
  if (prey_active && predator_active) {
    prey_loc += prey_speed * dt;

    predator_loc += prey_speed * dt * 2;
    if (predator_loc < 0) {
      predator_loc = 1;
    } else if (predator_loc > 1) {
      predator_loc = 0;
    }

    if (prey_loc >= 1) {
      // Prey has escaped.
      prey_active = false;
    } else if (prey_loc - 0.01 <= predator_loc && prey_loc + 0.01 >= predator_loc) {
      // Predator has reached the prey.
      prey_active = false;
      predator_size *= predator_growth_rate;
    }
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
  auto pt = path_to_point(prey_loc);

  glPushMatrix();
  glTranslatef(pt.first, pt.second, 0);
  glRotatef(get_facing_angle(prey_loc), 0, 0, 1);
  glScalef(15, 15, 0);
  glColor3f(0, 1, 0);

  glBegin(GL_POLYGON);
    glVertex2f(-0.75, -1);
    glVertex2f( 0.75, -1);
    glVertex2f( 0.75,  1);
    glVertex2f(-0.75,  1);
  glEnd();

  glPopMatrix();
}

void PlayState::render_predator() {
  auto pt = path_to_point(predator_loc);

  glPushMatrix();
  glTranslatef(pt.first, pt.second, 0);
  glRotatef(get_facing_angle(predator_loc), 0, 0, 1);
  glScalef(15 * predator_size, 15 * predator_size, 0);
  glColor3f(1, 0, 0);

  glBegin(GL_POLYGON);
    glVertex2f(-1, -1);
    glVertex2f( 1, -1);
    glVertex2f( 0,  1);
  glEnd();

  glPopMatrix();
}

void PlayState::handle_key(unsigned char key, int x, int y) {
  switch (key) {
    case '\e':
      std::exit(0);
  }
}

void PlayState::handle_mouse(int button, int bstate, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && bstate == GLUT_DOWN) {
    if (!prey_active) {
      prey_loc = point_to_path(x, height - y);
      if (prey_loc != -1) {
        prey_active = true;
      }
    } else if (!predator_active) {
      predator_loc = point_to_path(x, height - y);
      if (predator_loc != -1) {
        predator_active = true;
      }
    }
  }
}
