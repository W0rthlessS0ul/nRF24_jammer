## 🌟 nRF24 jammer 🌟
Welcome to the **nRF24** Jammer repository! 🎉

## 📚 Table of Contents
- [🚀 What Can You Do with This?](#-what-can-you-do-with-this)
- [📋 list of components.](#-list-of-components)
- [🧑‍🔧 Let's Get Started with Soldering!](#-Let's-Get-Started-with-Soldering)
- [📦 Flash Firmware](#-Flash-Firmware)
- [🎉 Final Outcome](#-Final-Outcome)
- [❤️ Thank You for Your Support!](#-thank-you-for-your-support)
- [🌐 Follow Me for Updates](#-follow-me-for-updates)

## 🚀 What Can You Do with This?
This jammer is based on the ESP32 with 30 pins and two NRF24 modules. With its capabilities, you can effectively interfere with Bluetooth, BLE, drones, and Wi-Fi signals.

## 📋 list of components.
To create this project, you will need: 
1. two NRF24L01+PA+LNA modules
2. ESP32 30 pins
3. two 16V capacitors rated at 100µF
4. 128x32 display
5. tactile button

## ‍🧑‍🔧 Let's Get Started with Soldering!

### HSPI Connection
| Pin Name | ESP32 GPIO | Connection   |
|----------|------------|--------------|
| VCC      | 3.3V      | (+) capacitor |
| GND      | GND       | (-) capacitor |
| CE       | GPIO 16   |              |
| CSN      | GPIO 15   |              |
| SCK      | GPIO 14   |              |
| MOSI     | GPIO 13   |              |
| MISO     | GPIO 12   |              |
| IRQ      |            |              |

### VSPI Connection
| Pin Name | ESP32 GPIO | Connection   |
|----------|------------|--------------|
| VCC      | 3.3V      | (+) capacitor |
| GND      | GND       | (-) capacitor |
| CE       | GPIO 22   |              |
| CSN      | GPIO 21   |              |
| SCK      | GPIO 18   |              |
| MOSI     | GPIO 23   |              |
| MISO     | GPIO 19   |              |
| IRQ      |            |              |

### OLED Connection
| Pin Name | ESP32 GPIO |
|----------|------------|
| VCC      | 3.3V      |
| GND      | GND       |
| SCL      | GPIO 22   |
| SDA      | GPIO 21   |

### Button Connection
| Pin Name | ESP32 GPIO |
|----------|------------|
| GND      | GND       |
|          | GPIO 25   |

## 📦 Flash Firmware
1. Download the files from the 'bin' folder in the repository or from the releases section.
2. Transition to ESPWebTool. https://esp.huhn.me/
3. To arrange the files in accordance with the table below.

| Address | File Name                               |
|---------|-----------------------------------------|
| 0x1000  | nRF24_jammer_bootloader.bin            |
| 0x8000  | nRF24_jammer_partitions.bin            |
| 0x10000 | nRF24_jammer.bin                        |

4. Click on "Flash," and once the installation is complete, restart your board and enjoy the experience!

## 🎉 Final Outcome
![final outcome](img/nRF24_jammer.jpg)

### Normal spectrum
![Normal spctr](img/normal_spctr.jpg)

### Bluetooth jam spectrum
![bluetooth_jam_spctr](img/bluetooth_jam_spctr.jpg)

### Drone jam spectrum
![drone_jam_spctr](img/drone_jam_spctr.jpg)

### Wifi jam spectrum
![wifi_jam_spctr](img/wifi_jam_spctr.jpg)

## ❤️ Thank You for Your Support!
If you would like to show your support, you can star my repository or follow me! If you appreciate this project, consider buying me a cup of coffee to keep the energy flowing! ☕ Thank you for your support!

**BTC Address:** `3PpBpNs1wXkhA93g5tfpMf4qierz4m9BCW` (this is a **SegWit** address)

Every donation you make is deeply appreciated and will help in the ongoing development of this project!

---

### 🌐 Follow Me for Updates
Stay connected and receive the latest updates:

[![GitHub](https://img.shields.io/badge/GitHub-W0rthlessS0ul-181717?style=flat&logo=github&logoColor=white)](https://github.com/W0rthlessS0ul)
![Visitors](https://api.visitorbadge.io/api/visitors?path=https%3A%2F%2Fgithub.com%2FW0rthlessS0ul%2FnRF24_jammer&countColor=%232ccce4&style=flat-square)
