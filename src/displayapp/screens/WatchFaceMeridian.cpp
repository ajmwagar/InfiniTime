#include "displayapp/screens/WatchFaceMeridian.h"

#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

namespace {
  lv_obj_t* gauge(lv_coord_t size, lv_coord_t width, lv_color_t track, lv_color_t indicator, int16_t range) {
    auto* arc = lv_arc_create(lv_scr_act(), nullptr);
    lv_obj_set_size(arc, size, size);
    lv_obj_align(arc, nullptr, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_bg_opa(arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_line_width(arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, width);
    lv_obj_set_style_local_line_color(arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, track);
    lv_obj_set_style_local_line_width(arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, width);
    lv_obj_set_style_local_line_color(arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, indicator);
    lv_arc_set_bg_angles(arc, 135, 45);
    lv_arc_set_range(arc, 0, range);
    return arc;
  }
}

WatchFaceMeridian::WatchFaceMeridian(Controllers::DateTime& dateTimeController)
  : dateTimeController {dateTimeController} {
  arcMinutes = gauge(216, 8, lv_color_hex(0x1C1C1C), lv_color_hex(0x2ED3FF), 3600);
  arcHours = gauge(174, 3, lv_color_hex(0x1C1C1C), lv_color_hex(0xFFB000), 720);

  labelTime = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);
  lv_obj_set_style_local_text_color(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_label_set_text_static(labelTime, "--:--");
  lv_obj_align(labelTime, nullptr, LV_ALIGN_CENTER, 0, -8);

  labelDate = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelDate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelDate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8A8A8A));
  lv_label_set_text_static(labelDate, "");
  lv_obj_align(labelDate, nullptr, LV_ALIGN_CENTER, 0, 36);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceMeridian::~WatchFaceMeridian() {
  lv_task_del(taskRefresh);
}

void WatchFaceMeridian::Refresh() {
  const uint8_t minute = dateTimeController.Minutes();
  const uint8_t second = dateTimeController.Seconds();
  if (minute == lastMinute && second == lastSecond) {
    return;
  }
  lastMinute = minute;
  lastSecond = second;

  lv_arc_set_value(arcMinutes, static_cast<int16_t>(minute * 60 + second));
  lv_arc_set_value(arcHours, static_cast<int16_t>((dateTimeController.Hours() % 12) * 60 + minute));

  lv_label_set_text_fmt(labelTime, "%02d:%02d", dateTimeController.Hours(), minute);
  lv_label_set_text_fmt(labelDate,
                        "%s %d %s",
                        dateTimeController.DayOfWeekShortToString(),
                        dateTimeController.Day(),
                        dateTimeController.MonthShortToString());
  lv_obj_realign(labelTime);
  lv_obj_realign(labelDate);
}
