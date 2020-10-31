#ifndef _2048_ENGINE_ENGINE_H_
#define _2048_ENGINE_ENGINE_H_

#include "logic/logic.h"
#include "display/display.h"
#include "animation/animation.h"

class Engine {
 public:
  Engine(int32_t length)
      : logic_(length)
      , animation_(logic_.GetMatrix())
      , state_(States::kArising)
      , key_pressed_(Keys::kNoKey) {
    Draw();
  }

  void MainLoop();

 private:
  enum class States {
    kTurn,
    kSuccess,
    kFail,
    kMoving,
    kArising,
  };

  Keys GetPressedKey();
  void Move();
  void Draw();
  void Turn();
  void UpdateArising();
  void UpdateMoving();

  Logic logic_;
  Display display_;
  Animation animation_;
  States state_;
  Keys key_pressed_;
};

#endif
