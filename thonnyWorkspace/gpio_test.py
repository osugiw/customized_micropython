import machine
from machine import Pin

gp = Pin(12, Pin.OUT)
gp(1)