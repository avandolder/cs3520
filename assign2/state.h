#ifndef STATE_H
#define STATE_H

class State {
 public:
  virtual ~State() {}

  virtual void tick(float dt) = 0;
  virtual void render() = 0;
  virtual void handle_key(Key key, int x, int y) = 0;
  virtual void handle_mouse(int button, int state, int x, int y) = 0;
};

class PlayState : public State {
 public:
  PlayState(int width, int height) : width(width), height(height) {}

  void tick(float dt) override;
  void render() override;
  void handle_key(Key key, int x, int y) override;
  void handle_mouse(int button, int state, int x, int y) override;

  void render_prey();
  void render_predator();

 private:
  const int width, height;

  float prey_angle = 0;
  float prey_x, prey_y;
  float prey_speed = 0.1;
  bool prey_active = false;

  float predator_angle = 0;
  float predator_x, predator_y;
  float predator_size = 1;
  bool predator_active = false;
};

#endif // STATE_H
