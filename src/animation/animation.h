#ifndef _2048_ANIMATION_ANIMATION_H_
#define _2048_ANIMATION_ANIMATION_H_

#include "logic/logic.h"

class Animation {
 public:
  struct TileInfo {
    TileInfo(Tiles a_value, TileStates a_state, int32_t a_dst_row,
             int32_t a_dst_column, double a_current, double a_speed,
             double a_opacity, Directions a_direction) 
      : value(a_value)
      , state(a_state)
      , dst_row(a_dst_row)
      , dst_column(a_dst_column)
      , current(a_current)
      , speed(a_speed)
      , opacity(a_opacity) 
      , direction(a_direction) {}

    Tiles value;
    TileStates state;
    int32_t dst_row, dst_column;
    double current, speed, opacity;
    Directions direction;
  };

  static constexpr double kArisingRate = 2;
  static constexpr double kBaseMovingRate = 2;

  Animation() {}

  Animation(const std::vector<std::vector<Logic::TileInfo>>& logic_matrix);

  const std::vector<Animation::TileInfo>& GetAnimationVector() {
    return tiles_to_draw_;
  }

  void UpdateMoving();

  void Merge();

  void UpdateArising();

  bool IsArisingFinished() const;

  bool IsMovingFinished() const;

 private:
  double GetSpeed(const Logic::TileInfo& tile,
                  int32_t dst_row, int32_t dst_column,
                  bool is_second_speed = false) const;
                  
  Tiles GetValue(const Logic::TileInfo& logic_tile) const ; 


  std::vector<Animation::TileInfo> tiles_to_draw_;
  int32_t old_time_;
  int32_t elapsed_time_;
};

#endif
