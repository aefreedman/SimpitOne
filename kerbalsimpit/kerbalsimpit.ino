//Sample using LiquidCrystal library
#include <LiquidCrystal.h>

#include <KerbalSimpit.h>
#include <PayloadStructs.h>
// #include "libraries/MaxUtil.h"
#include <LedControl.h>

// using namespace MaxUtil;

// LedControl
//                        MOSI              MISO              SCK             SS(Slave)   SS(Master)
// Mega1280 or Mega2560	  51 or ICSP-4	    50 or ICSP-1	  52 or ICSP-3	  53	        -
// Uno/ProMini         	  11 or ICSP-4	    12 or ICSP-1	  13 or ICSP-3	  53	        -

// MAX7219 digit indices are (left-to-right) 87654321
// LedControl readjusts the idices to be 0-indexed (76543210)
// MAX7219 Defines
#define MAX_CS_PIN 6
#define NUM_DISPLAYS 1

LedControl lc = LedControl(MAX_CS_PIN, NUM_DISPLAYS);

KerbalSimpit mySimpit(Serial);

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcd_key = 0;
int adc_key_in = 0;
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

// read the buttons
int read_LCD_buttons()
{
    adc_key_in = analogRead(0); // read the value from the sensor
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    if (adc_key_in > 1000)
        return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
    // For V1.1 us this threshold
    // if (adc_key_in < 50)   return btnRIGHT;
    // if (adc_key_in < 250)  return btnUP;
    // if (adc_key_in < 450)  return btnDOWN;
    // if (adc_key_in < 650)  return btnLEFT;
    // if (adc_key_in < 850)  return btnSELECT;

    Serial.print(lcd_key);
    Serial.print("\n");

    // For V1.0 comment the other threshold and use the one below:

    if (adc_key_in < 50)
        return btnRIGHT;
    if (adc_key_in < 195)
        return btnUP;
    if (adc_key_in < 380)
        return btnDOWN;
    if (adc_key_in < 555)
        return btnLEFT;
    if (adc_key_in < 790)
        return btnSELECT;

    return btnNONE; // when all others fail, return this...
}

void setup()
{
    for (int i = 0; i < NUM_DISPLAYS; i++)
    {
        lc.shutdown(i, false);
        lc.clearDisplay(i);
        lc.setIntensity(i, 8);
    }
    // char test[] = {' ', ' ', '-', '4', 'a', '6', '7', '8'};
    char test[] = {'a', 'r', 'd', 'u', 'i', 'n', 'o', ' '};
    displayChar(0, test);

    Serial.begin(115200);
    while (!mySimpit.init())
        ;

    lc.clearDisplay(0);

    lcd.begin(16, 2); // start the library
    lcd.setCursor(0, 0);
    lcd.print("Kerbal Simpit init"); // print a simple message
    lcd.clear();

    mySimpit.inboundHandler(messageHandler);
    mySimpit.registerChannel(ALTITUDE_MESSAGE);
    mySimpit.registerChannel(VELOCITY_MESSAGE);

    // LEDCONROL MAX7219

}

void messageHandler(byte messageType, byte message[], byte messageSize)
{
    switch (messageType)
    {
    case ALTITUDE_MESSAGE:
    {
        if (messageSize == sizeof(altitudeMessage))
        {
            altitudeMessage myAltitude;
            myAltitude = parseAltitude(message);
            lcd.setCursor(0, 0);
            lcd.print("ALT ");
            lcd.setCursor(4, 0);
            //float seaLevel = myAltitude.sealevel;
            int trimmedAlt = (int)myAltitude.sealevel;
            lcd.print(myAltitude.sealevel);
            String altString = "";
            altString += trimmedAlt;
            displayChar(0, altString);
        }
        break;
    }

    case VELOCITY_MESSAGE:
    {
        if (messageSize == sizeof(velocityMessage))
        {
            velocityMessage myVel;
            myVel = parseVelocity(message);
            lcd.setCursor(0, 1);
            lcd.print("VO ");
            lcd.setCursor(3, 1);
            lcd.print(myVel.orbital);
        }
        break;
    }
    }
}

void loop()
{
    mySimpit.update();
}

// LED

void displayNumber(int addr, byte data[])
{
    lc.clearDisplay(addr);
    for (int i = 0; i < 8; i++)
    {
        lc.setDigit(addr, i, data[i], false);
    }
}

void displayNumber(int addr, long value)
{
    byte testDisplay[8];
    convertToDigits(testDisplay, value);
    displayNumber(addr, testDisplay);
}

void displayChar(int addr, String data)
{
    lc.clearDisplay(addr);
    int maxLength = 8;
    int length = data.length();

    if (length > maxLength)
    {
        // do something
        return;
    }

    char charArray[length];
    data.toCharArray(charArray, length);
    // strncpy(charArray, data.c_str(), stringLength);

    int firstIndex = 7 - length;
    for (int digit = 0; digit < length; digit++)
    {
        lc.setChar(addr, digit, data[length - 1 - digit], false);
    }
}

// Simple method for displaying char[8] arrays
void displayChar(int addr, char data[])
{
    for (int digit = 0; digit < 8; digit++)
    {
        lc.setChar(addr, 7 - digit, data[digit], false);
    }
}

void convertToDigits(byte output[], unsigned long value)
{
    unsigned long digitsCounter = value;

    for (int i = 0; i < 8; i++)
    {
        output[i] = digitsCounter % 10;

        // if (digitsCounter < pow(10, i)) // 1, 10, 100, 1000....
        //     output[i] = 0x0F;          // set the data to something out of range so the chip doesn't display anything on that digit -- set to 0x00 to pad with 0s (or any other valid value)

        digitsCounter /= 10;
    }
}