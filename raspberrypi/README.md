# Raspberry Pi Setup Guide

This guide will walk you through setting up Mosquitto, Nginx, and configuring the necessary files on your Raspberry Pi. It includes instructions on copying the Mosquitto folder, starting the container with Docker Compose, and configuring Nginx.

## Prerequisites

- **Raspberry Pi** with Raspbian OS installed.
- **Docker** and **Docker Compose** installed on your Raspberry Pi.
- **Nginx** installed via `apt-get`.
- **Mosquitto** and **Nginx configuration files** available in your project folder.
- Basic knowledge of using the terminal/SSH.

---

## 1. **Setting Up Mosquitto with Docker Compose**

### Step 1: Copy the Mosquitto Folder

1. Copy the Mosquitto folder (which includes configuration files like `mosquitto.conf` and any other necessary files) to your Raspberry Pi.

    Example command to copy the folder (run this on your local machine):

    ```bash
    # Replace <raspberry_pi_ip> with your Raspberry Pi's IP address
    scp -r ./mosquitto pi@<raspberry_pi_ip>:/home/pi/mosquitto
    ```

### Step 2: Set Up and Start Mosquitto using Docker Compose

1. SSH into your Raspberry Pi:

    ```bash
    ssh pi@<raspberry_pi_ip>
    ```

2. Navigate to the `mosquitto` folder on your Raspberry Pi:

    ```bash
    cd /home/pi/mosquitto
    ```

3. **Start Mosquitto using Docker Compose**. Ensure that a `docker-compose.yml` file is present in the folder. If it isn't already, you may need to create one.

    ```bash
    sudo docker-compose up -d
    ```

    This command will start the Mosquitto service in the background.

### Step 3: Verify Mosquitto is Running

You can verify if Mosquitto is running by checking the status of your containers:

```bash
sudo docker ps
