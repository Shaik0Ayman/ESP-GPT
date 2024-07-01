ESP-GPT is a project designed to leverage the ESP32 microcontroller to connect to OpenAI's GPT (Generative Pre-trained Transformer) and host a language model on a small OLED display. This setup allows for interaction with the language model directly from the ESP32 device, making it a compact and portable solution for various applications.




Table of Contents:


1.Introduction

2.Hardware Requirements

3.Software Requirements

4.Setting Up the ESP32

5.Connecting to OpenAI

6.Displaying Output on the OLED

7.Example Use Cases

8.Troubleshooting

9.Future Improvements

10.References






Introduction:
ESP-GPT combines the power of OpenAI's GPT with the flexibility of the ESP32 microcontroller. This documentation provides a step-by-step guide to set up the hardware and software, connect to OpenAI, and display the model's output on an OLED screen.




Hardware Requirements:
1.ESP32 development board
2.Small OLED display (e.g., SSD1306, 128x64 pixels)
3.Jumper wires
4.Breadboard (optional)
5.USB cable for programming the ESP32
6.Some Leds



Software Requirements:
- Arduino IDE or PlatformIO
- ESP32 board support package for Arduino/PlatformIO
- Libraries: WiFi, HTTPClient, Adafruit_GFX, Adafruit_SSD1306
- OpenAI API key



Setting Up the ESP32:

Install ESP32 Board Support:
Open Arduino IDE.
Go to File > Preferences.
In the "Additional Board Manager URLs" field, add: https://dl.espressif.com/dl/package_esp32_index.json
Go to Tools > Board > Board Manager.
Search for "esp32" and install the package.

Connecting the OLED Display:
Connect the OLED display to the ESP32 using I2C.
Example connections:
VCC to 3.3V
GND to GND
SCL to GPIO 22
SDA to GPIO 21

Connecting to OpenAI:
Get OpenAI API Key:
Sign up for an account at OpenAI.
Obtain an API key from the API section of your account.
Code to Connect to Wi-Fi and OpenAI:
'''
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

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("WiFi Connected");
  display.display();

  // Call OpenAI API
  makeOpenAIRequest();
}

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


Displaying Output on the OLED:
Parse the OpenAI response and display it on the OLED screen.
Example code:

void displayResponse(String response) {
  // Parse the response and display it on the OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(response);
  display.display();
}
'''
Example Use Cases:
- Text generation: Generate creative writing, poetry, or stories.
- Chatbot: Create a conversational interface with the language model.
- Question answering: Provide answers to user queries based on the model's knowledge.
- Language translation: Translate text between different languages.Displaying Output on the OLED:
Parse the OpenAI response and display it on the OLED screen.
Example code:

void displayResponse(String response) {
  // Parse the response and display it on the OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(response);
  display.display();
}

Example Use Cases:
- Text generation: Generate creative writing, poetry, or stories.
- Chatbot: Create a conversational interface with the language model.
- Question answering: Provide answers to user queries based on the model's knowledge.
- Language translation: Translate text between different languages.

6. Displaying Output on the OLED
The displayResponse function in the code above handles parsing and displaying the response from OpenAI on the OLED screen.
Modify the function to parse the JSON response correctly and extract the relevant text.

7. Example Use Cases
Portable AI assistant
Smart IoT devices with language capabilities
Educational tools for learning about AI and microcontrollers

8. Troubleshooting
Wi-Fi Connection Issues:
Ensure the SSID and password are correct.
Check Wi-Fi signal strength.
OLED Display Not Working:

Verify connections (VCC, GND, SCL, SDA).
Ensure the correct I2C address is used.
API Request Failures:

Check API key validity.
Verify internet connectivity.
Inspect the response code and error messages.

9. Future Improvements
Add a touchscreen for better interactivity.
Integrate voice input and output.
Optimize the code for lower power consumption.


10. References
ESP32 Documentation
OpenAI API Documentation
Adafruit SSD1306 Library
Arduino JSON Library
