#include <Adafruit_GFX.h>       // include Adafruit graphics library
#include <Adafruit_ILI9341.h>   // include Adafruit ILI9341 TFT library

// Define TFT PIN
#define TFT_CS 8
#define TFT_RST 9
#define TFT_DC 10
#define TFT_MOSI 11
#define TFT_CLK 12
#define TFT_MISO 13

// Define ANALOG PIN
int ANALOG_PIN = A0;
int PADD = 3;
int FR_DISPLAY_TIMER = 5;

// initialize ILI9341 TFT library
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

unsigned long lastFrame = millis();
int frameDisplayTimer = 2;

void setup() {
  Serial.begin(9600);
  Serial.println("ILI9341 Test!"); 
 
  tft.begin();

  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  drawLine();
  drawLabel();
}

void loop() {
  // writeAnalogRead();
  String anag_read = String(getAnalogReading());
  drawDynamicValue(32, 16, 48, 32, ILI9341_BLACK, 32, 16, 2, ILI9341_GREEN, anag_read);

  delay(1);
}


void writeAnalogRead(){

  // frameDisplayTimer--;
  // if(frameDisplayTimer == 0){
  //   frameDisplayTimer = 2;
  //   tft.fillRect(32, 16, 48, 32, ILI9341_BLACK);
  //   tft.setCursor(32, 16);
  //   tft.setTextSize(2);
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.print(getAnalogReading());
  // }

  // while((millis() - lastFrame) < 2);

  // lastFrame = millis();

  // drawDynamicValue(32, 16, 48, 32, ILI9341_BLACK, 32, 16, 2, ILI9341_GREEN, getAnalogReading());

}

int getAnalogReading(){
  return analogRead(ANALOG_PIN);
}

void drawLine(){
  tft.drawLine(80, 0, 80, 240, ILI9341_WHITE);
  tft.drawLine(0, 48, 80, 48, ILI9341_WHITE);
  tft.drawLine(0, 96, 80, 96, ILI9341_WHITE);
  tft.drawLine(0, 144, 80, 144, ILI9341_WHITE);
  tft.drawLine(0, 192, 80, 192, ILI9341_WHITE);
  tft.drawLine(80, 80, 320, 80, ILI9341_WHITE);
  tft.drawLine(80, 160, 320, 160, ILI9341_WHITE);
}

void drawLabel(){
  // Print FC
  drawStaticLabel(0, 0, 1, ILI9341_GREEN, "FC");
  // Print SpO2
  drawStaticLabel(0, 48, 1, ILI9341_BLUE, "SpO2");
  // Print RR
  drawStaticLabel(0, 96, 1, ILI9341_ORANGE, "RR");
  // Print NIMB
  drawStaticLabel(0, 144, 1, ILI9341_WHITE, "NIMB");
  // Print TEMP
  drawStaticLabel(0, 192, 1, ILI9341_WHITE, "TEMP");


  // Print Lead II
  drawStaticLabel(80, 0, 1, ILI9341_GREEN, "Lead II");
  // Print SpO2
  drawStaticLabel(80, 80, 1, ILI9341_BLUE, "SpO2");
  // Print RR
  drawStaticLabel(80, 160, 1, ILI9341_ORANGE, "RR");


}


void drawStaticLabel(int cursorX, int cursorY, int textSize, uint16_t color, const String& text){
  tft.setCursor(cursorX + PADD, cursorY + PADD);
  tft.setTextSize(textSize);
  tft.setTextColor(color);
  tft.print(text);
}

void drawDynamicValue(int rectX, int rectY, int rectWidth, int rectHeight, int rectColor, int cursorX, int cursorY, int textSize, uint16_t textColor, const String& text){
  frameDisplayTimer--;
  if(frameDisplayTimer == 0){
    frameDisplayTimer = FR_DISPLAY_TIMER;
    tft.fillRect(rectX, rectY, rectWidth, rectHeight, rectColor);
    tft.setCursor(cursorX, cursorY);
    tft.setTextSize(textSize);
    tft.setTextColor(textColor);
    tft.print(text);
  }

  while((millis() - lastFrame) < FR_DISPLAY_TIMER);

  lastFrame = millis();
}













