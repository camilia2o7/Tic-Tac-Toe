### **1. Install Arduino IDE**  
- Download and install the **Arduino IDE** from [this link](https://www.arduino.cc/en/software).

### **2. Install ESP32 Board in Arduino IDE**  
- Open **Arduino IDE**.  
- Go to **File** > **Preferences**.  
- In the **Additional Board Manager URLs** section, add the following URL:  
  ```
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
- Click **OK** to save.  
- Open **Tools** > **Board** > **Boards Manager**.  
- Search for **ESP32**, then click **Install**.

### **3. Connect Your ESP32 to the Computer**  
- Use a **USB Type-C data cable** (not just a charging cable) to connect your **ESP32 Rev1 Type-C** to your computer.  
- If your ESP32 is not detected, you may need to install the appropriate USB-to-UART driver:  
  - **CP210x Driver**: [Download here](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads)  
  - **CH340 Driver**: [Download here](https://www.wch.cn/download/CH341SER_EXE.html)  
- Open **Tools** > **Port** in **Arduino IDE** and select the correct **COM port** for your ESP32.

### **4. Select the ESP32 Board in Arduino IDE**  
- Go to **Tools** > **Board** and select **ESP32 Dev Module** (or the specific ESP32 board you are using).

### **5. Install the ArduinoJson Library**  
- In **Arduino IDE**, go to **Sketch** > **Include Library** > **Manage Libraries**.  
- In the **Library Manager**, search for **ArduinoJson** by **Benoit Blanchon**.  
- Click **Install**.  
- More details about the library can be found on [Benoit Blanchon's blog](https://blog.benoitblanchon.fr).

### **6. Upload a Test Sketch**  
- Open a simple **example sketch** from **File** > **Examples** > **Basics** > **Blink**.  
- Click the **Upload** button in Arduino IDE to upload the sketch to your ESP32.

### **7. Open Serial Monitor (Optional)**  
- If needed, open **Tools** > **Serial Monitor** to check for any messages from your ESP32.  
- Set the **baud rate** to **115200** for proper communication.

After following these steps, your **ESP32 Rev1 Type-C** should be successfully connected and ready for programming!

---