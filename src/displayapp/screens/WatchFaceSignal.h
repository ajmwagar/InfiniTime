#pragma once

#include <lvgl/src/lv_core/lv_obj.h>

#include "components/battery/BatteryController.h"
#include "components/ble/SimpleWeatherService.h"
#include "components/datetime/DateTimeController.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "displayapp/Controllers.h"
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class WatchFaceSignal : public Screen {
      public:
        WatchFaceSignal(Controllers::DateTime& dateTimeController,
                        const Controllers::Battery& batteryController,
                        Controllers::MotionController& motionController,
                        Controllers::HeartRateController& heartRateController,
                        Controllers::SimpleWeatherService* weatherService);
        ~WatchFaceSignal() override;

      private:
        void Refresh() override;

        Controllers::DateTime& dateTimeController;
        const Controllers::Battery& batteryController;
        Controllers::MotionController& motionController;
        Controllers::HeartRateController& heartRateController;
        Controllers::SimpleWeatherService* weatherService;

        lv_obj_t* labelTime = nullptr;
        lv_obj_t* labelSteps = nullptr;
        lv_obj_t* labelStepsCaption = nullptr;
        lv_obj_t* labelHeart = nullptr;
        lv_obj_t* labelHeartCaption = nullptr;
        lv_obj_t* labelBattery = nullptr;
        lv_obj_t* labelBatteryCaption = nullptr;
        lv_obj_t* labelTemperature = nullptr;
        lv_obj_t* labelTemperatureCaption = nullptr;
        lv_task_t* taskRefresh = nullptr;
        uint8_t lastMinute = 0xFF;
        uint32_t lastSteps = 0xFFFFFFFF;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Signal> {
      static constexpr WatchFace watchFace = WatchFace::Signal;
      static constexpr const char* name = "Signal";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::WatchFaceSignal(controllers.dateTimeController,
                                            controllers.batteryController,
                                            controllers.motionController,
                                            controllers.heartRateController,
                                            controllers.weatherController);
      }

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
