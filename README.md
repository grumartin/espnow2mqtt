# espnow2mqtt project

# Project Overview

![project-overview](https://github.com/user-attachments/assets/2bc733ac-9805-478d-ab15-95698acb7332)

# Project Setup and Usage Guide

This project consists of three main parts:

1. **examples**: Code for the ESP8266/ESP32 sensors, which send data to a central gateway via ESP-NOW.
2. **esp2mqttGateway**: Code for the ESP-based gateway that receives data from the sensors and forwards it to an MQTT broker.
3. **raspberrypi**: A Raspberry Pi setup that runs Mosquitto (MQTT broker) in a Docker container and serves a web interface using Nginx.

Additionally, we will cover how to use **PlatformIO** to flash the ESP devices via **Visual Studio Code**.

## Project Folder Structure

### 1. `examples/`
This folder contains the code for the ESP devices (ESP8266/ESP32) that act as sensors. These devices send sensor data to the **esp2mqttGateway** via ESP-NOW. The sensors may include temperature, humidity, or other sensor data, depending on the specific application.

### 2. `esp2mqttGateway/`
This folder contains the code for the **ESP gateway**. The gateway receives the data from the sensors over **ESP-NOW** and then forwards this data to an MQTT broker. This code also handles the communication with the broker and may include handling incoming MQTT messages.

### 3. `raspberrypi/`
This folder contains the setup for a **Raspberry Pi** that runs **Mosquitto** (an MQTT broker) in a Docker container. It also includes configuration files for setting up **Nginx** to serve a web interface (e.g., a dashboard or status page for your IoT system).

---

## Setting Up PlatformIO and Flashing ESP Devices

### What is PlatformIO?
**PlatformIO** is an integrated development environment (IDE) and build system for embedded systems, such as ESP8266 and ESP32. It allows you to write, compile, and flash firmware to your devices directly from Visual Studio Code.

### Installing PlatformIO

1. **Install Visual Studio Code (VS Code)**:
    - Download Visual Studio Code from [here](https://code.visualstudio.com/) and install it on your system.

2. **Install PlatformIO in VS Code**:
    - Open VS Code.
    - Go to the **Extensions** view by clicking the **Extensions icon** on the sidebar.
    - Search for **PlatformIO IDE** and click **Install**.
    - Once installed, the PlatformIO toolbar should appear in the VS Code sidebar.

---

## Flashing the ESP Devices

### Step 1: Open the Project in VS Code

1. Open **Visual Studio Code**.
2. Select **File > Open Folder** and navigate to the **`esp_sensors`** folder (or **`esp2mqttGateway`** folder if you're working on the gateway code).
3. VS Code will automatically detect the **PlatformIO project** if the `platformio.ini` file is present in the folder.

### Step 2: Flash the ESP Devices

1. Connect your ESP8266 or ESP32 device to your computer via USB.
2. In PlatformIO, click the PlatformIO icon on the left sidebar.
3. Click on Upload (right arrow icon) to compile and upload the code to your ESP device.

PlatformIO will automatically compile the code, upload it to your device, and start the serial monitor to view any print statements from your ESP.

### Step 4: Monitor the Device Output

Once the code is uploaded, you can monitor the output from your ESP device by clicking on the Monitor (plug icon) in PlatformIO.
