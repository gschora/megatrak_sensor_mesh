#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

#define TFT_RST -1 //for TFT I2C Connector Shield V1.0.0 and TFT 1.4 Shield V1.0.0
#define TFT_CS D4  //for TFT I2C Connector Shield V1.0.0 and TFT 1.4 Shield V1.0.0
#define TFT_DC D3  //for TFT I2C Connector Shield V1.0.0 and TFT 1.4 Shield V1.0.0

// #define TFT_RST -1   //for TFT I2C Connector Shield (V1.1.0 or later)
// #define TFT_CS D0    //for TFT I2C Connector Shield (V1.1.0 or later)
// #define TFT_DC D8    //for TFT I2C Connector Shield (V1.1.0 or later)

#define REV_PIN D2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

uint16_t revCounter = 0;
float counter2 = 0.0;



void setup(void)
{
  tft.initR(INITR_144GREENTAB);
  tft.setTextWrap(false); // Allow text to run off right edge
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  pinMode(REV_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(REV_PIN), revCounting, FALLING); 
}

void loop(void)
{
//   counter = ++counter;
//   counter2 = ++counter2;
//   tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.setTextSize(3);
  tft.println(revCounter);

//   tft.setCursor(0, 80);
//   tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
//   tft.setTextSize(3);
//   tft.println(counter2);
//   delay(1000);
}

void revCounting(){
  ++revCounter;
//   tft.setCursor(0, 0);
//   tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
//   tft.setTextSize(3);
//   tft.println(revCounter);
}