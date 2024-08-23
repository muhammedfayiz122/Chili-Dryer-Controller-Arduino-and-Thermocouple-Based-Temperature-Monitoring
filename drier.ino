#include <TM1637Display.h>
#include <max6675.h>

// Define pins for MAX6675 Modules
#define MAX6675_1_CS 14 //D5
#define MAX6675_1_SCK 12 //D6
#define MAX6675_1_SO 13 //D7

#define MAX6675_2_CS 5 //D1
#define MAX6675_2_SCK 4 //D2
#define MAX6675_2_SO 0 //D3

// Define pins for TM1637 Displays
#define DISPLAY1_DIO 15 //D8
#define DISPLAY1_CLK 16 //D4

#define DISPLAY2_DIO 2 //D0
#define DISPLAY2_CLK 10 //D4

// Initialize MAX6675 Modules
MAX6675 thermocouple1(MAX6675_1_SCK, MAX6675_1_CS, MAX6675_1_SO);
MAX6675 thermocouple2(MAX6675_2_SCK, MAX6675_2_CS, MAX6675_2_SO);

// Initialize TM1637 Displays
// TM1637Display display1(DISPLAY1_CLK, DISPLAY1_DIO);
TM1637Display display1 = TM1637Display(DISPLAY1_CLK, DISPLAY1_DIO);
TM1637Display display2(DISPLAY2_CLK, DISPLAY2_DIO);

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  display1.setBrightness(7); // Set brightness (0x00 to 0x0f)
  display2.setBrightness(0x0f); // Set brightness (0x00 to 0x0f)

  delay(500); // Allow sensors to stabilize
}

void loop() {
  // Read temperatures from thermocouples
  double temp1 = thermocouple1.readCelsius();
  double temp2 = thermocouple2.readCelsius();

  // Display temperatures on respective displays
  // display1.showNumberDec((int)temp1, false); // Display temperature on display 1
  display1.showNumberDec(int(temp1));             
  display2.showNumberDec((int)temp2, false); // Display temperature on display 2

  // For debugging purposes
  Serial.print("Temp 1: "); Serial.print(int(temp1)); Serial.println(" *C");
  Serial.print("Temp 2: "); Serial.print(temp2); Serial.println(" *C");

  delay(1000); // Update every 1 second
}