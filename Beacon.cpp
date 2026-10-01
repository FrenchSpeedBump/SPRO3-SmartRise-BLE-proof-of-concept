#include "Beacon.hpp"
#include <pico/cyw43_arch.h> //CYW43 is the wireless chip of our picoW so its needed to access wireless functions
#include "pico/btstack_cyw43.h"//guidecode between btstack and our cyw43chip
#include "btstack.h" //main bt stack header - core bt functionality
#include <cstdint>//for uint8_t


Beacon::Beacon()
{
}

static btstack_packet_callback_registration_t hci_event_callback_registration; // btstack strucutre- stores which function should be called when bt events happen(packet_handler)

static void packet_handler(uint8_t packetType, uint16_t channel, uint8_t *packet, uint16_t size)//btstacks calls it wheneven bt events occue
//type of packet received, bt channel information, actual bt packet, packet size in bytes
{
    if (packetType != HCI_EVENT_PACKET)//ignore non events
        return;
    switch (hci_event_packet_get_type(packet))//reads event types
    {
    case BTSTACK_EVENT_STATE://bt state changed
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING)//check if bt is ready
        {
            return;
        }
        printf("Bluetooth ready\n");
        bd_addr_t null_addr;//storage for bt address
        memset(null_addr, 0, 6);//fill 6 bytes(bt address size) with 0

        gap_advertisements_set_params( // how often and how we advertise
            0x0030,                    // minimum interval ~ 0.30ms
            0x0030,                    // maximum interval => very frequent
            0x03,                         // Advertising type - 0=Connectable undirected advertising 0x03=
            0,                         // Address Type - 0 - Use public address.
            null_addr,                 // Target address - broadcast to everybody(no specific target)
            0x07,                      // Channel map(0b111) - use all 3 advertising channels
            0x00                       // Filter Policy - Accept everyone
        );
        
        //simple ble
        static const uint8_t adv_data2[] =//packet the phone sees
        {
            0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06, // 2 bytes follow 1 byte type 1 byte value , Flags field, 0x06 ~ Generally discoverable and BLE only

            0x0A, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, // device name 1 for type 9 for name
            'S', 'm', 'a', 'r', 't', 'R', 'i', 's', 'e'
        };

        //ibeacon
        static const uint8_t adv_data[] =
        {
            // Flags
            0x02,BLUETOOTH_DATA_TYPE_FLAGS,0x06,

            // Manufacturer Specific Data
            0x1A,BLUETOOTH_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA,

            // Apple Company ID (0x004C)
            0x4C, 0x00,

            // iBeacon Type
            0x02,
            0x15,

            // UUID
            0x12,0x34,0x56,0x78,
            0x12,0x34,
            0x56,0x78,
            0x12,0x34,
            0x56,0x78,0x9A,0xBC,0xDE,0xF0,

            // Major
            0x00,0x01,

            // Minor
            0x00,0x01,

            // Measured Power
            0xC5
        };
        gap_advertisements_set_data(sizeof(adv_data), (uint8_t *)adv_data); // supplying advertisment data ~ this is what we advertise

        gap_advertisements_enable(1); // enable advertising

        printf("Advertising as SmartRise\n");

        break;
    }
}

void Beacon::startAdvertising()
{
    if (cyw43_arch_init()) // initialize the wireless chip 0 success 1 failiue if(1) failed
    {
        printf("CYW43 init failed\n");
        return;
    }
    l2cap_init(); // Logical Link Control and Adaptation Protocol - BT transport layer
    sm_init();    // security manager - for bonding,pairing and encryption - no pairing but bt stack expects it

    hci_event_callback_registration.callback=&packet_handler;//when bt events happen call packet_handler

    hci_add_event_handler(&hci_event_callback_registration);//registers callback with BTstack

    hci_power_control(HCI_POWER_ON);//literally turns on BT
}