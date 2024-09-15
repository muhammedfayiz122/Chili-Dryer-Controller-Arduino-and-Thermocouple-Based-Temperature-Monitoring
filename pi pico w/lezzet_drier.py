import time
import machine
from machine import Pin, SPI
import network
import urequests
import tm1637

# Wi-Fi credentials
ssid = "hu"
password = "fayiz@1122"

# Define pins for MAX6675 Modules
MAX6675_1_CS = 14
MAX6675_1_SCK = 12
MAX6675_1_SO = 13

MAX6675_2_CS = 5
MAX6675_2_SCK = 4
MAX6675_2_SO = 0

# Define pins for TM1637 Displays
DISPLAY1_DIO = 15
DISPLAY1_CLK = 16
DISPLAY2_DIO = 2
DISPLAY2_CLK = 10

# Initialize MAX6675 Modules
class MAX6675:
    def __init__(self, sck_pin, cs_pin, so_pin):
        self.cs = Pin(cs_pin, Pin.OUT)
        self.cs.on()
        self.spi = SPI(1, baudrate=500000, polarity=0, phase=0, sck=Pin(sck_pin), mosi=Pin(so_pin), miso=Pin(so_pin))

    def read(self):
        self.cs.off()
        value = self.spi.read(2)
        self.cs.on()

        temp = (value[0] << 8 | value[1]) >> 3
        if temp == 0xFFFF:
            return None
        return temp * 0.25

thermocouple1 = MAX6675(MAX6675_1_SCK, MAX6675_1_CS, MAX6675_1_SO)
thermocouple2 = MAX6675(MAX6675_2_SCK, MAX6675_2_CS, MAX6675_2_SO)

# Initialize TM1637 Displays
display1 = tm1637.TM1637(clk=Pin(DISPLAY1_CLK), dio=Pin(DISPLAY1_DIO))
display2 = tm1637.TM1637(clk=Pin(DISPLAY2_CLK), dio=Pin(DISPLAY2_DIO))

# Your Google Script URL (replace this with the actual URL from your Apps Script)
script_url = "https://script.google.com/macros/s/YOUR_SCRIPT_ID/exec"

# Connect to Wi-Fi
def connect_to_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, password)
    
    while not wlan.isconnected():
        time.sleep(0.5)
        print("Connecting to WiFi...")
    
    print("Connected to WiFi", wlan.ifconfig())

# Send data to Google Sheets
def send_to_google_sheets(temp1, temp2):
    try:
        url = f"https://script.google.com/macros/s/AKfycbyZauZK4WTLWMsuvJnpGlFA-N9OOMmdF7vsj-wYsUKoThV07JQ8BW9iQxdO9vM1AQ8eNw/exec?temp1={temp1}&temp2={temp2}"
        response = urequests.get(url)
        print("Data sent to Google Sheets:", response.text)
        response.close()
    except Exception as e:
        print("Failed to send data:", e)

def main():
    connect_to_wifi()

    while True:
        # Read temperatures from thermocouples
        temp1 = thermocouple1.read()
        temp2 = thermocouple2.read()

        if temp1 is not None:
            display1.number(int(temp1))
            print(f"Temp 1: {temp1:.2f} C")
        else:
            display1.show("Err")

        if temp2 is not None:
            display2.number(int(temp2))
            print(f"Temp 2: {temp2:.2f} C")
        else:
            display2.show("Err")

        # Send temperature data to Google Sheets
        send_to_google_sheets(temp1, temp2)

        time.sleep(10)  # Update every 10 seconds

# Run the main loop
main()
