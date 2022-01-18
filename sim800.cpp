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
 * 
 *
 *      DEFAULT PINOUT:
 *	      ____________________________
 *	     |  ESP32      >>>   SIM800L  |
 *	      ----------------------------
 *        GND           >>>   GND
 *        RX  RX2       >>>   TX
 *        TX  TX2       >>>   RX
 *        RESET ?       >>>   RST
*/

#include "Arduino.h"
#include "SIM800Lhttp.h"


SIM800L::SIM800L(uint8_t num) : HardwareSerial(num)
{
	RX_PIN 		= 16;
	TX_PIN 		= 17;
	RESET_PIN 	= DEFAULT_RESET_PIN;
}

void SIM800L::begin(uint32_t baud)
{
	_baud = baud;
	this->HardwareSerial::begin(_baud, SERIAL_8N1, RX_PIN, TX_PIN, false);

	_buffer.reserve(BUFFER_RESERVE_MEMORY); // Reserve memory to prevent intern fragmention
	
	SIM800L::CheckModule();
	pinMode(RESET_PIN, OUTPUT);
	digitalWrite(RESET_PIN, HIGH);
}

void SIM800L::printDebugMessage(String message) {
	if (IS_DEBUG_ENABLED == 1) {
		Serial.println(message);
	}
}

void SIM800L::delayMs(int delayInMilliseconds){
	int Naft=0, Nnow=0;
	vTaskDelay( delayInMilliseconds / portTICK_PERIOD_MS);
}

String SIM800L::sendAtCommand(String command, int delayInMilliseconds) {

	String Str;
	if (IS_DEBUG_ENABLED == 1) {
		Serial.print("    Executing AT command: ");
		SIM800L::printDebugMessage(command);
	}

	SIM800L::println(command);
	
	SIM800L::delayMs(delayInMilliseconds);
	
	Str = SIM800L::_readSerial();
	
	if (IS_DEBUG_ENABLED == 1) {
		Serial.print("      -> Response: ");
		SIM800L::printDebugMessage(Str);
	}
	
	return Str;
};

bool SIM800L::CheckModule()
{
	String StrAux;
	
	SIM800L::printDebugMessage("Checking if GPRS module is responding...");
	StrAux = SIM800L::sendAtCommand("AT", 500);


	if(StrAux.indexOf("OK") != -1)
	{
		SIM800L::printDebugMessage("    Comm >>  OK");
		if (IS_DEBUG_ENABLED == 1) {
			SIM800L::GetDevInfo();
		}
		return true;
	}
	else
	{
		SIM800L::printDebugMessage("    Comm >> NOK");
		return false;
	}
}

bool SIM800L::ConnSetupHttp(String server, String apn)
{

	String StrAux;
	SIM800L::sendAtCommand("AT+CMEE=2", 4000);
	SIM800L::sendAtCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", 4000);
	SIM800L::sendAtCommand("AT+SAPBR=3,1,\"APN\",\""+apn+"\"", 4000);
	
	StrAux = "";
    while (StrAux.indexOf("OK") == -1)	{
		StrAux = SIM800L::sendAtCommand("AT+SAPBR=1,1", 6000);
		cont++;
		if(cont >= 5){
			//SIM800L::ResetModule();
			//cont = 0;
			//StrAux = "";
			//return false;
		}

		if (cont == 4){
			SIM800L::sendAtCommand("AT+SAPBR=0,1", 4000);
			StrAux = "";
			cont = 0;
		}
	}
	
	StrAux=SIM800L::sendAtCommand("AT+SAPBR=2,1", 4000);
	if(StrAux.indexOf("OK") != -1){
		if (IS_DEBUG_ENABLED == 1) {
			Serial.println("GSM Connection init sucess");
		}
		return true;
		
	}

	if (IS_DEBUG_ENABLED == 1) {
		Serial.println("GSM Connection init failure");
	}
	return false;
}

