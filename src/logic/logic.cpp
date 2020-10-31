#include "logic/logic.h"
#include "display/display.h"

#include <stdexcept>
#include <cstdint>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>

Logic::Logic(int32_t length)
    : length_(length)
    , tile_matrix_(length, std::vector<TileInfo>(length,
                                              TileInfo(Tiles::kNoTile)))
    , free_(length_ * length_)
    , game_over_(false)
    , success_(false) {
  srand(time(NULL));
  for (size_t i = 0; i < kInitialTilesNumber; i++)
    NewTile();
}

static Tiles GetNextTile(Tiles tile) {
  if (tile == Tiles::kTile_2048)
    throw std::runtime_error("next tile for 2048 is undefined");
  if (tile == Tiles::kNoTile)
    return Tiles::kNoTile;
  int32_t tile_number = static_cast<int32_t>(tile);
  return static_cast<Tiles>(tile_number + 1);
}

void Logic::NewTile() {
  if (!free_) {
    game_over_ = true;
    return;
  }
  int32_t number = rand() % free_, count = -1;
  for (int32_t i = 0; i < length_; i++)
    for (int32_t j = 0; j < length_; j++) {
      if (tile_matrix_[i][j].value == Tiles::kNoTile)
        count++;
      if (count == number) {
        tile_matrix_[i][j] = TileInfo(kInitialTile, TileStates::kArising,
        Directions::kNone, i , j);
        free_--;
        return;
      }
    }
}

void Logic::MergeLeft(int32_t row_idx) {
  std::vector<TileInfo>& row = tile_matrix_[row_idx];
  for (int32_t from = 1, to = 0; from < length_; from++) {
    if (row[from].value == Tiles::kNoTile)
      continue;
    if (row[to].value == Tiles::kNoTile || row[to].value != row[from].value) {
      to = from;
      continue;
    }
    Tiles result = GetNextTile(row[to].value);
    if (result == Tiles::kTile_2048)
      game_over_ = success_ = true;
    row[to] = TileInfo(result, TileStates::kMerging,
                       Directions::kLeft, from, to);
    row[from] = TileInfo(Tiles::kNoTile);
    free_++;
    to = ++from;
  }
}

void Logic::ShiftLeft(int32_t row_idx) {
  std::vector<TileInfo>& row = tile_matrix_[row_idx];
  int32_t from = 1, to = 0;
  while (from < length_) {
    if (row[from].value != Tiles::kNoTile) {
      if (row[to].value == Tiles::kNoTile) {
        if (row[from].state == TileStates::kMerging) {
          row[to] = row[from];
        } else {
          row[to] = TileInfo(row[from].value, TileStates::kMoving,
                             Directions::kLeft, from);
        }
        row[from] = TileInfo(Tiles::kNoTile);
      }
      if (to < from - 1) {
        to++;
        continue;
      }
    }
    from++;
  }
}

void Logic::MoveLeft() {
  for (size_t i = 0; i < tile_matrix_.size(); i++) {
    MergeLeft(i);
    ShiftLeft(i);
  }
}

void Logic::MergeRight(int32_t row_idx) {
  std::vector<TileInfo>& row = tile_matrix_[row_idx];
  for (int32_t from = length_ - 2, to = length_ - 1; from >= 0; from--) {
    if (row[from].value == Tiles::kNoTile)
      continue;
    if (row[to].value == Tiles::kNoTile || row[to].value != row[from].value) {
      to = from;
      continue;
    }
    Tiles result = GetNextTile(row[to].value);
    if (result == Tiles::kTile_2048)
      game_over_ = success_ = true;
    row[to] = TileInfo(result, TileStates::kMerging,
                       Directions::kRight, from, to);
    row[from] = TileInfo(Tiles::kNoTile);
    free_++;
    to = --from;
  }
}

void Logic::ShiftRight(int32_t row_idx) {
  std::vector<TileInfo>& row = tile_matrix_[row_idx];
  int32_t from = length_ - 2, to = length_ - 1;
  while (from >= 0) {
    if (row[from].value != Tiles::kNoTile) {
      if (row[to].value == Tiles::kNoTile) {
        row[to] = row[from];
        if (row[from].state == TileStates::kMerging) {
          row[to] = row[from];
        } else {
          row[to] = TileInfo(row[from].value, TileStates::kMoving,
                             Directions::kRight, from);
        }
        row[from] = TileInfo(Tiles::kNoTile);
      }
      if (to > from + 1) {
        to--;
        continue;
      }
    }
    from--;
  }
}

void Logic::MoveRight() {
  for (size_t i = 0; i < tile_matrix_.size(); i++) {
    MergeRight(i);
    ShiftRight(i);
  }
}

void Logic::Transpose() {
  for (int32_t i = 0; i < length_; i++) {
    for (int32_t j = i + 1; j < length_; j++) {
      TileInfo& tile_i_j = tile_matrix_[i][j];
      TileInfo& tile_j_i = tile_matrix_[j][i];
      if (tile_i_j.value == Tiles::kNoTile && tile_j_i.value == Tiles::kNoTile)
        continue;
      std::swap(tile_i_j, tile_j_i);
    }
  }
}

void Logic::ChangeDirections() {
  for (auto& row : tile_matrix_) {
    for (auto& tile : row) {
      switch (tile.direction) {
        case Directions::kLeft:
          tile.direction = Directions::kUp;
          break;
        case Directions::kRight:
          tile.direction = Directions::kDown;
          break;
        default:
          break;
      }
    }
  }
}

void Logic::MoveUp() {
  Transpose();
  MoveLeft();
  Transpose();
  ChangeDirections();
}

void Logic::MoveDown() {
  Transpose();
  MoveRight();
  Transpose();
  ChangeDirections();
}

void Logic::ResetStates() {
  for (int32_t i = 0; i < length_; i++) {
    for (int32_t j = 0; j < length_; j++) {
      TileInfo& tile = tile_matrix_[i][j];
      tile.state = TileStates::kDefault;
      tile.direction = Directions::kNone;
      tile.source_1 = tile.source_2 = 0;
    }
  }
}
