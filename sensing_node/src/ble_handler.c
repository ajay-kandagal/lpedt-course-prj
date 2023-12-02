/*
 * ble_handler.c
 *
 *  Created on: 21-Nov-2023
 *      Author: ajayk
 */
#include "em_common.h"
#include "app_assert.h"
#include "gatt_db.h"
#include "app_log.h"

#include "ble_handler.h"

static ble_data_t ble_data;

void ble_handler_init()
{
  memset(&ble_data, 0, sizeof(ble_data));
}

ble_data_t* ble_handler_get_inst()
{
  return &ble_data;
}

void ble_handler_evt_proc(sl_bt_msg_t *evt)
{
  sl_status_t sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Extract unique ID from BT Address.
      sc = sl_bt_system_get_identity_address(&ble_data.address, &ble_data.address_type);
      app_assert_status(sc);

      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&ble_data.advertising_handle);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
          ble_data.advertising_handle,
          160, // min. adv. interval (milliseconds * 1.6)
          160, // max. adv. interval (milliseconds * 1.6)
          0,   // adv. duration
          0);  // max. num. adv. events
      app_assert_status(sc);

      // Start general advertising and enable connections.
      sc = sl_bt_advertiser_start(
          ble_data.advertising_handle,
          sl_bt_advertiser_general_discoverable,
          sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      break;

      // -------------------------------
      // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      app_log_info("Connection opened.\n");
      ble_data.connection_handle = evt->data.evt_connection_opened.connection;
      break;

      // -------------------------------
      // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      app_log_info("Connection closed.\n");
      memset(&ble_data, 0, sizeof(ble_data));

      // Restart advertising after client has disconnected.
      sc = sl_bt_advertiser_start(
          ble_data.advertising_handle,
          sl_bt_advertiser_general_discoverable,
          sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      break;

      // -------------------------------
      // This event indicates that the value of an attribute in the local GATT
      // database was changed by a remote GATT client.
    case sl_bt_evt_gatt_server_attribute_value_id:
      // The value of the gattdb_led_control characteristic was changed.
      if (gattdb_relay_control == evt->data.evt_gatt_server_characteristic_status.characteristic) {
          uint8_t data_recv;
          size_t data_recv_len;

          // Read characteristic value.
          sc = sl_bt_gatt_server_read_attribute_value(gattdb_relay_control,
                                                      0,
                                                      sizeof(data_recv),
                                                      &data_recv_len,
                                                      &data_recv);

          if (sc != SL_STATUS_OK) {
              app_log_error("\n\rError reading attribute value");
              break;
          }

          switch(data_recv)
          {
            case BT_CODE_AUTO_ON:
              sl_bt_external_signal(EVT_AUTO_ON);
              break;
            case BT_CODE_AUTO_OFF:
              sl_bt_external_signal(EVT_AUTO_OFF);
              break;
            default:
              app_log_error("Invalid attribute value: 0x%02x\n", (int)data_recv);
              break;
          }
      }
      break;

      // -------------------------------
      // This event occurs when the remote device enabled or disabled the
      // notification.
    case sl_bt_evt_gatt_server_characteristic_status_id:
      if (gattdb_relay_control == evt->data.evt_gatt_server_characteristic_status.characteristic) {
          // A local Client Characteristic Configuration descriptor was changed in
          // the gattdb_report_button characteristic.
          if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_indication) {
              // The client just enabled the notification. Send notification of the
              // current button state stored in the local GATT table.
              ble_data.indication_en = 1;

              app_log_status_error(sc);
          } else if (evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_disable){
              ble_data.indication_en = 0;
          }
      }
      if(sl_bt_gatt_server_confirmation == evt->data.evt_gatt_server_characteristic_status.status_flags) {
          ble_data.indication_in_flight = false;
      }
      break;

    case sl_bt_evt_gatt_server_indication_timeout_id:
      ble_data.indication_in_flight = false;
      break;

    default:
      break;
  }
}
