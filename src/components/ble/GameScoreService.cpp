#include "components/ble/GameScoreService.h"
#include "components/ble/NimbleController.h"
#include "components/settings/Settings.h"
#include "main.h"
#include <nrf_log.h>

using namespace Pinetime::Controllers;

constexpr ble_uuid128_t GameScoreService::serviceUuid;
constexpr ble_uuid128_t GameScoreService::highScoreUuid;

namespace {
  int GameScoreCallback(uint16_t /*conn_handle*/, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
    auto* gameScoreService = static_cast<GameScoreService*>(arg);
    return gameScoreService->OnGameScoreRequested(attr_handle, ctxt);
  }
}

GameScoreService::GameScoreService(NimbleController& nimble, Settings& settings)
  : nimble {nimble},
    settings {settings},
    characteristicDefinition {{.uuid = &highScoreUuid.u,
                               .access_cb = GameScoreCallback,
                               .arg = this,
                               .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                               .val_handle = &highScoreHandle},
                              {0}},
    serviceDefinition {{.type = BLE_GATT_SVC_TYPE_PRIMARY,
                        .uuid = &serviceUuid.u,
                        .characteristics = characteristicDefinition},
                       {0}} {
}

void GameScoreService::Init() {
  int res = 0;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);
}

int GameScoreService::OnGameScoreRequested(uint16_t attributeHandle, ble_gatt_access_ctxt* context) {
  if (attributeHandle != highScoreHandle) {
    return 0;
  }

  const uint32_t score = settings.GetTwosHighScore();
  const uint8_t buffer[4] = {
    static_cast<uint8_t>(score & 0xff),
    static_cast<uint8_t>((score >> 8) & 0xff),
    static_cast<uint8_t>((score >> 16) & 0xff),
    static_cast<uint8_t>((score >> 24) & 0xff),
  };

  int res = os_mbuf_append(context->om, buffer, sizeof(buffer));
  return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

void GameScoreService::NotifyHighScore(uint32_t score) {
  const uint8_t buffer[4] = {
    static_cast<uint8_t>(score & 0xff),
    static_cast<uint8_t>((score >> 8) & 0xff),
    static_cast<uint8_t>((score >> 16) & 0xff),
    static_cast<uint8_t>((score >> 24) & 0xff),
  };

  auto* om = ble_hs_mbuf_from_flat(buffer, sizeof(buffer));
  if (om == nullptr) {
    return;
  }

  const uint16_t connectionHandle = nimble.connHandle();
  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
    return;
  }

  ble_gattc_notify_custom(connectionHandle, highScoreHandle, om);
}
