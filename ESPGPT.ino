
#include "WiFi.h"
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// Replace with your network credentials
const char* ssid     = "ssid";
const char* password = "password";
//chatgpt api
const char* chatgpt_token = "apikey";

char chatgpt_server[] = "https://api.openai.com/v1/completions";

// Set web server port number to 80
WiFiServer server(80);
WiFiClient client1;

HTTPClient https;

String chatgpt_Q;
String chatgpt_A;
String json_String;
uint16_t dataStart = 0;
uint16_t dataEnd = 0;
String dataStr;
int httpCode = 0;

typedef enum 
{
  do_webserver_index,
  send_chatgpt_request,
  get_chatgpt_list,
}STATE_;

STATE_ currentState;

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
}

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
#define LED_PIN1 13
#define LED_PIN2 12
#define LED_PIN3 14
#define LED_PIN4 27
#define LED_PIN5 26
#define LED_PIN6 25
void setup()
{
    pinMode(LED_PIN1, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);
    pinMode(LED_PIN3, OUTPUT);
    pinMode(LED_PIN4, OUTPUT);
    pinMode(LED_PIN5, OUTPUT);
    pinMode(LED_PIN6, OUTPUT);
    Serial.begin(115200);
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
    }
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
 
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    while(!Serial);

    Serial.println("WiFi Setup done!");
    
    WiFiConnect();
    // Start the TCP server server
    server.begin();
}
 
void loop()
{
  display.clearDisplay();
  display.setTextSize(2.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WELCOME");
  display.println("TO");
  display.println("ESP-GPT");
  display.setTextSize(1);
  display.println("");
  display.println("-by Ayman");


  display.display();
  switch(currentState){
    case do_webserver_index:
      Serial.println("Web Production Task Launch");
      client1 = server.available();              // Check if the client is connected
      if (client1){
        Serial.println("New Client.");           // print a message out the serial port
        // an http request ends with a blank line
        boolean currentLineIsBlank = true;    
        while (client1.connected()){
          if (client1.available()){
            char c = client1.read();             // Read the rest of the content, used to clear the cache
            json_String += c;
            if (c == '\n' && currentLineIsBlank) {                                 
              dataStr = json_String.substring(0, 4);
              Serial.println(dataStr);
              if(dataStr == "GET "){
                client1.print(html_page);        //Send the response body to the client
              }         
              else if(dataStr == "POST"){
                json_String = "";
                while(client1.available()){
                  json_String += (char)client1.read();
                }
                Serial.println(json_String); 
                dataStart = json_String.indexOf("chatgpttext=") + strlen("chatgpttext="); // parse the request for the following content
                chatgpt_Q = json_String.substring(dataStart, json_String.length());                    
                client1.print(html_page);
                Serial.print("Your Question is: ");
                display.clearDisplay();
                display.setTextSize(1);
                display.setTextColor(WHITE);
                display.setCursor(0, 0);
                display.println("YOUR QUESTION IS :");
                display.println(chatgpt_Q);
                display.display();
                Serial.println(chatgpt_Q);
                digitalWrite(LED_PIN1, HIGH);
                delay(2000);
                digitalWrite(LED_PIN2, HIGH);
                delay(2000);
                digitalWrite(LED_PIN3, HIGH);
                delay(2000);
                digitalWrite(LED_PIN1, LOW);
                digitalWrite(LED_PIN2, LOW);
                digitalWrite(LED_PIN3, LOW);

                // close the connection:
                delay(10);
                client1.stop();       
                currentState = send_chatgpt_request;
              }
              json_String = "";
              break;
            }
            if (c == '\n') {
              // you're starting a new line
              currentLineIsBlank = true;
            }
            else if (c != '\r') {
              // you've gotten a character on the current line
              currentLineIsBlank = false;
            }
          }
        }
      }
      delay(1000);
      break;
    case send_chatgpt_request:
      Serial.println("Ask ChatGPT a Question Task Launch");
      if (https.begin(chatgpt_server)) {  // HTTPS
        https.addHeader("Content-Type", "application/json"); 
        String token_key = String("Bearer ") + chatgpt_token;
        https.addHeader("Authorization", token_key);
        String payload = String("{\"model\": \"gpt-3.5-turbo-instruct\", \"prompt\": \"") + chatgpt_Q + String("\", \"temperature\": 0, \"max_tokens\": 50}"); //Instead of TEXT as Payload, can be JSON as Paylaod
        httpCode = https.POST(payload);   // start connection and send HTTP header
        payload = "";
        currentState = get_chatgpt_list;
      }
      else {
        Serial.println("[HTTPS] Unable to connect");
        delay(1000);
      }
      break;
    case get_chatgpt_list:
      Serial.println("Get ChatGPT Answers Task Launch");
      // httpCode will be negative on error      
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        Serial.println(payload);
        dataStart = payload.indexOf("\\n\\n") + strlen("\\n\\n");
        dataEnd = payload.indexOf("\",", dataStart); 
        chatgpt_A = payload.substring(dataStart, dataEnd);
        Serial.print("ChatGPT Answer is: ");
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("YOUR ANSWER IS :");
        display.display();
        display.println(chatgpt_A);
        Serial.println(chatgpt_A);
        display.display();
        digitalWrite(LED_PIN4, HIGH);
        delay(3000);
        digitalWrite(LED_PIN5, HIGH);
        delay(3000);
        digitalWrite(LED_PIN6, HIGH);
        delay(4000);
        digitalWrite(LED_PIN4, LOW);
        digitalWrite(LED_PIN5, LOW);
        digitalWrite(LED_PIN6, LOW);
        Serial.println("Wait 10s before next round...");
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Wait 10s");
        display.println("before");
        display.println("next round...");
        display.display();
        digitalWrite(LED_PIN1, HIGH);
        delay(200);
        digitalWrite(LED_PIN2, HIGH);
        delay(200);
        digitalWrite(LED_PIN3, HIGH);
        delay(200);
        digitalWrite(LED_PIN4, HIGH);
        delay(200);
        digitalWrite(LED_PIN5, HIGH);
        delay(200);
        digitalWrite(LED_PIN6, HIGH);
        delay(200);
        digitalWrite(LED_PIN1, LOW);
        delay(200);
        digitalWrite(LED_PIN2, LOW);
        delay(200);
        digitalWrite(LED_PIN3, LOW);
        delay(200);
        digitalWrite(LED_PIN4, LOW);
        delay(200);
        digitalWrite(LED_PIN5, LOW);
        delay(200);
        digitalWrite(LED_PIN6, LOW);
        delay(200);
        currentState = do_webserver_index;
      }
      else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        while(1);
      }
      https.end();
      break;
  }
}