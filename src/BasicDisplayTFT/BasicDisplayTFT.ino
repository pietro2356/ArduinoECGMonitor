#include <Adafruit_GFX.h>       // include Adafruit graphics library
#include <Adafruit_ILI9341.h>   // include Adafruit ILI9341 TFT library

#define TFT_CS 8
#define TFT_RST 9
#define TFT_DC 10
#define TFT_MOSI 11
#define TFT_CLK 12
#define TFT_MISO 13

int ANALOG_PIN = A0;

int pot_val = 0;
// initialize ILI9341 TFT library
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);


void setup() {
  Serial.begin(9600);
  Serial.println("ILI9341 Test!"); 
 
  tft.begin();

  // read diagnostics (optional but can help debug problems)
  // uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  // Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  // x = tft.readcommand8(ILI9341_RDMADCTL);
  // Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  // x = tft.readcommand8(ILI9341_RDPIXFMT);
  // Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  // x = tft.readcommand8(ILI9341_RDIMGFMT);
  // Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  // x = tft.readcommand8(ILI9341_RDSELFDIAG);
  // Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 

  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);

  Serial.print(F("Text                     "));
  Serial.println(testText());

  // sismic();
}

void loop() {
  // put your main code here, to run repeatedly:
  // testText();
  testText();

  // pot_val = analogRead(ANALOG_PIN) * (5.0 / 1023.0);
  // tft.setTextWrap(false);
  // tft.setTextColor(ILI9341_WHITE);
  // tft.setCursor(30,160);
  // tft.print(pot_val);
  // tft.print("");
}

unsigned long testText(){
  unsigned long start = micros();
  // Cursore a 0, 0
  tft.setCursor(0, 0);
  // Colore BIANCO
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);

  pot_val = analogRead(ANALOG_PIN);  // read the input pin
  Serial.println(pot_val);          // debug value
  
  tft.setTextColor(ILI9341_ORANGE);  
  tft.setTextSize(2);

  // Scriviamo la stringa Hello Wordl
  String val = String(pot_val);
  tft.println("Letturra dal pin A0: " + val);

  return micros() - start;
}

// void readAnalogVal(){
//   pot_val = analogRead(ANALOG_PIN);  // read the input pin
//   Serial.println(pot_val);          // debug value
  
//   tft.setTextColor(ILI9341_ORANGE);  tft.setTextSize(2);
//   tft.println(pot_val);
// }

void sismic(){
  tft.setRotation(3);     //Landscape, for portrat it is 0
  tft.fillScreen(ILI9341_BLACK); 
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  //Escribimos el texto:
  tft.setCursor(150,10);
  tft.print("Seismic Wave Graphic");
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(120,300);
  tft.print("INGENIERIA GEOFISICA");
  tft.drawLine(20,60,20,260,ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(30,50);
  tft.print("19 mm");
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(30,260);
  tft.print("19 mm");
}




