#include <stdint.h>
#include <EEPROM.h>
#include "TouchScreen.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library


#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define TS_MINX    160
#define TS_MINY    140
#define TS_MAXX    880
#define TS_MAXY    940
#define MIN_TOUCH  10


// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define GRAY    0x0101
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define FONT_WIDTH  12
#define FONT_HEIGHT 17

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

static int mapa[][3] = {
  { 100, 20, 64 },  
  { 500, 40, 72 },  
  { 700, 10, 80 },  
  { 450, 50, 70 },  
  { 600, 50, 70 },  
  { 250, 50, 70 },  
  { 450, 50, 70 },  
  {   0, 20, 64 },  
  {-300, 30, 56 },  
  {-600, 50, 64 },  
  {   0, 30, 56 },  
};

int cesta_h1;
int cesta_h2;
int cesta_d1;
int cesta_d2;

int imapa = 0;
int krok = 0;

float uhol = 0;
float uhol_krok = 0;
int rychl = 64;
int rychl_old = 0;
int step = 10;

void setup(void) 
{
  Serial.begin(9600, SERIAL_8N1);
  Serial.setTimeout(100);
  Serial.println(F("T-Rex. Software by Zdeno Sekerak (c) 2018."));

  cesta_h1 = 70;
  cesta_h2 = -70;
  cesta_d1 = 100;
  cesta_d2 = 220;

  tft.reset();
  tft.begin(tft.readID());
  MainKresli();
}
// -----------------------------------------------------------------------------

void MainKresli(void)
{
  tft.fillScreen(BLACK);
  tft.setRotation(3);

  // copyright
  tft.setTextColor(BLUE);
  tft.setTextSize(1);
  tft.setCursor(2, 230);
  tft.print(F("Software by Zdeno Sekerak (c)2018, v0.1"));

  uhol = mapa[0][0];
  krok = mapa[0][1];
  uhol_krok = (mapa[1][0] - uhol)/krok; 
  
  KresliCesta(cesta_d1, cesta_d2, cesta_h1, cesta_h2, uhol, WHITE);
  KresliRychl(rychl);
}
// -----------------------------------------------------------------------------

void KresliRychl(int rychl)
{
  int x;
  for(x=min(rychl,rychl_old); x<max(rychl,rychl_old); x=x+8)
  {
    tft.fillRect(x, 0, 7, 8, (x<rychl)? YELLOW: BLACK);  
  }
  rychl_old = rychl;
}
// -----------------------------------------------------------------------------

void KresliTo(int stred, int zatoc, int vzork, int col)
{
  int x, y;
  int z, zat;

   x = 0;
   z = 0;
   zat = ((int)32*zatoc)/240;

   for(y=220; y>20; y--)
   {
      z += zat;
      x += z/256;

      if(((y/vzork) % 2)
      || (vzork == 1)) {
         tft.drawPixel(stred + x/32, y, col);
      }
   }
}
// -----------------------------------------------------------------------------

void KresliCesta(int x1, int x2, int y1, int y2, int uhol, int color)
{
    KresliTo(x1, y1+uhol, 1, color);
    KresliTo(x2, y2+uhol, 1, color);
    KresliTo((x1 + x2)/2, ((y1+y2)/2) + uhol, 15, color);
}
// -----------------------------------------------------------------------------

// the loop function - interactive with touch
/*
void loop()
{
   TSPoint p = ts.getPoint();

   if( p.z > MIN_TOUCH )
   {
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
    
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, tft.width(), 0); 

      KresliCesta(cesta_d1, cesta_d2, cesta_h1, cesta_h2, uhol, BLACK);

      // vlavo-vpravo
      if(( p.y > 200 ) && ( p.x > 80 ) && ( p.x < 160 ) && ( uhol > -680 )) uhol=uhol-step;
      if(( p.y < 100 ) && ( p.x > 80 ) && ( p.x < 160 ) && ( uhol <  680 )) uhol=uhol+step;

      // plyn-brzda
      if(( p.x <  80 ) && ( p.y > 100 ) && ( p.y < 200 ) && ( rychl < 248 )) rychl=rychl+8;
      if(( p.x > 160 ) && ( p.y > 100 ) && ( p.y < 200 ) && ( rychl >=  8 )) rychl=rychl-8;

      KresliCesta(cesta_d1, cesta_d2, cesta_h1, cesta_h2, uhol, WHITE);
      KresliRychl(rychl);
   }
}
*/

void loop()
{
    KresliCesta(cesta_d1, cesta_d2, cesta_h1, cesta_h2, uhol, BLACK);
    
    // posun uhol
    uhol += uhol_krok;
    // tento krok je dokonceny
    // daj dalsi
    if( krok-- == 0 )
    {
      imapa++;
      if((imapa+1) >= sizeof(mapa)/sizeof(mapa[0]))
          imapa = 0;
      uhol = mapa[imapa][0];
      krok = mapa[imapa][1]; 
      uhol_krok = (mapa[imapa+1][0] - uhol)/krok; 
    }

    KresliCesta(cesta_d1, cesta_d2, cesta_h1, cesta_h2, uhol, WHITE);
    KresliRychl(mapa[imapa][2]);
    delay(100);
}
