#include "MaxUtil.h"
#include <LedControl.h>


namespace MaxUtil
{
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
        displayNumber(lc, addr, testDisplay);
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
}