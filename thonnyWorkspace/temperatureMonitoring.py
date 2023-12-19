from digitalGPIO import digitalWrite, digitalRead
import i2c_ssd1306 as ssd1306
from ntc_module import ntc_5k
from time import sleep_ms

# GPIO Number
ntc_channel = 6
led_pin = 12
pb_pin = 35

# SSD1306 Pin
ssd1306_SDA = 21
ssd1306_SCL = 22

class roomMonitoring:
    def __init__(self, _ntc_channel, _led_pin, _pb_pin, _ssd1306_SDA=21, _ssd1306_SCL=22):
        self.ntc_channel = _ntc_channel
        self.led_pin = _led_pin
        self.pb_pin = _pb_pin
        self.ssd1306_SDA = _ssd1306_SDA
        self.ssd1306_SCL = _ssd1306_SCL
        
        # Variables
        self.ledState = False
        self.pbState = None
        self.roomTemp = 0
        self.lampText = "Off"

    def readTemperature(self):
        self.roomTemp = ntc_5k(self.ntc_channel)
    
    def initDisplay(self):
        ssd1306.initDisplay(self.ssd1306_SDA, self.ssd1306_SCL)
        
    def beginMonitoring(self):
        self.readTemperature()
        _data = f"Temperature {self.roomTemp} C"
        ssd1306.drawString(5, 10, 12, _data)
        sleep_ms(100)
        
    def controlLamp(self):
        self.pbState = digitalRead(self.pb_pin)
        
        if self.pbState == False and self.ledState == False:
            self.ledState = True
            self.lampText = "On"
        elif self.pbState == False and self.ledState == True:
            self.ledState = False
            self.lampText = "Off"
            
        digitalWrite(self.led_pin, self.ledState)
        ssd1306.drawString(5, 30, 12, f"Lamp is {self.lampText}")
            
monitorTemperature = roomMonitoring(ntc_channel, led_pin, pb_pin, ssd1306_SDA, ssd1306_SCL)
monitorTemperature.initDisplay()
sleep_ms(1000)
ssd1306.clearScreen()

while True:
    monitorTemperature.beginMonitoring()
    monitorTemperature.controlLamp()