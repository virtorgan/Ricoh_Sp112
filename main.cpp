#include <Arduino.h>
#include <wire.h>

unsigned int eeprom = 0x53;

//i2c bus speed 400khz
unsigned int clock = 400000L;

void initialize(void) {

	Serial.begin(9600);
	Serial.println("Start...");
	Wire.begin();
	Wire.setClock(clock);
	Serial.println("I2C bus initalized!");
}
void displayHelp()
{
  Serial.println(F("\nArduino Toner reset"));
  Serial.println(F("\tr = read entire eeprom"));
  Serial.println(F("\tw = reset bytes 16 to 126 to 0x00"));
  Serial.println();
}

char getCommand()
{
  char c = '\0';
  if (Serial.available())
  {
    c = Serial.read();
  }
  return c;
}

unsigned int setCurrentAddress(int eeprom, unsigned int address) {

	Wire.beginTransmission(eeprom);

	byte size = Wire.write(address);
	if (size == 0) {
		Serial.println("Failed to write address");
		return 10;
	}
	byte error = Wire.endTransmission(false);
	if (error == 0) {
		// Serial.println("tranmission success");
	} else if (error == 1) {
		Serial.println("data too long to fit in buffer");
	} else if (error == 2) {
		Serial.println("receive NAK when transmiting address");
	} else if (error == 3) {
		Serial.println("receive NAK when transmiting data");
	} else if (error == 4) {
		Serial.println("other error");
	} else {
		Serial.println("unknown error");
	}
}

unsigned int printCurrentAddress(int eeprom) {
	byte size = Wire.requestFrom(eeprom, 1, true);
	if (size == 0) {
	}
	if (Wire.available()) {
		byte rdata = Wire.read();
		Serial.print("0x");
		Serial.print(rdata, HEX);
		Serial.print(",");
		return 0;
	} else {
		Serial.println("no data available from device");
		return 1;
	}
}

unsigned int printRandomAddress(int eeprom, unsigned int address) {

	if (setCurrentAddress(eeprom, address) != 0) {
		Serial.println("failed to set current address");
		return 1;
	}
	delay(5); // wait 5 ms between write and read

	if (printCurrentAddress(eeprom) != 0) {
		Serial.println("failed to read current address");
		return 2;
	}
	return 0;
 }

// display the content of the eeprom
void eepromRead(unsigned int eeprom) {

	Serial.println("reading device ");

	unsigned int address;
	for (address = 0; address < 256; address++) {
		if (printRandomAddress(eeprom, address) != 0) {
			Serial.print("Read failed at ");
			Serial.print(address, HEX);
			Serial.println("!");
			break;
		}
	}
	Serial.println("read end.");
    displayHelp();
}

unsigned int randomWrite(int eeprom, unsigned int address, byte data) {

	Wire.beginTransmission(eeprom);

	byte size = Wire.write(address);
	if (size == 0) {
		Serial.println("Failed to write address");
		return 1;
	}
	size = Wire.write(data);
	if (size == 0) {
		Serial.println("Failed to write data");
		return 2;
	}

	byte error = Wire.endTransmission(true);
	if (error == 0) {
		// Serial.println("tranmission success");
	} else if (error == 1) {
		Serial.println("data too long to fit in buffer");
	} else if (error == 2) {
		Serial.println("receive NAK when transmiting address");
	} else if (error == 3) {
		Serial.println("receive NAK when transmiting data");
	} else if (error == 4) {
		Serial.println("other error");
	} else {
		Serial.println("unknown error");
	}

	delay(5); // wait 5 ms, a write cycle

	return error;
}

void eepromWrite(unsigned int eeprom) {

	Serial.print("writing device ");
	Serial.print(eeprom, HEX);
	Serial.println("");

	unsigned int address;
	for (address = 16; address < 127; address++) {
		if (randomWrite(eeprom, address, 0x00) != 0) {
			Serial.print("Write failed at ");
			Serial.print(address, HEX);
			Serial.println("!");
			break;
		}
	}
	Serial.println("write finished.");
    displayHelp();
}

void setup(void) {

    initialize();
	displayHelp();
}

void loop() {
char command = getCommand();
  switch (command)
  {
    case 'r':
      eepromRead(eeprom);
      break;
    case 'w':
      eepromWrite(eeprom);
      break;
  }
}
