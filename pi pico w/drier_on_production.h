#include <TM1637Display.h>
#include <max6675.h>
#include <WiFi.h>
#include "TRIGGER_GOOGLESHEETS.h"

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

// Define pins for TM1637 Displays:
#define DISPLAY1_CLK 6
#define DISPLAY1_DIO 7

#define DISPLAY2_CLK 8
#define DISPLAY2_DIO 9

// Define pins for MAX6675 Modules:
#define MAX6675_1_SO 10
#define MAX6675_1_CS 11
#define MAX6675_1_SCK 12

#define MAX6675_2_SO 13
#define MAX6675_2_CS 14
#define MAX6675_2_SCK 15

// Buzzer pin
#define BUZZER_PIN 18

// MQ2 Sensor pin
#define MQ2_PIN 19
#define GAS_THRESHOLD 300

int timer = 0;

uint8_t data[] = { 0x79, 0x50, 0x50, 0x00 };

// Initialize MAX6675 Modules
MAX6675 thermocouple1(MAX6675_1_SCK, MAX6675_1_CS, MAX6675_1_SO);
MAX6675 thermocouple2(MAX6675_2_SCK, MAX6675_2_CS, MAX6675_2_SO);

// Initialize TM1637 Displays
TM1637Display display1(DISPLAY1_CLK, DISPLAY1_DIO);
TM1637Display display2(DISPLAY2_CLK, DISPLAY2_DIO);

/********** Google Sheets Definitions ***********/
char column_name_in_sheets[][6] = { "temp1", "temp2", "v3", "v4" };
String Sheets_GAS_ID = "AKfycbyZauZK4WTLWMsuvJnpGlFA-N9OOMmdF7vsj-wYsUKoThV07JQ8BW9iQxdO9vM1AQ8eNw";
int No_of_Parameters = 4;
/*********************************************/

void setup() {
  Serial.begin(115200);

  // Initialize buzzer pin as output
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize MQ2 sensor pin
  pinMode(MQ2_PIN, INPUT);

  display1.setBrightness(7);
  display2.setBrightness(0x0f);

  Serial.println("Initializing...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    timer++;
    delay(1000);
    Serial.println("Connecting to WiFi...");

    // Scroll "WiFi not connected" on the display
    scrollText("CONNECTING TO WIFI_____", display1);

    double temp1 = thermocouple1.readCelsius();
    double temp2 = thermocouple2.readCelsius();

    if (temp1 > 0 && temp1 < 1000) {
      Serial.println("1 ok");
    } else {
      temp1 = -1;
    }

    if (temp2 > 0 && temp2 < 1000) {
      Serial.println("2 ok");
    } else {
      temp2 = -1;
    }

    // Display temperatures
    display1.showNumberDec(int(temp1));
    display2.showNumberDec((int)temp2, false);

    if (timer > 5 || WiFi.status() == WL_CONNECTED) {
      break;
    }
  }
  // Serial.println("Connected to WiFi");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WIFI NOT CONNECTED");
    scrollText("WIFI NOT CONNECTED", display1);
  } else {
    Serial.println("WIFI CONNECTED");
    scrollText("WIFI CONNECTED", display1);
  }

  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters);

  delay(500);  // Allow sensors to stabilize
}

void loop() {
  
  // Read temperatures from thermocouples
  double temp1 = thermocouple1.readCelsius();
  double temp2 = thermocouple2.readCelsius();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WIFI NOT CONNECTED");
    scrollText("WIFI NOT CONNECTED ", display1);
    WiFi.begin(ssid, password);
  }

  // Display temperatures
  if (15.0 <= temp1 && temp1 <= 400.0) {
    display1.showNumberDec((int)temp1, false);
  } else {
    display1.setSegments(data);
  }

  if (15.0 <= temp2 && temp2 <= 400.0) {
    display2.showNumberDec((int)temp2, false);
  } else {
    display2.setSegments(data);
  }

  // Debugging purposes
  Serial.print("Temp 1: ");
  Serial.print(temp1);
  Serial.println(" *C");
  Serial.print("Temp 2: ");
  Serial.print(temp2);
  Serial.println(" *C");

  // Read gas value from MQ2 sensor
  int mq2_value = analogRead(MQ2_PIN);
  Serial.print("MQ2 Sensor Value: ");
  Serial.println(mq2_value);

  if (temp1 > 0 && temp1 < 1000) {
    Serial.println("1 ok");
  } else {
    temp1 = -1;
  }

  if (temp2 > 0 && temp2 < 1000) {
    Serial.println("2 ok");
  } else {
    temp2 = -1;
  }
  if (temp1 > 40 || temp2 > 40) {



    // Send data to Google Sheets
    Data_to_Sheets(No_of_Parameters, temp1, temp2, mq2_value, 0);
  }

  // Check if temperature exceeds 80°C
  if (temp1 > 80 || temp2 > 80) {
    // Make the buzzer beep
    digitalWrite(BUZZER_PIN, HIGH);  // Beep
    delay(2000);                     // Beep for 2 seconds
    digitalWrite(BUZZER_PIN, LOW);   // Turn off buzzer
  }

  delay(5000);  // Update every 5 seconds
}

// Function to scroll text across the 4-digit display
void scrollText(String text, TM1637Display& display) {
  const uint8_t alphabets[] = {
    0b00111111,  // 0
    0b00000110,  // 1
    0b01011011,  // 2
    0b01001111,  // 3
    0b01100110,  // 4
    0b01101101,  // 5
    0b01111101,  // 6
    0b00000111,  // 7
    0b01111111,  // 8
    0b01101111,  // 9
    0b01110111,  // A
    0b01111100,  // b
    0b00111001,  // C
    0b01011110,  // d
    0b01111001,  // E
    0b01110001,  // F
    0b00111101,  // G
    0b01110110,  // H
    0b00110000,  // I
    0b00011110,  // J
    0b01110110,  // K
    0b00111000,  // L
    0b00010101,  // M (approximation)
    0b01010100,  // N (approximation)
    0b00111111,  // O
    0b01110011,  // P
    0b01101011,  // Q (approximation)
    0b01010000,  // R (approximation)
    0b01101101,  // S
    0b01111000,  // T
    0b00111110,  // U
    0b00111110,  // V (same as U, limited by display)
    0b00101010,  // W (approximation)
    0b01110110,  // X
    0b01101110,  // Y
    0b01011011,  // Z
    0b00000000,  // Space
    0b00001000   // _ (underscore)
  };

  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    uint8_t segments[4] = { 0, 0, 0, 0 };

    // Shift text to the left, displaying 4 characters at a time
    for (int j = 0; j < 4 && (i + j) < text.length(); j++) {
      char nextChar = text.charAt(i + j);

      // Map the character to a 7-segment display pattern
      if (nextChar >= '0' && nextChar <= '9') {
        segments[j] = alphabets[nextChar - '0'];
      } else if (nextChar >= 'A' && nextChar <= 'Z') {
        segments[j] = alphabets[nextChar - 'A' + 10];  // Map letters A-Z
      } else if (nextChar == '_') {
        segments[j] = alphabets[37];  // Underscore
      } else if (nextChar == ' ') {
        segments[j] = alphabets[36];  // Space
      } else {
        segments[j] = 0;  // Unsupported character
      }
    }

    // Display the segments on the TM1637
    display.setSegments(segments);
    delay(300);  // Adjust delay for scrolling speed
  }
}
