#include <Arduino.h>
#include <TinyGPSPlus.h>

TinyGPSPlus gps;

/**		GPS data structure.
		Separate rounding functions may be needed.
		Uncomment this to use in code or for reference of output types.
		///////////////////////////////////////////////////////////////
struct GPS_package{
	double latitude;
	double longitude;
	double altitude;

	uint8_t month;
	uint8_t day;
	uint16_t year;

	uint8_t hour;
	uint8_t minute;
	uint8_t second;
};
**/
void GPS_logic() {	//currently only prints out value, modify to return values if needed
	while (Serial1.available() > 0) {
		if (gps.encode(Serial1.read())) {	//Serial1 is the default for UART (unless different on esp32)
			Serial.print(F("Location: "));
			if (gps.location.isValid()) {	//outputs Lat/Long/Alt.
				Serial.print(gps.location.lat(), 6);
				Serial.print(F(","));
				Serial.print(gps.location.lng(), 6);
				Serial.print(F(","));
				Serial.print(gps.altitude.meters(), 3);
			} else {
				Serial.print(F("INVALID"));
			}

			Serial.print(F("  Date: "));	//outputs date in MM/DD/YYYY
			if (gps.date.isValid()) {
				Serial.print(gps.date.month());
				Serial.print(F("/"));
				Serial.print(gps.date.day());
				Serial.print(F("/"));
				Serial.print(gps.date.year());
			} else {
				Serial.print(F("INVALID"));
			}

			Serial.print(F("  Time: "));	//outputs time in HH:MM:SS
			if (gps.time.isValid()) {
				if (gps.time.hour() < 10) Serial.print(F("0"));
				Serial.print(gps.time.hour());
				Serial.print(F(":"));
				if (gps.time.minute() < 10) Serial.print(F("0"));
				Serial.print(gps.time.minute());
				Serial.print(F(":"));
				if (gps.time.second() < 10) Serial.print(F("0"));
				Serial.print(gps.time.second());
			} else {
				Serial.print(F("INVALID"));
			}

			Serial.println();
		}
	}

	if (millis() > 5000 && gps.charsProcessed() < 10) {  //debugging purposes
		Serial.println(F("GPS not detected."));
		while (true);
	}
}
