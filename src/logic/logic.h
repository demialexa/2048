#ifndef _2048_LOGIC_LOGIC_H_
#define _2048_LOGIC_LOGIC_H_

#include "display/display.h"

#include <cstdint>
#include <vector>

enum class TileStates {
  kDefault,
  kMoving,
  kMerging,
  kArising,
  kDying,
};

enum class Directions {
  kLeft,
  kRight,
  kUp,
  kDown,
  kNone,
};

class Logic {
 public:
  struct TileInfo {
    TileInfo(Tiles a_value, TileStates a_state = TileStates::kDefault,
             Directions a_direction = Directions::kNone,
             int32_t a_source_1 = 0, int32_t a_source_2 = 0)
        : value(a_value)
        , state(a_state)
        , direction(a_direction)
        , source_1(a_source_1)
        , source_2(a_source_2) {}

    Tiles value;
    TileStates state;
    Directions direction;
    int32_t source_1;
    int32_t source_2;
  };

  static constexpr Tiles kInitialTile = Tiles::kTile_2;
  static constexpr size_t kInitialTilesNumber = 2;

  Logic(int32_t length);

  std::vector<std::vector<TileInfo>> GetMatrix() const {
    return tile_matrix_;
  }

  bool IsGameOver() const {
    return game_over_;
  }

  int32_t IsSuccess() const {
    return success_;
  }

  void NewTile();
  void MoveLeft();
  void MoveRight();
  void MoveUp();
  void MoveDown();
  void ResetStates();
  bool HasSomethingChanged() const;

 private:
  void Transpose();
  void ChangeDirections();
  void MergeLeft(int32_t row_idx);
  void ShiftLeft(int32_t row_idx);
  void MergeRight(int32_t row_idx);
  void ShiftRight(int32_t row_idx);

  int32_t length_;
  std::vector<std::vector<TileInfo>> tile_matrix_;
  int32_t free_;
  bool game_over_;
  bool success_;
};

#endif
