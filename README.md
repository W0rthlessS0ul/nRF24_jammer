<div align="center">
  <img src="https://avatars.githubusercontent.com/u/176677387" width="150" height="auto" />
  <h1> 🌟 nRF24 Jammer 🌟 </h1>
</div>

Welcome to the **nRF24 Jammer** repository! 🎉 Dive into the world of RF interference with this unique project based on the ESP32 and NRF24 technology.

## 📚 Table of Contents
- [🎯 Possible Additions](#-possible-additions)
- [🚀 What Can You Do with This?](#-what-can-you-do-with-this)
- [📋 List of Components](#-list-of-components)
- [🧑‍🔧 Let's Get Started with Soldering!](#-lets-get-started-with-soldering)
- [📦 Flash Firmware](#-flash-firmware)
- [🌐 Web Interface](#-Web-Interface)
- [🎉 Final Outcome](#-final-outcome)
- [🙏 Acknowledgments](#-acknowledgments)
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
This amazing jammer is built on the **ESP32** architecture integrated with **two NRF24** modules. With its extraordinary capabilities, you can effectively disrupt signals across different technologies including:
- **Bluetooth** 🔊
- **BLE** 📱
- **Drones** 🚁
- **Wi-Fi** 📶
- **Zigbee**📡

-----

## 📋 List of Components
To bring this project to life, you will need the following components:
1. **Two NRF24L01+PA+LNA modules** *(or one for the "Compact" version)* 🛠️
2. **ESP32-DevKitC** *(with Type-C)* **or**  **ESP32-DevKit V1** *(with Micro USB)*⚙️
3. **Two 16V capacitors** rated at **100µF** 🔋
4. **128x32 or 128x64 OLED display** 📺 *(Not required when using the "without OLED" version)*
5. **Tactile button** 🔘 *(Not required when using the "without OLED" version)*

-----

## 🧑‍🔧 Let's Get Started with Soldering!
<details>
<summary><strong>With OLED</strong></summary>

<div style="margin-left: 20px;">

## Differences between versions

The **Compact** version is equipped with a **single NRF24** module, while the **Standard** version features **two**.
 
Notably, the Compact version allows **uninterrupted access to the display** even when jamming is started. 

This will enable me to utilize features that may be added in the future but are currently **unavailable** in the **Standard version**. For instance, **one already implemented** feature is the a**bility to exit jamming mode** by simply pressing the "**OK**" button (*pin 25*). 

Given these advantages, **I highly recommend choosing the Compact version** for its versatility and potential for future enhancements.

---

<details>
<summary><strong>Compact</strong></summary>

<div style="margin-left: 20px;">

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


</div>
</details>

<details>
<summary><strong>Standard</strong></summary>

<div style="margin-left: 20px;">

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


</div>
</details>

</div>
</details>
<details>
<summary><strong>Without OLED</strong></summary>

<div style="margin-left: 20px;">

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

</div>
</details>

-----

## 📦 Flash Firmware
<details>
<summary><strong>Manual Update</strong></summary>

<div style="margin-left: 20px;">

Follow these steps to flash the firmware:
1. Download the firmware from the **releases** section that corresponds to your display.
2. Navigate to **ESPWebTool**: [ESPWebTool](https://esp.huhn.me/)
3. Arrange the files as per the table below:

| **Address** | **File Name**                             |
|-------------|-------------------------------------------|
| 0x1000      | nRF24_jammer_bootloader.bin              |
| 0x8000      | nRF24_jammer_partitions.bin              |
| 0x10000     | nRF24_jammer.bin                          |

4. Click "Flash," and once the installation is complete, **restart your board** and enjoy using your jammer!

![flash](img/flash.jpg)

</div>
</details>
<details>
<summary><strong>OTA upgrade</strong></summary>

<div style="margin-left: 20px;">

### This method can only be used if you have firmware version 1.9 or higher installed.
Follow these steps to flash the firmware:
1. Turn on your device and open a web browser
2. Navigate to the web interface at **192.168.4.1**
3. Click on "**OTA**" and select the file "**nRF24_jammer.bin**" (`⚠️️ make sure not to choose the bootloader or partitions files`).

</div>
</details>

-----

## 🌐 Web Interface

- To utilize the web interface, please follow the steps outlined below.
1. activate the **nRF24 jammer**.
2. Connect to the Wi-Fi network named `jammer` using the password `W0rthlessS0ul`.
3. open your web browser and navigate to the IP address `192.168.4.1`.
4. Now you can control your nRF24 jammer through an web interface.

![web_interface](img/gif/web_interface_.gif)

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

-----

## 🙏 Acknowledgments
- [smoochiee](https://github.com/smoochiee) - **for the main logic of the jammer.**
- [samukas81](https://github.com/samukas81) - **for suggesting ideas.**

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
