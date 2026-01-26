<div align="center">
  <img src="https://avatars.githubusercontent.com/u/176677387" width="150" height="auto" />
  <h1> 🌟 nRF24 Jammer 🌟 </h1>
</div>

Welcome to the **nRF24 Jammer** repository! 🎉 Dive into the world of RF interference with this unique project based on the ESP32 and nRF24 technology.

## 📚 Table of Contents
- [🎯 Possible Additions](#-possible-additions)
- [🚀 What Can You Do with This?](#-what-can-you-do-with-this)
- [📋 List of Components](#-list-of-components)
- [🧑‍🔧 Let's Get Started with Soldering!](#-lets-get-started-with-soldering)
- [📦 Flash Firmware](#-flash-firmware)
- [🎮 Device Control](#-device-control)
- [🌐 Web Interface](#-Web-Interface)
- [🔌 Serial Interface](#-Serial-Interface)
- [🎉 Final Outcome](#-final-outcome)
- [🙏 Acknowledgments](#-acknowledgments)
- [🤝 Project Supporters](#-project-supporters)
- [❤️ Support the project](#-support-the-project)
- [🌐 Follow Me for Updates](#-follow-me-for-updates)

-----

## 🎯 Possible Additions
- **Deauthentication attack**
- **BLE spam**
- **Beacon spam**
- **nRF24 mousejack**

***`If you have the capability to assist with these features, please feel free to create pull requests!`***

-----

## 🚀 What Can You Do with This?
This amazing jammer is built on the **ESP32** architecture integrated with **configurable numbers of nRF24** modules. With its extraordinary capabilities, you can effectively disrupt signals across different technologies including:
- **Bluetooth** 🔊
- **BLE** 📱
- **Drones** 🚁
- **Wi-Fi** 📶
- **Zigbee**📡

-----

## 📋 List of Components
To bring this project to life, you will need the following components:
1. **nRF24L01+PA+LNA modules** 🛠️
2. **ESP-WROOM-32** ⚙️
3. **16V capacitors** rated at **100µF** 🔋
4. **128x32 or 128x64 OLED display** 📺 *(optional)*
5. **Tactile button** 🔘 *(optional)*

-----

## 🧑‍🔧 Let's Get Started with Soldering!
<details>
<summary><strong>With OLED</strong></summary>

<div style="margin-left: 20px;">

## Differences between versions

**Flexible version**

<ul><li>Number of modules: Supports up to 30 nRF24 modules (a software limit). In practice, the maximum number depends on the number of available GPIO pins on the microcontroller</li></ul>

<ul><li>Advantage: All modules share common MOSI, MISO, and SCK lines, which saves pins and simplifies wiring. This also ensures that access to the display remains available during operation</li></ul>

<ul><li>Flexibility: You can choose any number of modules within the limit, optimizing the device for your specific tasks</li></ul>

<ul><li>Disadvantage: Before first use, you must define the CE and CSN pins for each nRF24 module in the settings</li></ul>

**Compact version (outdated)**

    To migrate from Compact to Flexible:

    1. Erase the device's memory 
    2. Install the Flexible version firmware
    3. In the Web interface configure the pins:
    - CE: 16
    - CSN: 15

<ul><li>Number of modules: Uses only 1 nRF24 module, which significantly reduces the device's size</li></ul>

<ul><li>Disadvantage: Smaller jamming range and lower effectiveness compared to other versions</li></ul>

<ul><li>Compatibility: It is possible to flash the Flexible version firmware onto a Compact hardware device. If you do this, you will need to manually configure the CE and CSN pins for the single module in the settings</li></ul>

**Standard version (outdated)**

    To migrate from Standard to Flexible:

    1. Erase the device's memory 
    2. Install the Flexible version firmware
    3. Disconnect or reconnect the VSPI module according to the new Flexible wiring diagram
    4. In the Web interface configure the pins:
    - For the HSPI module: set CE to 16 and CSN to 15
    - For the VSPI module: set CE and CSN according to the pinout table in the Flexible documentation (if reconnected to new pins)

<ul><li>Number of modules: Used 2 nRF24 modules, which provided sufficient jamming power</li></ul>

<ul><li>Critical Flaw: Due to a pin conflict between the nRF24 modules and the display, the user interface became unresponsive after jamming was started. This made it impossible to stop the device via the display, requiring a full power cycle to shut it down</li></ul>

<ul><li>Status: Starting from version V2.6.0, this version is no longer supported or updated. It is highly recommended to switch to the Flexible version</li></ul>

---

<details>
<summary><strong>Flexible</strong></summary>

<div style="margin-left: 20px;">

### Connecting First nRF24 module (**optional**)
| **Pin Name** | **ESP32 GPIO** | **Connection**       |
|--------------|----------------|----------------------|
| VCC          | 3.3V          | (+) capacitor        |
| GND          | GND           | (-) capacitor        |
| CE           | GPIO 16       |                      |
| CSN          | GPIO 15       |                      |
| SCK          | GPIO 14       |                      |
| MOSI         | GPIO 13       |                      |
| MISO         | GPIO 12       |                      |
| IRQ          |                |                      |

### Connecting Second nRF24 module (**optional**)
| **Pin Name** | **ESP32 GPIO** | **Connection**       |
|--------------|----------------|----------------------|
| VCC          | 3.3V          | (+) capacitor        |
| GND          | GND           | (-) capacitor        |
| CE           | GPIO 18       |                      |
| CSN          | GPIO 17       |                      |
| SCK          | GPIO 14       |                      |
| MOSI         | GPIO 13       |                      |
| MISO         | GPIO 12       |                      |
| IRQ          |                |                      |

### Connecting Third nRF24 module (**optional**)
| **Pin Name** | **ESP32 GPIO** | **Connection**       |
|--------------|----------------|----------------------|
| VCC          | 3.3V          | (+) capacitor        |
| GND          | GND           | (-) capacitor        |
| CE           | GPIO 23       |                      |
| CSN          | GPIO 19       |                      |
| SCK          | GPIO 14       |                      |
| MOSI         | GPIO 13       |                      |
| MISO         | GPIO 12       |                      |
| IRQ          |                |                      |

### Connecting Fourth nRF24 module (**optional**)
| **Pin Name** | **ESP32 GPIO** | **Connection**       |
|--------------|----------------|----------------------|
| VCC          | 3.3V          | (+) capacitor        |
| GND          | GND           | (-) capacitor        |
| CE           | GPIO 5        |                      |
| CSN          | GPIO 4        |                      |
| SCK          | GPIO 14       |                      |
| MOSI         | GPIO 13       |                      |
| MISO         | GPIO 12       |                      |
| IRQ          |                |                      |

### Connecting Fifth nRF24 module (**optional**)
| **Pin Name** | **ESP32 GPIO** | **Connection**       |
|--------------|----------------|----------------------|
| VCC          | 3.3V          | (+) capacitor        |
| GND          | GND           | (-) capacitor        |
| CE           | GPIO 33       |                      |
| CSN          | GPIO 32       |                      |
| SCK          | GPIO 14       |                      |
| MOSI         | GPIO 13       |                      |
| MISO         | GPIO 12       |                      |
| IRQ          |                |                      |

### OLED Connection (**optional**)
| **Pin Name** | **ESP32 GPIO** |
|--------------|----------------|
| VCC          | 3.3V          |
| GND          | GND           |
| SCL          | GPIO 22       |
| SDA          | GPIO 21       |

### Button Connection (**optional**)
| **Button Actions** | **ESP32 GPIO** |
|--------------|----------------|
| OK          | GPIO 25       |
| NEXT (Optional)             | GPIO 26       |
| PREVIOUS (Optional)            | GPIO 27       |

![Flexible](schemes/Flexible/scheme.png)

###### In all configurations the same SCK, MOSI, and MISO pins are used. This is not a mistake—SPI interfaces can share clock and data lines, while proper operation is ensured by separate control signals (CSN and CE)

</div>
</details>

<details>
<summary><strong>Compact (outdated)</strong></summary>

<div style="margin-left: 20px;">

    To migrate from Compact to Flexible:

    1. Erase the device's memory 
    2. Install the Flexible version firmware
    3. In the Web interface configure the pins:
    - CE: 16
    - CSN: 15

### HSPI Connection
| **Pin Name** | **ESP32 GPIO** | **Connection**       |
|--------------|----------------|----------------------|
| VCC          | 3.3V          | (+) capacitor        |
| GND          | GND           | (-) capacitor        |
| CE           | GPIO 16       |                      |
| CSN          | GPIO 15       |                      |
| SCK          | GPIO 14       |                      |
| MOSI         | GPIO 13       |                      |
| MISO         | GPIO 12       |                      |
| IRQ          |                |                      |

### OLED Connection
| **Pin Name** | **ESP32 GPIO** |
|--------------|----------------|
| VCC          | 3.3V          |
| GND          | GND           |
| SCL          | GPIO 22       |
| SDA          | GPIO 21       |

### Button Connection
| **Button Actions** | **ESP32 GPIO** |
|--------------|----------------|
| OK          | GPIO 25       |
| NEXT (Optional)             | GPIO 26       |
| PREVIOUS (Optional)            | GPIO 27       |

![Compact](schemes/Compact/scheme.png)

</div>
</details>

<details>
<summary><strong>Standard (outdated)</strong></summary>

<div style="margin-left: 20px;">

    To migrate from Standard to Flexible:

    1. Erase the device's memory 
    2. Install the Flexible version firmware
    3. Disconnect or reconnect the VSPI module according to the new Flexible wiring diagram
    4. In the Web interface configure the pins:
    - For the HSPI module: set CE to 16 and CSN to 15
    - For the VSPI module: set CE and CSN according to the pinout table in the Flexible documentation (if reconnected to new pins)

### HSPI Connection
| **Pin Name** | **ESP32 GPIO** | **Connection**       |
|--------------|----------------|----------------------|
| VCC          | 3.3V          | (+) capacitor        |
| GND          | GND           | (-) capacitor        |
| CE           | GPIO 16       |                      |
| CSN          | GPIO 15       |                      |
| SCK          | GPIO 14       |                      |
| MOSI         | GPIO 13       |                      |
| MISO         | GPIO 12       |                      |
| IRQ          |                |                      |

### VSPI Connection
| **Pin Name** | **ESP32 GPIO** | **Connection**       |
|--------------|----------------|----------------------|
| VCC          | 3.3V          | (+) capacitor        |
| GND          | GND           | (-) capacitor        |
| CE           | GPIO 22       |                      |
| CSN          | GPIO 21       |                      |
| SCK          | GPIO 18       |                      |
| MOSI         | GPIO 23       |                      |
| MISO         | GPIO 19       |                      |
| IRQ          |                |                      |

### OLED Connection
| **Pin Name** | **ESP32 GPIO** |
|--------------|----------------|
| VCC          | 3.3V          |
| GND          | GND           |
| SCL          | GPIO 22       |
| SDA          | GPIO 21       |

### Button Connection
| **Button Actions** | **ESP32 GPIO** |
|--------------|----------------|
| OK          | GPIO 25       |
| NEXT (Optional)             | GPIO 26       |
| PREVIOUS (Optional)            | GPIO 27       |

![Standard](schemes/Standard/scheme.png)

</div>
</details>

</div>
</details>
<details>
<summary><strong>Without OLED (outdated)</strong></summary>

<div style="margin-left: 20px;">

    To migrate from Without OLED to Flexible:

    1. Erase the device's memory 
    2. Install the Flexible version firmware
    3. Disconnect or reconnect the VSPI module according to the new Flexible wiring diagram
    4. In the Web interface configure the pins:
    - For the HSPI module: set CE to 16 and CSN to 15
    - For the VSPI module: set CE and CSN according to the pinout table in the Flexible documentation (if reconnected to new pins)
    5. Go to the Web interface and turn off the display in the settings

### HSPI Connection
| **Pin Name** | **ESP32 GPIO** | **Connection**       |
|--------------|----------------|----------------------|
| VCC          | 3.3V          | (+) capacitor        |
| GND          | GND           | (-) capacitor        |
| CE           | GPIO 16       |                      |
| CSN          | GPIO 15       |                      |
| SCK          | GPIO 14       |                      |
| MOSI         | GPIO 13       |                      |
| MISO         | GPIO 12       |                      |
| IRQ          |                |                      |

### VSPI Connection
| **Pin Name** | **ESP32 GPIO** | **Connection**       |
|--------------|----------------|----------------------|
| VCC          | 3.3V          | (+) capacitor        |
| GND          | GND           | (-) capacitor        |
| CE           | GPIO 22       |                      |
| CSN          | GPIO 21       |                      |
| SCK          | GPIO 18       |                      |
| MOSI         | GPIO 23       |                      |
| MISO         | GPIO 19       |                      |
| IRQ          |                |                      |

![Without_OLED](schemes/Without_OLED/scheme.png)

</div>
</details>

-----

## 📦 Flash Firmware
<details>
<summary><strong>Manual Update</strong></summary>

<div style="margin-left: 20px;">

Follow these steps to flash the firmware:
1. Download the firmware from the **[releases](https://github.com/W0rthlessS0ul/nRF24_jammer/releases)** section that corresponds to your display
2. Navigate to **ESPWebTool**: [ESPWebTool](https://esp.huhn.me/)
3. Arrange the files as per the table below:

| **Address** | **File Name**                             |
|-------------|-------------------------------------------|
| 0x1000      | bootloader.bin              |
| 0x8000      | partitions.bin              |
| 0x10000     | firmware.bin                          |

4. Click "Flash," and once the installation is complete, **restart your board** and enjoy using your jammer!

![flash](img/flash.jpg)

</div>
</details>
<details>
<summary><strong>OTA upgrade</strong></summary>

<div style="margin-left: 20px;">

### This method can only be used if you have firmware version 1.9 or higher installed.
Follow these steps to flash the firmware:
1. Download the firmware from the **[releases](https://github.com/W0rthlessS0ul/nRF24_jammer/releases)** section that corresponds to your display
2. Turn on your device and open a web browser
3. Navigate to the web interface at **192.168.4.1**
4. Click on "**OTA**" and select the file "**firmware.bin**" (`⚠️️ make sure not to choose the bootloader or partitions files`)

![web_interface](img/gif/OTA.gif)

</div>
</details>

###### ⚠️ Important for source builds: Since v2.5.0, this project uses a modified RF24 library (see /lib).

-----

## 🎮 Device Control 

<details>
<summary><strong>🟢 1-Button Configuration</strong></summary>
<div style="margin-left: 20px;">

#### 📋 Menu Navigation
- **Short press** → Next menu item
- **Long press** → Select menu item

#### 📡 Misc Jammer
- **Short press** → Channel +1
- **Long press** → Continuous channel +1 (every 100ms)
- **Double press** → Select channel

</div>
</details>

<details>
<summary><strong>🔵 2-Button Configuration</strong></summary>
<div style="margin-left: 20px;">

#### 📋 Menu Navigation
- **Next button** short press → Next menu item
- **OK button** short press → Select menu item

#### 📡 Misc Jammer
- **Next button** short press → Channel +1
- **Next button** long press → Continuous channel +1 (every 100ms)
- **OK button** short press → Select channel

</div>
</details>

<details>
<summary><strong>🟣 3-Button Configuration</strong></summary>
<div style="margin-left: 20px;">

#### 📋 Menu Navigation
- **Next button** short press → Next menu item
- **Previous button** short press → Previous menu item
- **OK button** short press → Select menu item

#### 📡 Misc Jammer
- **Next button** short press → Channel +1
- **Next button** long press → Continuous channel +1 (every 100ms)
- **Previous button** short press → Channel -1
- **Previous button** long press → Continuous channel -1 (every 100ms)
- **OK button** short press → Select channel

</div>
</details>

> **Single press** stops active attacks (not supported on "Standard" versions)

-----

## 🌐 Web Interface

- To utilize the web interface, please follow the steps outlined below
1. activate the **nRF24 jammer**
2. Connect to the Wi-Fi network named `jammer` using the password `W0rthlessS0ul`
3. open your web browser and navigate to the IP address `192.168.4.1`
4. Now you can control your nRF24 jammer through an web interface

![web_interface](img/gif/web_interface_.gif)

-----

## 🔌 Serial Interface

- To utilize the serial interface, please follow the steps outlined below
1. Activate the **nRF24 jammer**
2. Connect the device to a PC or phone via USB
3. You can use any serial terminals for control, for example: [spacehuhn terminal](https://terminal.spacehuhn.com/)
4. Now write **help** to get a list of commands

![serial_interface](img/gif/serial_interface.gif)

-----

## 🎉 Final Outcome
### Standard version 
![Standard](img/Standard.jpg)

### Compact version
![Compact](img/Compact.jpg)

### Normal Spectrum
![Normal Spectrum](img/gif/normal_spctr.gif)

### Bluetooth Jam Spectrum
![Bluetooth Jam Spectrum](img/gif/bluetooth_jam_spctr.gif)

### Drone Jam Spectrum
![Drone Jam Spectrum](img/gif/drone_jam_spctr.gif)

### Wi-Fi Jam Spectrum
![Wi-Fi Jam Spectrum](img/gif/wifi_jam_spctr.gif)

### BLE Jam Spectrum
![BLE Jam Spectrum](img/gif/ble_jam_spctr.gif)

### Zigbee Jam Spectrum
![Zigbee Jam Spectrum](img/gif/zigbee_jam_spctr.gif)

### Misc Jam Spectrum
![Misc Jam Spectrum](img/gif/misc_jam_spctr.gif)

-----

## 🙏 Acknowledgments
- [smoochiee](https://github.com/smoochiee) - **for the main logic of the jammer.**
- [samukas81](https://github.com/samukas81) - **for suggesting ideas.**

-----

## 🤝 Project Supporters

| **Supporter**                                  | **Contribution**|
|------------------------------------------------|-----------------|
|  [@sergey2735](https://github.com/sergey2735)  |    40.7 ADA     |

-----

## ❤️ Support the project
If you would like to support this project, please consider starring the repository or following me! If you appreciate the hard work that went into this, buying me a cup of coffee would keep me fueled! ☕ 

**BTC Address:** `bc1qvul4mlxxw5h2hnt8knnxdrxuwgpf4styyk20tm`

**ETH Address:** `0x5c54eAb2acFE1c6C866FB4b050d8B69CfB1138Af`

**LTC Address:** `LbdzCsYbxuD341raar6Cg1yKavaDq7fjuV`

**XRP Address:** `rKLLPzoBGfqY3pAQPwTFPRYaWjpHSwHNDw`

**ADA Address:** `addr1qyz2aku0ucmxqnl60lza23lkx2xha8zmxz9wqxnrtvpjysgy4mdcle3kvp8l5l7964rlvv5d06w9kvy2uqdxxkcryfqs7pajev`

**DOGE Address:** `DBzAvD62yQUkP4Cb7C5LuFYQEierF3D3oG`

Every donation is greatly appreciated and contributes to the ongoing development of this project!

---

## 🌐 Follow Me for Updates
Stay connected to receive the latest updates:

[![GitHub](https://img.shields.io/badge/GitHub-W0rthlessS0ul-181717?style=flat&logo=github&logoColor=white)](https://github.com/W0rthlessS0ul)

<img src="https://profile-counter.glitch.me/W0rthlessS0ul.nRF24_jammer/count.svg"/>