uint32_t SIM800L::HttpPost(String msg, String server, String apn, String & StrRead, bool & flagOK)
{
	String StrAux; 

	StrAux = SIM800L::sendAtCommand("AT+HTTPINIT", 4000);
	if(StrAux.indexOf("OK") == -1){
		SIM800L::sendAtCommand("AT+HTTPTERM", 4000);
		StrAux = SIM800L::sendAtCommand("AT+HTTPINIT", 4000);
		if(StrAux.indexOf("OK") == -1){
			flagOK=false;
		}
		while(!flagOK){
			SIM800L::ConnSetupHttp(server, apn);
			SIM800L::sendAtCommand("AT+HTTPTERM", 4000);
			StrAux = SIM800L::sendAtCommand("AT+HTTPINIT", 4000);
			if(StrAux.indexOf("OK") != -1){
				flagOK=true;
			}	
		}
	}
	
	SIM800L::sendAtCommand("AT+HTTPPARA=\"CID\",1", 4000);
	SIM800L::sendAtCommand("AT+HTTPPARA=\"URL\",\""+server+"\"", 4000);
	SIM800L::sendAtCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", 4000);

	SIM800L::sendAtCommand("AT+HTTPDATA=" + String(msg.length()) + ",10000", 4000);//100000
	SIM800L::sendAtCommand(msg,4000);
	SIM800L::sendAtCommand("AT+HTTPACTION=1",4000);

	StrAux = SIM800L::sendAtCommand("AT+HTTPREAD",4000);

	StrRead = StrAux;

	SIM800L::sendAtCommand("AT+HTTPTERM",4000);

	return StrAux.length();
}


String SIM800L::HttpGet()
{
	String StrAux;
	
	SIM800L::sendAtCommand("AT+HTTPACTION = 0", 5000);
	StrAux = SIM800L::sendAtCommand("AT+HTTPREAD", 5000);
	SIM800L::sendAtCommand("AT+HTTPTERM",4000);
	
	return StrAux;
}

String SIM800L::GetDevInfo()
{
	String StrAux;
	
	if (IS_DEBUG_ENABLED == 1) {
		SIM800L::printDebugMessage("Modem firmware version");
	}
	
	StrAux = SIM800L::sendAtCommand("AT+GMR", 500);
	
	return StrAux;
}

String SIM800L::signalQuality(uint16_t & powerLvl)
{
	String sigStren="";
	String  pch="";
    int16_t sigVal=0;
	int16_t val=0;
	int16_t rssi=0;
	
	if (IS_DEBUG_ENABLED == 1)
	{
		SIM800L::printDebugMessage("Signal Strength");
	}
	
	SIM800L::println("AT+CSQ");
	SIM800L::delayMs(3000);
	sigStren = SIM800L::printSerialData();
	
	if (IS_DEBUG_ENABLED == 1)
	{
    	Serial.println(sigStren);
    }

	int sigStrenFirst = sigStren.indexOf(":"); // encontrar o char ':' na resposta
	int sigStrenSec = sigStren.indexOf(',', sigStrenFirst+1); // econtrar a primeira ',' na resposta 
	pch = sigStren.substring(sigStrenFirst+2,sigStrenSec); // a intensidade do sinal do sinal esta entre os dois caracteres acima

	val = pch.toInt(); //Converte para int
	
	if (val < 10){
		powerLvl = 0;
	}else if (val < 15){
		powerLvl = 1;
	}else{
		powerLvl = 2;
	}
	Serial.println(powerLvl);

	rssi = 2*val-113;  //convertendo para dBm
	sigStren = String(rssi); // retorna pra String

    return sigStren;
}

void SIM800L::ResetModule() {

	if (IS_DEBUG_ENABLED == 1) {
		Serial.println("Resetando módulo");
	}

	digitalWrite(RESET_PIN, LOW);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	digitalWrite(RESET_PIN, HIGH);
}

//private
String SIM800L::_readSerial()
{
	String str;
	while (SIM800L::available())
    {
		if (SIM800L::available()>0)
        {
			str += (char) SIM800L::read();
        }
    }

	return str;
}