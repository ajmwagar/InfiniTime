#include "displayapp/screens/WatchFaceJarvis.h"

#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

WatchFaceJarvis::WatchFaceJarvis(Controllers::DateTime& dateTimeController)
  : dateTimeController {dateTimeController} {
  auto* title = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2ED3FF));
  lv_label_set_text_static(title, "J A R V I S");
  lv_obj_align(title, nullptr, LV_ALIGN_IN_TOP_MID, 0, 20);

  labelTime = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_extrabold_compressed);
  lv_label_set_text_static(labelTime, "--:--");
  lv_obj_align(labelTime, nullptr, LV_ALIGN_CENTER, 0, 0);

  labelDate = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(labelDate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8A8A8A));
  lv_label_set_text_static(labelDate, "");
  lv_obj_align(labelDate, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, -40);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceJarvis::~WatchFaceJarvis() {
  lv_task_del(taskRefresh);
}

void WatchFaceJarvis::Refresh() {
  lv_label_set_text_fmt(labelTime, "%02d:%02d", dateTimeController.Hours(), dateTimeController.Minutes());
  lv_label_set_text_fmt(labelDate,
                        "%s %d %s %d",
                        dateTimeController.DayOfWeekShortToString(),
                        dateTimeController.Day(),
                        dateTimeController.MonthShortToString(),
                        dateTimeController.Year());
  lv_obj_realign(labelTime);
  lv_obj_realign(labelDate);
}
