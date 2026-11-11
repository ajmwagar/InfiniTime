#include "displayapp/screens/WatchFaceSector.h"

#include <cmath>
#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

namespace {
  constexpr float Pi = 3.14159265f;
  constexpr int Center = 120;

  lv_obj_t* hand(lv_color_t color, lv_coord_t width) {
    auto* line = lv_line_create(lv_scr_act(), nullptr);
    lv_obj_set_style_local_line_color(line, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, color);
    lv_obj_set_style_local_line_width(line, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, width);
    lv_obj_set_style_local_line_rounded(line, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, true);
    return line;
  }

  void pointAt(lv_point_t* points, float degrees, int length) {
    const float radians = (degrees - 90.0f) * Pi / 180.0f;
    points[0] = {Center, Center};
    points[1] = {static_cast<lv_coord_t>(Center + std::cos(radians) * length),
                 static_cast<lv_coord_t>(Center + std::sin(radians) * length)};
  }
}

WatchFaceSector::WatchFaceSector(Controllers::DateTime& dateTimeController) : dateTimeController {dateTimeController} {
  for (int i = 0; i < 12; i++) {
    const bool cardinal = (i % 3) == 0;
    auto* tick = hand(cardinal ? LV_COLOR_WHITE : lv_color_hex(0x4A4A4A), cardinal ? 3 : 1);
    const float radians = (i * 30.0f - 90.0f) * Pi / 180.0f;
    const int inner = cardinal ? 96 : 103;
    tickPoints[i][0] = {static_cast<lv_coord_t>(Center + std::cos(radians) * inner),
                        static_cast<lv_coord_t>(Center + std::sin(radians) * inner)};
    tickPoints[i][1] = {static_cast<lv_coord_t>(Center + std::cos(radians) * 110),
                        static_cast<lv_coord_t>(Center + std::sin(radians) * 110)};
    lv_line_set_points(tick, tickPoints[i], 2);
  }

  hourHand = hand(LV_COLOR_WHITE, 5);
  minuteHand = hand(LV_COLOR_WHITE, 3);
  secondHand = hand(lv_color_hex(0xFF6A00), 2);

  auto* dot = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(dot, 10, 10);
  lv_obj_set_style_local_radius(dot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_style_local_bg_color(dot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_bg_opa(dot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
  lv_obj_set_style_local_border_width(dot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_align(dot, nullptr, LV_ALIGN_CENTER, 0, 0);

  auto* ring = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(ring, 226, 226);
  lv_obj_set_style_local_radius(ring, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_style_local_bg_opa(ring, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_border_width(ring, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
  lv_obj_set_style_local_border_color(ring, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2A2A2A));
  lv_obj_align(ring, nullptr, LV_ALIGN_CENTER, 0, 0);

  labelDate = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelDate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelDate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8A8A8A));
  lv_label_set_text_static(labelDate, "1");
  lv_obj_align(labelDate, nullptr, LV_ALIGN_CENTER, 0, 58);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceSector::~WatchFaceSector() {
  lv_task_del(taskRefresh);
}

void WatchFaceSector::Refresh() {
  const uint8_t second = dateTimeController.Seconds();
  if (second == lastSecond) {
    return;
  }
  lastSecond = second;

  const float minute = dateTimeController.Minutes() + second / 60.0f;
  const float hour = (dateTimeController.Hours() % 12) + minute / 60.0f;

  pointAt(hourPoints, hour * 30.0f, 52);
  pointAt(minutePoints, minute * 6.0f, 80);
  pointAt(secondPoints, second * 6.0f, 102);
  lv_line_set_points(hourHand, hourPoints, 2);
  lv_line_set_points(minuteHand, minutePoints, 2);
  lv_line_set_points(secondHand, secondPoints, 2);

  lv_label_set_text_fmt(labelDate, "%d", dateTimeController.Day());
  lv_obj_realign(labelDate);
}
