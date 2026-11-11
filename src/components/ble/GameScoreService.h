#pragma once
#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#undef max
#undef min
#include <cstdint>

namespace Pinetime {
  namespace Controllers {
    class NimbleController;
    class Settings;

    class GameScoreService {
    public:
      GameScoreService(NimbleController& nimble, Settings& settings);
      void Init();
      int OnGameScoreRequested(uint16_t attributeHandle, ble_gatt_access_ctxt* context);
      void NotifyHighScore(uint32_t score);

      // 00060000-78fc-48fe-8e23-433b3a1942d0
      static constexpr ble_uuid128_t serviceUuid {
        .u = {.type = BLE_UUID_TYPE_128},
        .value = {0xd0, 0x42, 0x19, 0x3a, 0x3b, 0x43, 0x23, 0x8e, 0xfe, 0x48, 0xfc, 0x78, 0x00, 0x00, 0x06, 0x00}};

      // 00060001-78fc-48fe-8e23-433b3a1942d0, uint32 little-endian high score
      static constexpr ble_uuid128_t highScoreUuid {
        .u = {.type = BLE_UUID_TYPE_128},
        .value = {0xd0, 0x42, 0x19, 0x3a, 0x3b, 0x43, 0x23, 0x8e, 0xfe, 0x48, 0xfc, 0x78, 0x01, 0x00, 0x06, 0x00}};

    private:
      NimbleController& nimble;
      Settings& settings;

      struct ble_gatt_chr_def characteristicDefinition[2];
      struct ble_gatt_svc_def serviceDefinition[2];
      uint16_t highScoreHandle;
    };
  }
}
