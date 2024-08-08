// Now turn this trash into treasure!
#include <TM1637.h>
#include <Adafruit_NeoPixel.h>

#define pir 2
#define button 4
#define buzzer 3
#define neopixel 5
#define clk 27
#define dio 28

TM1637 tm(clk, dio);
Adafruit_NeoPixel pixels(1, neopixel, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(pir, INPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(neopixel, OUTPUT);
  tm.init();
  tm.set(BRIGHT_TYPICAL);
  pixels.begin();
}
void loop() {

}
