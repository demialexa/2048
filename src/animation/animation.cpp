#include "animation/animation.h"
#include <chrono>

static Tiles GetNextTile(Tiles tile) {
  if (tile == Tiles::kTile_2048)
    throw std::runtime_error("next tile for 2048 is undefined");
  if (tile == Tiles::kNoTile)
    return Tiles::kNoTile;
  int32_t tile_number = static_cast<int32_t>(tile);
  return static_cast<Tiles>(tile_number + 1);
}

static Tiles GetPreviousTile(Tiles tile) {
  if (tile == Tiles::kTile_2)
    throw std::runtime_error("previous tile for 2 is undefined");
  if (tile == Tiles::kNoTile)
    return Tiles::kNoTile;
  int32_t tile_number = static_cast<int32_t>(tile);
  return static_cast<Tiles>(tile_number - 1);
}

bool Animation::IsMovingFinished() const {
  for (auto& tile : tiles_to_draw_) {
    if (tile.state != TileStates::kMoving
        && tile.state != TileStates::kMerging)
      continue;
    bool is_vertical = tile.direction == Directions::kUp ||
                       tile.direction == Directions::kDown;
    if ((is_vertical && tile.current != tile.dst_row)
        || (!is_vertical && tile.current != tile.dst_column))
      return false;
  }
  return true;
}

bool Animation::IsArisingFinished() const {
  for (auto& tile : tiles_to_draw_) {
    if ((tile.state == TileStates::kArising && tile.opacity != 1)
        || (tile.state == TileStates::kDying && tile.opacity != 0))
      return false;
  }
  return true;
}

static int32_t GetCurrentTime() {
  using namespace std::chrono;
  milliseconds ms = duration_cast<milliseconds>(
      system_clock::now().time_since_epoch()
  );
  return ms.count();
}

Animation::Animation(const std::vector<std::vector<Logic::TileInfo>>& logic_matrix) {
  for (size_t i = 0; i < logic_matrix.size(); i++) {
    for (size_t j = 0; j < logic_matrix[i].size(); j++) {
      const Logic::TileInfo& tile = logic_matrix[i][j];
      if (tile.value == Tiles::kNoTile)
        continue;
      Tiles value = GetValue(tile);
      /* same state */
      int32_t dst_row = i, dst_column = j;
      double current = tile.source_1, speed = GetSpeed(tile, i , j);
      double opacity = (tile.state == TileStates::kArising) ? 0 : 1;
      Animation::TileInfo animation_tile(value, tile.state,
                                         dst_row, dst_column, current ,
                                         speed, opacity, tile.direction);
      tiles_to_draw_.push_back(animation_tile);

      if (animation_tile.state != TileStates::kMerging)
        continue;
      animation_tile.current = tile.source_2;
      animation_tile.speed = GetSpeed(tile, i, j, true);
      tiles_to_draw_.push_back(animation_tile);
    }
  }
  old_time_ = GetCurrentTime();
  elapsed_time_ = 0;
}

Tiles Animation::GetValue( const Logic::TileInfo& logic_tile) const {
  if (logic_tile.state == TileStates::kMerging)
    return GetPreviousTile(logic_tile.value);
  return logic_tile.value;
}

double Animation::GetSpeed(const Logic::TileInfo& tile,
                           int32_t dst_row, int32_t dst_column,
                           bool is_second_speed) const {
  if (tile.state != TileStates::kMoving && tile.state != TileStates::kMerging)
    return 0;
  bool is_vertical = tile.direction == Directions::kUp ||
                     tile.direction == Directions::kDown;
  int32_t destination = is_vertical ? dst_row : dst_column;
  int32_t source = is_second_speed ? tile.source_2 : tile.source_1;
  return (destination - source) * kBaseMovingRate;
}

void Animation::UpdateMoving() {
  int32_t new_time = GetCurrentTime();
  elapsed_time_ = new_time - old_time_;
  old_time_ = new_time;
  for (auto& tile : tiles_to_draw_) {
    if (tile.state != TileStates::kMoving && tile.state != TileStates::kMerging)
      continue;
    double offset = tile.speed * elapsed_time_ / 1000.0;
    tile.current += offset;
    if ((tile.direction == Directions::kLeft
        && tile.current < tile.dst_column)
        || (tile.direction == Directions::kRight
        && tile.current > tile.dst_column)) {
      tile.current = tile.dst_column;
    } else if ((tile.direction == Directions::kUp
        && tile.current < tile.dst_row)
        || (tile.direction == Directions::kDown
        && tile.current > tile.dst_row)) {
      tile.current = tile.dst_row;
    }
  }
}

void Animation::Merge() {
  std::vector<Animation::TileInfo>::iterator i;
  for (i = tiles_to_draw_.begin(); i != tiles_to_draw_.end(); i++) {
    if (i->state != TileStates::kMerging)
      continue;
    i->state = TileStates::kArising;
    i->value = GetNextTile(i->value);
    i->opacity = 0;
    (i + 1)->state = TileStates::kDying;
  }
}
 
void Animation::UpdateArising() {
  int32_t new_time = GetCurrentTime();
  elapsed_time_ = new_time - old_time_;
  old_time_ = new_time;
  for (auto& tile : tiles_to_draw_) {
    if (tile.state == TileStates::kArising) {
      tile.opacity += elapsed_time_ * kArisingRate / 1000;
      if (tile.opacity > 1)
        tile.opacity = 1;
    } else if (tile.state == TileStates::kDying) {
      tile.opacity -= elapsed_time_ * kArisingRate / 1000;
      if (tile.opacity < 0)
        tile.opacity = 0;
    }
  }
}
