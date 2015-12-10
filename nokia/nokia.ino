/*

  >>> Before compiling: Please remove comment from the constructor of the
  >>> connected graphics display (see below).

  Universal 8bit Graphics Library, https://github.com/olikraus/u8glib/

*/


#include "U8glib.h"

// setup u8g object, please remove comment from one of the following constructor calls
// IMPORTANT NOTE: The following list is incomplete. The complete list of supported
// devices with all constructor calls is here: https://github.com/olikraus/u8glib/wiki/device
U8GLIB_PCD8544 u8g(12, 11, 10, 9, 8);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9, Reset = 8


#define Latcount 10
#define ADC_debug

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int PowerPin = A1;
int PowerValue = 0;
int sensorLat = 0;
int PowerLat = 0;
char latency = 0;
int DrawH = 0;
char HBarW = 10;
int FuelStart = 70, FuelEnd = 950; //ADC offset for fuel LVL sevsor
char FuelLVLProcent = 0;
char LbarH = 0;
void u8g_prepare(void) {
  // u8g.setFont(u8g_font_6x10);
  //u8g.setFont(u8g_font_04b_24n);
  u8g.setFont(u8g_font_5x8);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

void u8g_box_frame(uint8_t a) {
  //  u8g.drawStr( 0, 0, "drawBox");
  char buf[8];
  char heightLCD, widthLCD;

  heightLCD = u8g.getHeight(); //высота
  widthLCD = u8g.getWidth(); //ширина

#ifdef ADC_debug
  //u8g.drawFrame(0,0,84,48);nokia 5110 lcd frame
  sprintf (buf, "a0=%d", sensorValue);
  u8g.drawStr(widthLCD / 2 - 10, 0, buf);
#endif
  // fuel LVL
  // xx% near bar
  sprintf (buf, "%d%%", FuelLVLProcent);
  u8g.drawStr(widthLCD - HBarW - 16, heightLCD - 8 , buf);
  //bar
  u8g.drawFrame(widthLCD - HBarW , 0, HBarW, heightLCD - DrawH);
  u8g.drawBox(widthLCD - HBarW , heightLCD - DrawH, HBarW, heightLCD);
  //end fuel LVL
  //left bar
  u8g.drawFrame(0 , 0, HBarW, heightLCD - LbarH );
  u8g.drawBox(0, heightLCD - LbarH, HBarW, heightLCD);
  //end of Left bar



#ifdef ADC_debug
  //Raw a1 out
  sprintf (buf, "a1=%d", PowerValue);
  u8g.drawStr(widthLCD / 2 - 10, 10, buf);
#endif
  //sensorValue
}

void u8g_ascii_1() {
  char s[2] = " ";
  uint8_t x, y;
  u8g.drawStr( 0, 0, "ASCII page 1");
  for ( y = 0; y < 6; y++ ) {
    for ( x = 0; x < 16; x++ ) {
      s[0] = y * 16 + x + 32;
      u8g.drawStr(x * 7, y * 10 + 10, s);
    }
  }
}

void u8g_ascii_2() {
  char s[2] = " ";
  uint8_t x, y;
  u8g.drawStr( 0, 0, "ASCII page 2");
  for ( y = 0; y < 6; y++ ) {
    for ( x = 0; x < 16; x++ ) {
      s[0] = y * 16 + x + 160;
      u8g.drawStr(x * 7, y * 10 + 10, s);
    }
  }
}



uint8_t draw_state = 0;

void draw(void) {
  u8g_prepare();
  switch (draw_state >> 3) {
    case 0: u8g_box_frame(draw_state & 7); break;
      //case 6: u8g_ascii_1(); break;
      //case 7: u8g_ascii_2(); break;

  }
}


void setup(void) {

  // flip screen, if required
  //u8g.setRot180();

  pinMode(13, OUTPUT); //arduino led pin
  digitalWrite(13, HIGH);

  pinMode(3, OUTPUT); //backlight
  analogWrite(3, 0);


}

void loop(void) {

  // picture loop
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );

  // increase the state
  draw_state = 6;
  // if ( draw_state >= 9*8 )
  //   draw_state = 0;
  //delay(500);
  // rebuild the picture after some delay

  delay(100);
  if (latency < Latcount)
  {
    latency++;
    sensorLat = sensorLat + analogRead(sensorPin);
    PowerLat = PowerLat + analogRead(PowerPin);
  } else
  {
    latency = 0;
    DrawH = map(sensorLat, FuelStart * Latcount, FuelEnd * Latcount, 1, 47);
    FuelLVLProcent = map(sensorLat, FuelStart * Latcount, FuelEnd * Latcount, 0, 100);
    sensorValue = sensorLat / Latcount;
    sensorLat = 0;

    PowerValue = PowerLat / Latcount;
    PowerLat = 0;
  }

  if (LbarH < 48)
  {
    LbarH++ ;
  } else
  {
    LbarH = 0;
  }
}


