# ::::::MQTT Over GPRS -v1.0::::: #

* **First commit**: 16/Aug/2020
* **Author**: Rodolpho Gianini
* **Notes:**
  * First release
  * Designed to work with the GSM SIM800L.
  * This library uses Serial #2 from ESP32 ("SIM800L.h" by default pins are RX=16 and TX=17)
  * RESET_PIN defined but NOT tested.
  
 
**DEFAULT PINOUT**:

* 
 	      ____________________________
 	     |  ESP32      >>>   SIM800L  |
 	      ----------------------------
          GND           >>>   GND
          RX  RX2       >>>   TX
          TX  TX2       >>>   RX
          RESET ?       >>>   RST
