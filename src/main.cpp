#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;       // hard-wired for UNO shields anyway.
#include <TouchScreen.h>

//ID=0x9341
constexpr int XP=6; 
constexpr int XM=A2;
constexpr int YP=A1;
constexpr int YM=7; 
constexpr int TS_LEFT = 151;
constexpr int TS_RT = 922;
constexpr int TS_TOP = 134;
constexpr int TS_BOT = 942;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 200
#define MAXPRESSURE 1000

uint8_t Orientation = 1;

#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define MATRIX_GREEN    0x6767

void throwSixDice(uint8_t count) {
    for (uint8_t i = 0; i <  random(10, 30); ++i) {
        tft.fillRect(1, 1, 158, 125, BLACK);
        tft.setCursor(8, 34);
        uint8_t throwResult = 0;
        for (uint8_t j = 0; j < count; ++j) {
            throwResult += (uint8_t) random(1, 6 + 1);
        }
        char str[3];
        sprintf(str, "%02d", throwResult);
        tft.print(str);
        delay(100);
    }
}

void throwTenDice() {
    for (uint8_t i = 0; i <  random(10, 30); ++i) {
        tft.fillRect(160, 1, 159, 238, BLACK);
        tft.setCursor(167, 76);
        char str[3];
        sprintf(str, "%02d", (uint8_t) random(1, 10 + 1));
        tft.print(str);
        delay(100);
    }
}

void updateDiceCount(uint8_t count) {
     tft.fillRect(1, 126, 158, 33, BLACK);
    uint8_t diceXDrawPos = 9;
    for (uint8_t i = 0; i < count; ++i) {
        tft.drawRect(diceXDrawPos, 135, 16, 16, MATRIX_GREEN);
        tft.fillRect(diceXDrawPos + 3, 138, 2, 2, MATRIX_GREEN);
        tft.fillRect(diceXDrawPos + 11, 138, 2, 2, MATRIX_GREEN);
        tft.fillRect(diceXDrawPos + 7, 142, 2, 2, MATRIX_GREEN);
        tft.fillRect(diceXDrawPos + 3, 146, 2, 2, MATRIX_GREEN);
        tft.fillRect(diceXDrawPos + 11, 146, 2, 2, MATRIX_GREEN);
        diceXDrawPos += 16 + 5; 
    }
}

void setup(void)
{
    randomSeed(analogRead(5));
    tft.reset();
    tft.begin(tft.readID());
    tft.setRotation(Orientation);
    tft.fillScreen(BLACK);
    // Serial.begin(9600);

    // Draw main boxes
    tft.drawFastHLine(0, 0, 319, MATRIX_GREEN);
    tft.drawFastHLine(0, 239, 319, MATRIX_GREEN);
    tft.drawFastVLine(0, 0, 239, MATRIX_GREEN);
    tft.drawFastVLine(319, 0, 239, MATRIX_GREEN);
    tft.drawFastVLine(159, 0, 239, MATRIX_GREEN);

    // Draw dice config box
    tft.drawFastHLine(0, 159, 159, MATRIX_GREEN);
    tft.drawFastVLine(79, 159, 80, MATRIX_GREEN);
    tft.setTextColor(MATRIX_GREEN);
    tft.setTextWrap(false);
    tft.setCursor(14, 164);
    tft.setTextSize(10);
    tft.print('+');
    tft.setCursor(94, 163);
    tft.print('-');
    updateDiceCount(1);

    // Draw initial values
    tft.setTextSize(13);
    tft.setCursor(8, 34);
    tft.write("00");
    tft.setCursor(167, 76);
    tft.write("00");
}

void loop()
{
    static uint8_t diceCounter = 1;
    uint16_t xpos, ypos;
    tp = ts.getPoint();

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
        xpos = map(tp.y, 134, 942, 0, tft.width());
        ypos = map(tp.x, 922, 151, 0, tft.height());
        
        // Serial.print(xpos);
        // Serial.print(", ");
        // Serial.println(ypos);

        if (xpos > 320 || ypos > 240) {
            return; // Input out of bounds
        }

        if (ypos >= 118) {  // Ten dice
            throwTenDice();
        } else if (xpos <= 205) {   // Six dice
            throwSixDice(diceCounter);
        } else if (ypos <= 49) {    // Add dice
            if (diceCounter < 7) {
                ++diceCounter;
                updateDiceCount(diceCounter);
            }
        } else { // Subtract dice
            if (diceCounter > 1) {
                --diceCounter;
                updateDiceCount(diceCounter);
            }
        }

        delay(500); // Debounce
    }
}
