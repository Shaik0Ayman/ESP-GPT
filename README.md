# ESP-GPT

ESP-GPT is a project designed to leverage the ESP32 microcontroller to connect to OpenAI's GPT (Generative Pre-trained Transformer) and host a language model on a small OLED display. This setup allows for interaction with the language model directly from the ESP32 device, making it a compact and portable solution for various applications.

## Table of Contents

1. [Introduction](#introduction)
2. [Hardware Requirements](#hardware-requirements)
3. [Software Requirements](#software-requirements)
4. [Setting Up the ESP32](#setting-up-the-esp32)
5. [Connecting to OpenAI](#connecting-to-openai)
7. [Displaying Output on the OLED](#displaying-output-on-the-oled)
8. [Example Use Cases](#example-use-cases)
9. [Troubleshooting](#troubleshooting)
10. [Future Improvements](#future-improvements)
11. [References](#references)

## Introduction

ESP-GPT combines the power of OpenAI's GPT with the flexibility of the ESP32 microcontroller. This documentation provides a step-by-step guide to set up the hardware and software, connect to OpenAI, and display the model's output on an OLED screen.

## Hardware Requirements

- ESP32 development board
- Small OLED display (e.g., SSD1306, 128x64 pixels)
- Jumper wires
- Breadboard (optional)
- USB cable for programming the ESP32

## Software Requirements

- Arduino IDE or PlatformIO
- ESP32 board support package for Arduino/PlatformIO
- Libraries: `WiFi`, `HTTPClient`, `Adafruit_GFX`, `Adafruit_SSD1306`
- OpenAI API key

## Setting Up the ESP32

1. **Install ESP32 Board Support:**
   - Open Arduino IDE.
   - Go to File > Preferences.
   - In the "Additional Board Manager URLs" field, add: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Go to Tools > Board > Board Manager.
   - Search for "esp32" and install the package.

2. **Connecting the OLED Display:**
   - Connect the OLED display to the ESP32 using I2C.
   - Example connections:
     - `VCC` to `3.3V`
     - `GND` to `GND`
     - `SCL` to `GPIO 22`
     - `SDA` to `GPIO 21`

## Connecting to OpenAI

1. **Get OpenAI API Key:**
   - Sign up for an account at [OpenAI](https://www.openai.com).
   - Obtain an API key from the API section of your account.

2. **Code to Connect to Wi-Fi and OpenAI:**

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* openai_api_key = "your_OPENAI_API_KEY";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  // Initialize the OLED display
  if(!display.begin(SSD1306_I2C_ADDRESS, OLED_RESET)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Connecting to WiFi...");
  display.display();

void WiFiConnect(void){
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("CONNECT TO");
    display.println(WiFi.localIP());
    display.display();
    currentState = do_webserver_index;
    delay(5000);

void loop() {
}

void makeOpenAIRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("https://api.openai.com/v1/engines/davinci/completions");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + openai_api_key);

    String payload = "{\"prompt\": \"Say something smart\", \"max_tokens\": 50}";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
      displayResponse(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void displayResponse(String response) {
  // Parse the response and display it on the OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(response);
  display.display();
}
```
## HTML code for the prompt sender
- the following code will give the user a webpage on the local network to use ESP-GPT

```cpp
const char html_page[] PROGMEM = {
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"  // the connection will be closed after completion of the response
    //"Refresh: 1\r\n"         // refresh the page automatically every n sec
    "\r\n"
    "<!DOCTYPE HTML>\r\n"
    "<html>\r\n"
    "<head>\r\n"
      "<meta charset=\"UTF-8\">\r\n"
      "<title>PROMPT SENDER</title>\r\n"
      "<link rel=\"icon\" href=\"https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/chatgpt-logo.png\" type=\"image/x-icon\">\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "<p style=\"text-align:center;\">\r\n"
    "<img alt=\"ChatGPT\" src=\"https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/chatgpt-logo.png\" height=\"200\" width=\"200\">\r\n"
    "<h1 align=\"center\">OpenAI ChatGPT</h1>\r\n" 
    "<h1 align=\"center\">SEND YOUR PROMPTS TO YOUR ESP FROM HERE</h1>\r\n" 
    "<div style=\"text-align:center;vertical-align:middle;\">"
    "<form action=\"/\" method=\"post\">"
    "<input type=\"text\" placeholder=\"Please enter your question\" size=\"35\" name=\"chatgpttext\" required=\"required\"/>\r\n"
    "<input type=\"submit\" value=\"Submit\" style=\"height:30px; width:80px;\"/>"
    "</form>"
    "</div>"
    "</p>\r\n"
    "</body>\r\n"
    "<html>\r\n"
};
```


## Displaying Output on the OLED

The displayResponse function in the code above handles parsing and displaying the response from OpenAI on the OLED screen.
Modify the function to parse the JSON response correctly and extract the relevant text.

## Example Use Cases
- Portable AI assistant
- Smart IoT devices with language capabilities
- Educational tools for learning about AI and microcontrollers
- Troubleshooting
- Wi-Fi Connection Issues:

## Troubleshooting
- Ensure the SSID and password are correct.
- Check Wi-Fi signal strength.
- OLED Display Not Working:
- Verify connections (VCC, GND, SCL, SDA).
- Ensure the correct I2C address is used.
API Request Failures:
- Check API key validity.
- Verify internet connectivity.
- Inspect the response code and error messages.

## Future Improvements
- Add a touchscreen for better interactivity.
- Integrate voice input and output.
- Optimize the code for lower power consumption.

## References
- ESP32 Documentation
- OpenAI API Documentation
- Adafruit SSD1306 Library
- Arduino JSON Library
