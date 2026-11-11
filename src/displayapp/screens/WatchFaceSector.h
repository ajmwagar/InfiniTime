#pragma once

#include <lvgl/src/lv_core/lv_obj.h>

#include "components/datetime/DateTimeController.h"
#include "displayapp/Controllers.h"
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class WatchFaceSector : public Screen {
      public:
        explicit WatchFaceSector(Controllers::DateTime& dateTimeController);
        ~WatchFaceSector() override;

      private:
        void Refresh() override;

        Controllers::DateTime& dateTimeController;
        lv_obj_t* labelDate = nullptr;
        lv_obj_t* hourHand = nullptr;
        lv_obj_t* minuteHand = nullptr;
        lv_obj_t* secondHand = nullptr;
        lv_point_t tickPoints[12][2];
        lv_point_t hourPoints[2];
        lv_point_t minutePoints[2];
        lv_point_t secondPoints[2];
        lv_task_t* taskRefresh = nullptr;
        uint8_t lastSecond = 0xFF;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Sector> {
      static constexpr WatchFace watchFace = WatchFace::Sector;
      static constexpr const char* name = "Sector";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::WatchFaceSector(controllers.dateTimeController);
      }

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
