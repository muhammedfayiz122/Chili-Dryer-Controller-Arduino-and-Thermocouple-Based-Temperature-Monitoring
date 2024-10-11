#include <TM1637Display.h>
#include <max6675.h>
#include <WiFi.h>
#include "TRIGGER_GOOGLESHEETS.h"

// Wi-Fi credentials
const char* ssid = "hu";
const char* password = "fayiz@1122";

// Define pins for TM1637 Displays : 
//display-1
#define DISPLAY1_CLK 6
#define DISPLAY1_DIO 7

//display-2
#define DISPLAY2_CLK 8
#define DISPLAY2_DIO 9

// Define pins for MAX6675 Modules :
//temp sensor - 1
#define MAX6675_1_SO 10
#define MAX6675_1_CS 11 //white
#define MAX6675_1_SCK 12


//temp sensor - 2
#define MAX6675_2_SO 13
#define MAX6675_2_CS 14
#define MAX6675_2_SCK 15

// Initialize MAX6675 Modules
MAX6675 thermocouple1(MAX6675_1_SCK, MAX6675_1_CS, MAX6675_1_SO);
MAX6675 thermocouple2(MAX6675_2_SCK, MAX6675_2_CS, MAX6675_2_SO);

// Initialize TM1637 Displays
TM1637Display display1(DISPLAY1_CLK, DISPLAY1_DIO);
TM1637Display display2(DISPLAY2_CLK, DISPLAY2_DIO);

/********** Google Sheets Definitions ***********/
char column_name_in_sheets[ ][6] = {"temp1", "temp2", "v3", "v4"};
String Sheets_GAS_ID = "AKfycbyZauZK4WTLWMsuvJnpGlFA-N9OOMmdF7vsj-wYsUKoThV07JQ8BW9iQxdO9vM1AQ8eNw";
int No_of_Parameters = 4;
/*********************************************/

void setup() {
  Serial.begin(115200);

  display1.setBrightness(7);
  display2.setBrightness(0x0f);

  Serial.println("Initializing...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters);

  delay(500); // Allow sensors to stabilize

}

void loop() {
  // Read temperatures from thermocouples
  double temp1 = thermocouple1.readCelsius();
  double temp2 = thermocouple2.readCelsius();

  // Display temperatures
  display1.showNumberDec(int(temp1));             
  display2.showNumberDec((int)temp2, false);

  // Debugging purposes
  Serial.print("Temp 1: "); Serial.print(temp1); Serial.println(" *C");
  Serial.print("Temp 2: "); Serial.print(temp2); Serial.println(" *C");

  // Send data to Google Sheets
  Data_to_Sheets(No_of_Parameters, temp1, temp2, 0, 0);

  delay(5000); // Update every 1 second
}

