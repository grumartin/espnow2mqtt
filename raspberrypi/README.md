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
```

### Step 4: Install Nginx to host static Website

    ```bash
    sudo apt-get install nginx
    ```
    Installs Nginx on the Raspberry. To start it, execute following command

    ```bash
    sudo systemctl start nginx
    ```

   Next copy the index.html File to the Raspberry
   ```bash
   scp ./index.html pi@<raspberry_pi_ip>:/home/pi/index.html 
   ```

   Move the file to the following directory
   ```bash 
   sudo cp /home/pi/index.html /var/www/html/
   ```
  
   Update the index.html with the actual IP of the Raspberry
   ```bash
   sudo nano /var/www/html/index.html
   ```
  
   Lastly resart nginx with the command
   ```bash 
   sudo systemctl restart nginx
   ```

 
