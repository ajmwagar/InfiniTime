#pragma once

#include <lvgl/src/lv_core/lv_obj.h>

#include "components/datetime/DateTimeController.h"
#include "displayapp/Controllers.h"
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class WatchFaceMeridian : public Screen {
      public:
        explicit WatchFaceMeridian(Controllers::DateTime& dateTimeController);
        ~WatchFaceMeridian() override;

      private:
        void Refresh() override;

        Controllers::DateTime& dateTimeController;
        lv_obj_t* arcMinutes = nullptr;
        lv_obj_t* arcHours = nullptr;
        lv_obj_t* labelTime = nullptr;
        lv_obj_t* labelDate = nullptr;
        lv_task_t* taskRefresh = nullptr;
        uint8_t lastMinute = 0xFF;
        uint8_t lastSecond = 0xFF;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Meridian> {
      static constexpr WatchFace watchFace = WatchFace::Meridian;
      static constexpr const char* name = "Meridian";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::WatchFaceMeridian(controllers.dateTimeController);
      }

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
