#include <TM1637Display.h>
#include <max6675.h>

// Define pins for MAX6675 Modules
#define MAX6675_1_CS 11 //D5
#define MAX6675_1_SCK 10 //D6
#define MAX6675_1_SO 12  //D7

#define MAX6675_2_CS 4  //D1
#define MAX6675_2_SCK 3  //D2
#define MAX6675_2_SO 5  //D3

// Define pins for TM1637 Displays
#define DISPLAY1_DIO 7 //D8
#define DISPLAY1_CLK 6  //D4

#define DISPLAY2_DIO 9 //D0
#define DISPLAY2_CLK 8 //SDD3

// Define pin for Buzzer
#define BUZZER_PIN 13 // SDD2

// Initialize MAX6675 Modules
MAX6675 thermocouple1(MAX6675_1_SCK, MAX6675_1_CS, MAX6675_1_SO);
MAX6675 thermocouple2(MAX6675_2_SCK, MAX6675_2_CS, MAX6675_2_SO);

// Initialize TM1637 Displays
TM1637Display display1(DISPLAY1_CLK, DISPLAY1_DIO);
TM1637Display display2(DISPLAY2_CLK, DISPLAY2_DIO);

uint8_t data[] = {0x79, 0x50, 0x50, 0x00};

int led_blinker = 0;

void setup() {
  Serial.begin(115200);

  display1.setBrightness(0x0f); // Set brightness (0x00 to 0x0f)
  display2.setBrightness(0x0f); // Set brightness (0x00 to 0x0f)

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Buzzer off initially

  delay(500); // Allow sensors to stabilize
}

void loop() {
  // Read temperatures from thermocouples
  double temp1 = thermocouple1.readCelsius();
  double temp2 = thermocouple2.readCelsius();

  // For debugging purposes
  Serial.print("Temp 1: "); Serial.print(temp1); Serial.println(" *C");
  Serial.print("Temp 2: "); Serial.print(temp2); Serial.println(" *C");

  // Check if temperatures are out of range
  if (15.0 <= temp1 && temp1 <= 400.0) {
    display1.showNumberDec((int)temp1, false);
  }
  else {
    display1.setSegments(data);
  }

  // Check if temperatures are out of range
  if (15.0 <= temp2 && temp2 <= 400.0) {
    display2.showNumberDec((int)temp2, false);
  }
  else {
    display2.setSegments(data);
  }
  // temp1 = 80;
  
  // Check if any temperature exceeds 75°C
  if (temp1 > 75.0 || temp2 > 75.0) {
    if (led_blinker%2 == 0) {
      digitalWrite(BUZZER_PIN, LOW);
      led_blinker = 1 ;
      }
    else{
      digitalWrite(BUZZER_PIN, HIGH);
      led_blinker++;
    }
    // digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer
  } else {
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
  }

  delay(1000); // Update every 1 second
}
