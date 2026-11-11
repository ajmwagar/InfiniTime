#include "displayapp/screens/WatchFaceSignal.h"

#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

namespace {
  constexpr lv_color_t Accent = LV_COLOR_MAKE(0x2E, 0xD3, 0xFF);
  constexpr lv_color_t Muted = LV_COLOR_MAKE(0x8A, 0x8A, 0x8A);
  constexpr lv_color_t Battery = LV_COLOR_MAKE(0x6F, 0xE0, 0x8A);
  constexpr lv_color_t Heart = LV_COLOR_MAKE(0xFF, 0x5A, 0x5A);
  constexpr lv_color_t Temperature = LV_COLOR_MAKE(0xFF, 0xB0, 0x00);
  constexpr lv_color_t Amber = LV_COLOR_MAKE(0xFF, 0xB0, 0x00);

  lv_obj_t* value(lv_obj_t* parent) {
    auto* label = lv_label_create(parent, nullptr);
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);
    lv_label_set_text_static(label, "--");
    return label;
  }

  lv_obj_t* caption(lv_obj_t* parent) {
    auto* label = lv_label_create(parent, nullptr);
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Muted);
    return label;
  }
}

WatchFaceSignal::WatchFaceSignal(Controllers::DateTime& dateTimeController,
                                 const Controllers::Battery& batteryController,
                                 Controllers::MotionController& motionController,
                                 Controllers::HeartRateController& heartRateController,
                                 Controllers::SimpleWeatherService* weatherService)
  : dateTimeController {dateTimeController},
    batteryController {batteryController},
    motionController {motionController},
    heartRateController {heartRateController},
    weatherService {weatherService} {
  labelTime = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_set_style_local_text_color(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Muted);
  lv_label_set_text_static(labelTime, "--:--");
  lv_obj_align(labelTime, nullptr, LV_ALIGN_IN_TOP_MID, 0, 14);

  labelSteps = value(lv_scr_act());
  lv_obj_set_style_local_text_color(labelSteps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Accent);
  lv_obj_align(labelSteps, nullptr, LV_ALIGN_CENTER, -60, -28);
  labelStepsCaption = caption(lv_scr_act());
  lv_label_set_text_static(labelStepsCaption, "STEPS");
  lv_obj_align(labelStepsCaption, nullptr, LV_ALIGN_CENTER, -60, 12);

  labelHeart = value(lv_scr_act());
  lv_obj_set_style_local_text_color(labelHeart, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Heart);
  lv_obj_align(labelHeart, nullptr, LV_ALIGN_CENTER, 60, -28);
  labelHeartCaption = caption(lv_scr_act());
  lv_label_set_text_static(labelHeartCaption, "BPM");
  lv_obj_align(labelHeartCaption, nullptr, LV_ALIGN_CENTER, 60, 12);

  labelBattery = value(lv_scr_act());
  lv_obj_set_style_local_text_color(labelBattery, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Battery);
  lv_obj_align(labelBattery, nullptr, LV_ALIGN_CENTER, -60, 52);
  labelBatteryCaption = caption(lv_scr_act());
  lv_label_set_text_static(labelBatteryCaption, "BATT");
  lv_obj_align(labelBatteryCaption, nullptr, LV_ALIGN_CENTER, -60, 92);

  labelTemperature = value(lv_scr_act());
  lv_obj_set_style_local_text_color(labelTemperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Temperature);
  lv_obj_align(labelTemperature, nullptr, LV_ALIGN_CENTER, 60, 52);
  labelTemperatureCaption = caption(lv_scr_act());
  lv_label_set_text_static(labelTemperatureCaption, "TEMP");
  lv_obj_align(labelTemperatureCaption, nullptr, LV_ALIGN_CENTER, 60, 92);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceSignal::~WatchFaceSignal() {
  lv_task_del(taskRefresh);
}

void WatchFaceSignal::Refresh() {
  const uint8_t minute = dateTimeController.Minutes();
  const uint32_t steps = motionController.NbSteps();
  if (minute == lastMinute && steps == lastSteps) {
    return;
  }
  lastMinute = minute;
  lastSteps = steps;

  lv_label_set_text_fmt(labelTime, "%02d:%02d", dateTimeController.Hours(), minute);
  lv_label_set_text_fmt(labelSteps, "%u", steps);
  const uint8_t battery = batteryController.PercentRemaining();
  lv_label_set_text_fmt(labelBattery, "%u%%", battery);
  lv_obj_set_style_local_text_color(labelBattery,
                                    LV_LABEL_PART_MAIN,
                                    LV_STATE_DEFAULT,
                                    battery >= 40 ? Battery : (battery >= 15 ? Amber : Heart));

  const uint8_t heartRate = heartRateController.HeartRate();
  if (heartRate == 0 || heartRateController.State() == Controllers::HeartRateController::States::Stopped) {
    lv_label_set_text_static(labelHeart, "--");
  } else {
    lv_label_set_text_fmt(labelHeart, "%u", heartRate);
  }

  if (weatherService != nullptr && weatherService->Current().has_value()) {
    const auto weather = weatherService->Current().value();
    lv_label_set_text_fmt(labelTemperature, "%d°", weather.temperature.Celsius());
  } else {
    lv_label_set_text_static(labelTemperature, "--");
  }

  lv_obj_realign(labelTime);
  lv_obj_realign(labelSteps);
  lv_obj_realign(labelBattery);
  lv_obj_realign(labelHeart);
  lv_obj_realign(labelTemperature);
}
