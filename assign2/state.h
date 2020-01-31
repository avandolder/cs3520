#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <utility>

class State {
 public:
  virtual ~State() {}

  virtual void tick(float dt) = 0;
  virtual void render() = 0;
  virtual void handle_key(unsigned char key, int x, int y) = 0;
  virtual void handle_mouse(int button, int state, int x, int y) = 0;
};

class PlayState : public State {
 public:
  PlayState(int width, int height) : width(width), height(height) {}

  void tick(float dt) override;
  void render() override;
  void handle_key(unsigned char key, int x, int y) override;
  void handle_mouse(int button, int state, int x, int y) override;

  void render_prey();
  void render_predator();

  std::pair<int, int> path_to_point(float loc);
  float point_to_path(int x, int y);
  float get_facing_angle(float loc);

 private:
  int width, height;

  float prey_loc;
  float prey_speed = 0.1;
  bool prey_active = false;

  float predator_loc;
  float predator_size = 1;
  float predator_growth_rate = 1.15;
  bool predator_active = false;
};

#endif // STATE_H
