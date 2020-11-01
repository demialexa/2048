#include "display/display.h"
#include "logic/logic.h"
#include "engine/engine.h"
#include "animation/animation.h"

#include <stdexcept>
#include <iostream>

Keys Engine::GetPressedKey() {
  if (display_.IsKeyPressed(Keys::kKeyLeft))
    return Keys::kKeyLeft;
  if (display_.IsKeyPressed(Keys::kKeyRight))
    return Keys::kKeyRight;
  if (display_.IsKeyPressed(Keys::kKeyUp))
    return Keys::kKeyUp;
  if (display_.IsKeyPressed(Keys::kKeyDown))
    return Keys::kKeyDown;
  return Keys::kNoKey;
}

void Engine::Move() {
  switch (key_pressed_) {
    case Keys::kKeyLeft:
      logic_.MoveLeft();
      break;
    case Keys::kKeyRight:
      logic_.MoveRight();
      break;
    case Keys::kKeyUp:
      logic_.MoveUp();
      break;
    case Keys::kKeyDown:
      logic_.MoveDown();
      break;
    default:
      throw std::runtime_error("no key pressed => no moving");
  }
}

void Engine::Turn() {
  key_pressed_ = GetPressedKey();
  if (key_pressed_ == Keys::kNoKey)
    return;
  Move();
  if (logic_.HasSomethingChanged())
    logic_.NewTile();
  state_ = States::kMoving;
  const std::vector<std::vector<Logic::TileInfo>>&
      logic_matrix = logic_.GetMatrix();
  animation_ = Animation(logic_matrix);
}

void Engine::Draw() {
  const std::vector<Animation::TileInfo>&
      tiles_to_draw = animation_.GetAnimationVector();
  double current_row, current_column;
  for (const auto& tile : tiles_to_draw) {
    if (tile.value == Tiles::kNoTile)
      continue;
    if (tile.state == TileStates::kArising
        || tile.state == TileStates::kDying
        || tile.state == TileStates::kDefault) {
      current_row = tile.dst_row;
      current_column = tile.dst_column;
    } else if (tile.direction == Directions::kUp
        || tile.direction == Directions::kDown) {
      current_row = tile.current;
      current_column = tile.dst_column;
    } else {
      current_row = tile.dst_row;
      current_column = tile.current;
    }
    display_.DrawTile(current_row, current_column, tile.value, tile.opacity);
  }
}

void Engine::UpdateMoving() {
  animation_.UpdateMoving();
  if (animation_.IsMovingFinished()) {
    state_ = States::kArising;
    animation_.Merge();
  }
}

void Engine::UpdateArising() {
  animation_.UpdateArising();
  if (animation_.IsArisingFinished()) {
    if (logic_.IsGameOver()) {
      state_ = (logic_.IsSuccess() ? States::kSuccess : States::kFail);
    } else {
      state_ = States::kTurn;
      logic_.ResetStates();
    }
  }
}

void Engine::MainLoop() {
  Draw();
  key_pressed_ = Keys::kNoKey;
  while (!display_.Closed()) {
    display_.ProcessEvents();
    switch (state_) {
      case States::kTurn:
        Turn();
        break;
      case States::kMoving:
        UpdateMoving();
        break;
      case States::kArising:
        UpdateArising();
        break;
      case States::kSuccess:
        display_.DrawWinMessage();
        break;
      case States::kFail:
      default:
        break;
    }
    Draw();
    display_.Render();
  }
}
