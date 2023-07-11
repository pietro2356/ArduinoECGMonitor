#include <Adafruit_GFX.h>       // include Adafruit graphics library
#include <Adafruit_ILI9341.h>   // include Adafruit ILI9341 TFT library
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;

// Define TFT PIN
#define TFT_CS 8
#define TFT_RST 9
#define TFT_DC 10
#define TFT_MOSI 11
#define TFT_CLK 12
#define TFT_MISO 13

#define MAX_BRIGHTNESS 255

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100]; //infrared LED sensor data
uint16_t redBuffer[100];  //red LED sensor data
#else
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
#endif

int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

byte pulseLED = 11; //Must be on PWM pin
byte readLED = 13; //Blinks with each data read

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

  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  // byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  // byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  // byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  // byte sampleRate = 1000; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  // int pulseWidth = 411; //Options: 69, 118, 215, 411
  // int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 3200; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
  Serial.println("Setup Completed"); 
}

void loop() {
  // writeAnalogRead();
  readDataFromSpO2Sensor();

  String anag_read = String(getAnalogReading());
  drawDynamicValue(32, 16, 48, 32, ILI9341_BLACK, 32, 16, 2, ILI9341_GREEN, anag_read);

  if(validSPO2 == 1){
    String str_spo2 = String(spo2);
    drawDynamicValue(32, 64, 48, 32, ILI9341_BLACK, 32, 64, 2, ILI9341_BLUE, str_spo2);
  }else{
    drawDynamicValue(32, 64, 48, 32, ILI9341_BLACK, 32, 64, 2, ILI9341_BLUE, "--");
  }


  // delay(1);
}


void initializeSpO2Sensor(){
  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 1000; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 69; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
  
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample

    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.println(irBuffer[i], DEC);
  }

  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

}

void readDataFromSpO2Sensor(){
  unsigned long start = micros();
  //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
  for (byte i = 25; i < 100; i++)
  {
    redBuffer[i - 25] = redBuffer[i];
    irBuffer[i - 25] = irBuffer[i];
  }

  //take 25 sets of samples before calculating the heart rate.
  for (byte i = 75; i < 100; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    digitalWrite(readLED, !digitalRead(readLED)); //Blink onboard LED with every data read

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample

    //send samples and calculation result to terminal program through UART
    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.print(irBuffer[i], DEC);

    Serial.print(F(", HR="));
    Serial.print(heartRate, DEC);

    Serial.print(F(", HRvalid="));
    Serial.print(validHeartRate, DEC);

    Serial.print(F(", SPO2="));
    Serial.print(spo2, DEC);

    Serial.print(F(", SPO2Valid="));
    Serial.println(validSPO2, DEC);
  }

  //After gathering 25 new samples recalculate HR and SP02
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  Serial.println((micros() - start));
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













