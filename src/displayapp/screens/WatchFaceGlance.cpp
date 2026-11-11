#include "displayapp/screens/WatchFaceGlance.h"

#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

namespace {
  constexpr lv_color_t Accent = LV_COLOR_MAKE(0x2E, 0xD3, 0xFF);
  constexpr lv_color_t Muted = LV_COLOR_MAKE(0x8A, 0x8A, 0x8A);
  constexpr lv_color_t Dim = LV_COLOR_MAKE(0x3A, 0x3A, 0x3A);
}

WatchFaceGlance::WatchFaceGlance(Controllers::DateTime& dateTimeController,
                                 Controllers::NotificationManager& notificationManager,
                                 Controllers::AlarmController& alarmController,
                                 const Controllers::Battery& batteryController,
                                 const Controllers::Ble& bleController,
                                 Controllers::MotionController& motionController,
                                 Controllers::HeartRateController& heartRateController)
  : dateTimeController {dateTimeController},
    notificationManager {notificationManager},
    alarmController {alarmController},
    batteryController {batteryController},
    bleController {bleController},
    motionController {motionController},
    heartRateController {heartRateController} {
  labelTime = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Muted);
  lv_label_set_text_static(labelTime, "--:--");
  lv_obj_align(labelTime, nullptr, LV_ALIGN_IN_TOP_LEFT, 16, 16);

  labelBattery = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelBattery, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelBattery, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Muted);
  lv_label_set_text_static(labelBattery, "");
  lv_obj_align(labelBattery, nullptr, LV_ALIGN_IN_TOP_RIGHT, -20, 16);

  linkDot = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(linkDot, 8, 8);
  lv_obj_set_style_local_radius(linkDot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_style_local_bg_opa(linkDot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
  lv_obj_set_style_local_border_width(linkDot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_align(linkDot, nullptr, LV_ALIGN_IN_TOP_RIGHT, -6, 21);

  labelKicker = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelKicker, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelKicker, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Accent);
  lv_label_set_text_static(labelKicker, "");
  lv_obj_align(labelKicker, nullptr, LV_ALIGN_CENTER, 0, -44);

  labelMessage = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelMessage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelMessage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_label_set_long_mode(labelMessage, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(labelMessage, 200);
  lv_label_set_align(labelMessage, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text_static(labelMessage, "");
  lv_obj_realign(labelMessage);

  labelSteps = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelSteps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelSteps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Muted);
  lv_label_set_text_static(labelSteps, "");
  lv_obj_align(labelSteps, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, 16, -18);

  labelHeart = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelHeart, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelHeart, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Muted);
  lv_label_set_text_static(labelHeart, "");
  lv_obj_align(labelHeart, nullptr, LV_ALIGN_IN_BOTTOM_RIGHT, -16, -18);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceGlance::~WatchFaceGlance() {
  lv_task_del(taskRefresh);
}

void WatchFaceGlance::Refresh() {
  const uint8_t second = dateTimeController.Seconds();
  const uint8_t notificationId = notificationManager.GetLastNotification().id;
  if (second == lastSecond && notificationId == lastNotification) {
    return;
  }
  lastSecond = second;
  lastNotification = notificationId;

  lv_label_set_text_fmt(labelTime, "%02d:%02d", dateTimeController.Hours(), dateTimeController.Minutes());
  lv_label_set_text_fmt(labelBattery, "%u%%", batteryController.PercentRemaining());
  lv_obj_set_style_local_bg_color(linkDot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, bleController.IsConnected() ? Accent : Dim);
  lv_obj_realign(labelTime);
  lv_obj_realign(labelBattery);

  if (notificationManager.IsEmpty()) {
    lv_obj_set_style_local_text_color(labelKicker, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Muted);
    lv_label_set_text_static(labelKicker, "QUIET");
    if (alarmController.IsEnabled()) {
      lv_label_set_text_fmt(labelMessage, "ALARM %02d:%02d", alarmController.Hours(), alarmController.Minutes());
    } else {
      lv_label_set_text_static(labelMessage, "NO ALERTS");
    }
  } else {
    const auto notification = notificationManager.GetLastNotification();
    lv_obj_set_style_local_text_color(labelKicker, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Accent);
    lv_label_set_text_static(labelKicker, notificationManager.AreNewNotificationsAvailable() ? "NEW" : "LAST");
    const char* title = notification.Title();
    lv_label_set_text(labelMessage, (title != nullptr && title[0] != '\0') ? title : notification.Message());
  }

  const uint8_t heartRate = heartRateController.HeartRate();
  lv_label_set_text_fmt(labelSteps, "STEPS %u", motionController.NbSteps());
  if (heartRate == 0 || heartRateController.State() == Controllers::HeartRateController::States::Stopped) {
    lv_label_set_text_static(labelHeart, "HR --");
  } else {
    lv_label_set_text_fmt(labelHeart, "HR %u", heartRate);
  }
  lv_obj_realign(labelKicker);
  lv_obj_align(labelMessage, nullptr, LV_ALIGN_CENTER, 0, -6);
  lv_obj_realign(labelSteps);
  lv_obj_realign(labelHeart);
}
