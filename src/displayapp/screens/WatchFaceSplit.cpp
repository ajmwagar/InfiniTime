#include "displayapp/screens/WatchFaceSplit.h"

#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

WatchFaceSplit::WatchFaceSplit(Controllers::DateTime& dateTimeController,
                               const Controllers::Battery& batteryController,
                               Controllers::MotionController& motionController)
  : dateTimeController {dateTimeController}, batteryController {batteryController}, motionController {motionController} {
  labelDate = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelDate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelDate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8A8A8A));
  lv_label_set_text_static(labelDate, "");
  lv_obj_align(labelDate, nullptr, LV_ALIGN_IN_TOP_MID, 0, 22);

  labelTime = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_76);
  lv_obj_set_style_local_text_color(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_label_set_text_static(labelTime, "--:--");
  lv_obj_align(labelTime, nullptr, LV_ALIGN_CENTER, 0, -6);

  auto* rule = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(rule, 180, 2);
  lv_obj_set_style_local_bg_color(rule, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2ED3FF));
  lv_obj_set_style_local_bg_opa(rule, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
  lv_obj_set_style_local_border_width(rule, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_align(rule, nullptr, LV_ALIGN_CENTER, 0, 56);

  labelSteps = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelSteps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelSteps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8A8A8A));
  lv_label_set_text_static(labelSteps, "");
  lv_obj_align(labelSteps, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, 30, -24);

  labelBattery = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelBattery, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelBattery, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8A8A8A));
  lv_label_set_text_static(labelBattery, "");
  lv_obj_align(labelBattery, nullptr, LV_ALIGN_IN_BOTTOM_RIGHT, -30, -24);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceSplit::~WatchFaceSplit() {
  lv_task_del(taskRefresh);
}

void WatchFaceSplit::Refresh() {
  const uint8_t minute = dateTimeController.Minutes();
  const uint32_t steps = motionController.NbSteps();
  const auto battery = static_cast<uint8_t>(batteryController.PercentRemaining());
  if (minute == lastMinute && steps == lastSteps && battery == lastBattery) {
    return;
  }
  lastMinute = minute;
  lastSteps = steps;
  lastBattery = battery;

  lv_label_set_text_fmt(labelDate,
                        "%s %d %s",
                        dateTimeController.DayOfWeekShortToString(),
                        dateTimeController.Day(),
                        dateTimeController.MonthShortToString());
  lv_label_set_text_fmt(labelTime, "%02d:%02d", dateTimeController.Hours(), minute);
  lv_label_set_text_fmt(labelSteps, "STEPS %u", steps);
  lv_label_set_text_fmt(labelBattery, "%u%%", battery);
  lv_obj_realign(labelDate);
  lv_obj_realign(labelTime);
  lv_obj_realign(labelSteps);
  lv_obj_realign(labelBattery);
}
