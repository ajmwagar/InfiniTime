#pragma once

#include <lvgl/src/lv_core/lv_obj.h>

#include "components/alarm/AlarmController.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/datetime/DateTimeController.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "displayapp/Controllers.h"
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class WatchFaceGlance : public Screen {
      public:
        WatchFaceGlance(Controllers::DateTime& dateTimeController,
                        Controllers::NotificationManager& notificationManager,
                        Controllers::AlarmController& alarmController,
                        const Controllers::Battery& batteryController,
                        const Controllers::Ble& bleController,
                        Controllers::MotionController& motionController,
                        Controllers::HeartRateController& heartRateController);
        ~WatchFaceGlance() override;

      private:
        void Refresh() override;

        Controllers::DateTime& dateTimeController;
        Controllers::NotificationManager& notificationManager;
        Controllers::AlarmController& alarmController;
        const Controllers::Battery& batteryController;
        const Controllers::Ble& bleController;
        Controllers::MotionController& motionController;
        Controllers::HeartRateController& heartRateController;

        lv_obj_t* labelTime = nullptr;
        lv_obj_t* labelBattery = nullptr;
        lv_obj_t* linkDot = nullptr;
        lv_obj_t* labelKicker = nullptr;
        lv_obj_t* labelMessage = nullptr;
        lv_obj_t* labelSteps = nullptr;
        lv_obj_t* labelHeart = nullptr;
        lv_task_t* taskRefresh = nullptr;
        uint8_t lastSecond = 0xFF;
        uint8_t lastNotification = 0xFF;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Glance> {
      static constexpr WatchFace watchFace = WatchFace::Glance;
      static constexpr const char* name = "Glance";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::WatchFaceGlance(controllers.dateTimeController,
                                            controllers.notificationManager,
                                            controllers.alarmController,
                                            controllers.batteryController,
                                            controllers.bleController,
                                            controllers.motionController,
                                            controllers.heartRateController);
      }

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
