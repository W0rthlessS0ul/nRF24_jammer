# OTG Jammer
A fork of W0rthlessS0ul's nRF24_jammer, with "on the go" usage in mind. This fork removes a few features that don't really serve a prupose as a portable device, and some that I think don't really add to the device itself.

## Features
- Configurable number of nRF21 modules (up to 30).
- Jamming:
  - Bluetooth
  - BLE
  - Wi-Fi
  - Zigbee
  - Drones
- Serial Comand interpreter. For when you don't have a screen and want to use it as external device.

## Possible Additions
- Deauthentication attack
- BLE spam
- Beacon spam
- nRF24 mousejack

***`If you have the capability to assist with these features, please feel free to create pull requests!`***

## List of Components (TBD)
To bring this project to life, you will need the following components:
1. nRF24L01+PA+LNA modules
2. ESP-WROOM-32
3. 16V capacitors rated at 100µF
4. Nokia 1100 (or PCF8814 based) LCD 
5. 3 Tactile buttons or 1 Thumbwheel Toggle Slide Dial Switch

## Flash Firmware

Follow these steps to flash the firmware:
1. Download the firmware from the **[releases](https://github.com/fpp3/otg_jammer/releases)** section.
2. Navigate to **ESPWebTool**: [ESPWebTool](https://esp.huhn.me/)
3. Arrange the files as per the table below:

| **Address** | **File Name**                             |
|-------------|-------------------------------------------|
| 0x1000      | nRF24_jammer_bootloader.bin               |
| 0x8000      | nRF24_jammer_partitions.bin               |
| 0x10000     | nRF24_jammer.bin                          |

4. Click "Flash," and once the installation is complete, **restart your board** and enjoy using your jammer!

## Acknowledgments
- [smoochiee](https://github.com/smoochiee) - **for the main logic of the jammer.**
- [samukas81](https://github.com/samukas81) - **for suggesting ideas.**
- [W0rthlessS0ul](https://github.com/w0rthlesss0ul) - **for the main project and idea**

