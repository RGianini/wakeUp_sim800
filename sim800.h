/*
 *  ::::::MQTT Over GPRS -v1.0::::::
 *
 *  Date: 16/Aug/2020
 *  Author: Rodolpho Gianini
 *  Notes:
 *        * First release
 *        * Designed to work with the GSM SIM800L.
 *        * This library uses Serial #2 from ESP32 ("SIM800L.h" by default pins are RX=16 and TX=17)
 *        * RESET_PIN defined but NOT tested.
 *        * APN: m2m.arqia.br
 * 
 *
 *      DEFAULT PINOUT:
 *	      ____________________________
 *	     |  ESP32      >>>   SIM800L  |
 *	      ----------------------------
 *        GND          >>>   GND
 *        RX  RX2       >>>   TX
 *        TX  TX2       >>>   RX
 *       RESET ?       >>>   RST
*/


#ifndef SIM800Lhttp_h
	#define SIM800Lhttp_h
	#include "Arduino.h"


	#define IS_DEBUG_ENABLED        0            // Used for printing messages to console; 0 = disabled, 1 = enabled

	#define DEFAULT_RX_PIN 		    16
	#define DEFAULT_TX_PIN 		    17
	#define DEFAULT_RESET_PIN 	    2		

	#define DEFAULT_LED_FLAG	    false 	     // true: use led.	 false: don't user led.
	#define DEFAULT_LED_PIN 	    13 		     // pin to indicate states.

	#define BUFFER_RESERVE_MEMORY	255
	#define DEFAULT_BAUD_RATE		4800
	#define TIME_OUT_READ_SERIAL	5000
	

	class SIM800L : public HardwareSerial
	{
		private:

			uint32_t _baud;
			int _timeout;
			int cont = 0;
			String _buffer;
			String _readSerial();
			String _readSerial(uint32_t timeout);

		public:

			uint8_t	RX_PIN;
			uint8_t TX_PIN;
			uint8_t RESET_PIN;
			uint8_t LED_PIN;
			bool	LED_FLAG;
			const byte numChars = 64;
			char endMarker = '\n';
			char receivedChars[64];

			SIM800L(uint8_t);

			void begin();					//Default baud 9600
			void begin(uint32_t baud);
			void reset();

			bool CheckModule();
			bool CheckAttach();
			bool ConnSetup();
			bool ConnSetupHttp(String server, String apn);
			uint32_t HttpPost(String msg, String server, String apn, String & StrRead, bool & flagOK);//uint32_t HttpPost(String msg, String server, String apn);
			String HttpGet();
						
			void printDebugMessage(String message);
			void delayMs(int delayInMilliseconds);
			String sendAtCommand(String command, int delayInMilliseconds);
			
			bool setPIN(String pin);
			String GetDevInfo();

			String signalQuality(uint16_t & powerLvl);
			void ResetModule();

	};
#endif