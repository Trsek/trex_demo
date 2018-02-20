#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h> // Hardware-specific library

// SD card chip selected
#define SD_CHIP_SELECTED 10   // for device from www.mcufriend.com (IDF=0x154)

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
// for device from www.mcufriend.com (IDF=0x154)
#define LCD_CS A3    // Chip Select goes to Analog 3
#define LCD_CD A2    // Command/Data goes to Analog 2
#define LCD_WR A1    // LCD Write goes to Analog 1
#define LCD_RD A0    // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// These are the pins for the shield!
// for device from www.mcufriend.com (IDF=0x154)
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int cesta_h1;
int cesta_h2;
int cesta_d1;
int cesta_d2;
int step = 5;

void setup(void) 
{
  Serial.begin(38400, SERIAL_8N1);
  Serial.setTimeout(100);
  Serial.println(F("T-Rex. Software by Zdeno Sekerak (c) 2018."));

  cesta_h1 = 70;
  cesta_h2 = -70;
  cesta_d1 = 100;
  cesta_d2 = 220;

  tft.reset();
  tft.begin(tft.readID());
}
// -----------------------------------------------------------------------------

void MainKresli(void)
{
  tft.fillScreen(BLACK);
  tft.setRotation(3);

  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.setCursor(9, 3);
  tft.print(F("Speed: 100 km/h"));

  // copyright
  tft.setTextColor(BLUE);
  tft.setTextSize(1);
  tft.setCursor(2, 310);
  tft.print(F("Software by Zdeno Sekerak (c)2018, v0.1"));
//tft.fillRect(x, y, count * text_size * FONT_WIDTH, text_size * FONT_HEIGHT, BLACK);  
//tft.drawRect(0, LOG_Y, tft.width(), LOG_HEIGHT+2, WHITE);
}

void KresliTo(int stred, int zatoc, int vzork, int col)
{
  int y;
  float z, x,zat;

   x = 0;
   z = 0;
   zat = 1.0*zatoc/240;
   zatoc = abs(zatoc);

   for(int y=240; y>0; y--)
   {
      //if(z < zatoc) 
      z += zat;
      x += z/240;

      if(((y/vzork) % 2)
      || (vzork == 1))
         tft.drawPixel(stred + x, y, col);
   }
}
// -----------------------------------------------------------------------------

// the loop function runs over and over again forever
void loop()
{
  int i;

   MainKresli();
   for(i=-200; i<200; i=i+step)
   {
    KresliTo(cesta_d1, cesta_h1+i-step, 1, BLACK);
    KresliTo(cesta_d1, cesta_h1+i, 1, WHITE);

    KresliTo(cesta_d2, cesta_h2+i-step, 1, BLACK);
    KresliTo(cesta_d2, cesta_h2+i, 1, WHITE);
    
    KresliTo((cesta_d1 + cesta_d2)/2, (cesta_h1+i-step + cesta_h2+i-step)/2, 15, BLACK);
    KresliTo((cesta_d1 + cesta_d2)/2, (cesta_h1+i + cesta_h2+i)/2, 15, WHITE);
    
//    delay(100);
   }
}
