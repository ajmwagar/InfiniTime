#pragma once

#include <lvgl/src/lv_core/lv_obj.h>

#include "components/battery/BatteryController.h"
#include "components/datetime/DateTimeController.h"
#include "components/motion/MotionController.h"
#include "displayapp/Controllers.h"
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class WatchFaceSplit : public Screen {
      public:
        WatchFaceSplit(Controllers::DateTime& dateTimeController,
                       const Controllers::Battery& batteryController,
                       Controllers::MotionController& motionController);
        ~WatchFaceSplit() override;

      private:
        void Refresh() override;

        Controllers::DateTime& dateTimeController;
        const Controllers::Battery& batteryController;
        Controllers::MotionController& motionController;
        lv_obj_t* labelDate = nullptr;
        lv_obj_t* labelTime = nullptr;
        lv_obj_t* labelSteps = nullptr;
        lv_obj_t* labelBattery = nullptr;
        lv_task_t* taskRefresh = nullptr;
        uint8_t lastMinute = 0xFF;
        uint32_t lastSteps = 0xFFFFFFFF;
        uint8_t lastBattery = 0xFF;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Split> {
      static constexpr WatchFace watchFace = WatchFace::Split;
      static constexpr const char* name = "Split";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::WatchFaceSplit(
          controllers.dateTimeController, controllers.batteryController, controllers.motionController);
      }

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
