#pragma once

#include <lvgl/src/lv_core/lv_obj.h>

#include "components/datetime/DateTimeController.h"
#include "displayapp/Controllers.h"
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class WatchFaceJarvis : public Screen {
      public:
        explicit WatchFaceJarvis(Controllers::DateTime& dateTimeController);
        ~WatchFaceJarvis() override;

      private:
        void Refresh() override;

        Controllers::DateTime& dateTimeController;
        lv_obj_t* labelTime = nullptr;
        lv_obj_t* labelDate = nullptr;
        lv_task_t* taskRefresh = nullptr;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Jarvis> {
      static constexpr WatchFace watchFace = WatchFace::Jarvis;
      static constexpr const char* name = "Jarvis";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::WatchFaceJarvis(controllers.dateTimeController);
      }

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
