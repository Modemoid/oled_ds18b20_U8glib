/*

  >>> Before compiling: Please remove comment from the constructor of the
  >>> connected graphics display (see below).

  Universal 8bit Graphics Library, https://github.com/olikraus/u8glib/

*/


#include "U8glib.h"

// setup u8g object, please remove comment from one of the following constructor calls
// IMPORTANT NOTE: The following list is incomplete. The complete list of supported
// devices with all constructor calls is here: https://github.com/olikraus/u8glib/wiki/device
U8GLIB_PCD8544 u8g(12, 11, 10, 7, 8);		// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 7, Reset = 8


#define Latcount 10
#define ADC_debug
#define Fuel_mark
#define Fuel_Bar_mark
#define HBarW 10
#define Accum_mark
#define AccHeight 10 //mark height +1
#define acuOffsetW 0//по ширинеот левого бара 
#define acuOffsetH -0 //по высоте от нижнего края

#define TopPicHeight 16

#define HeadLight
#define TurnSig
//#define smallarrow
#define ArrOffset 0

#define backlight 9 //backlight pin. Must can PWM

#define SigTable //draw table greed around top icons

#define Left_Bar
#define Left_Bar_mark

//manual defined screen size for nokia
#define heightLCD 48 //if not defined be calculated
#define widthLCD 84
//end manual defined screen size

//ALL EXTERNAL PINS MUST HAVE LEVEL LIMITERS (ZENNER diode)
//ALL EXTERNAL PINS MUST HAVE LEVEL LIMITERS (ZENNER diode)
//ALL EXTERNAL PINS MUST HAVE LEVEL LIMITERS (ZENNER diode)

#define RTurnPin 2 //attach pins
#define LTurnPin 3 //attach pins
#define HeadBemPin 4 //attach pins
#define LoBemPin 5 //attach pins

//ALL EXTERNAL PINS MUST HAVE LEVEL LIMITERS (ZENNER diode)
//ALL EXTERNAL PINS MUST HAVE LEVEL LIMITERS (ZENNER diode)
//ALL EXTERNAL PINS MUST HAVE LEVEL LIMITERS (ZENNER diode)

int sensorPin = A0;   //Fuel level pin
int sensorValue = 0;  //
int sensorLat = 0;

int PowerPin = A1; //"Power" level pin
int PowerValue = 0;
int PowerLat = 0;

int VoltPin = A2; //BoardPower net  pin
int VoltValue = 0;
int VoltLat = 0;

//ALL EXTERNAL PINS MUST HAVE LEVEL LIMITERS (ZENNER diode)
//ALL EXTERNAL PINS MUST HAVE LEVEL LIMITERS (ZENNER diode)
//ALL EXTERNAL PINS MUST HAVE LEVEL LIMITERS (ZENNER diode)
char latency = 0;
int DrawH = 0;
int FuelStart = 70, FuelEnd = 950; //ADC offset for fuel LVL sevsor
char FuelLVLProcent = 0;
char LbarH = 0;

#ifdef TurnDebug
char TurnBlink = 1;
#endif
//bitmap
#ifdef Fuel_mark
const uint8_t fuel_bitmap[] PROGMEM = {
  0b00000000,
  0b01111100,
  0b01000110,
  0b01000110,
  0b01111100,
  0b01111100,
  0b01111100,
  0b00000000
};
#endif

#ifdef Accum_mark
const uint8_t accum_bitmap[] PROGMEM = {
  0b00011100, 0b00111000,
  0b01110111, 0b11101110,
  0b01000000, 0b00000010,
  0b01001000, 0b00000010,
  0b01011100, 0b00111010,
  0b01001000, 0b00000010,
  0b01000000, 0b00000010,
  0b01111111, 0b11111110
};
#endif
#ifdef HeadLight
const uint8_t hibeam_bitmap[] PROGMEM = {
  0b00001000,
  0b00111000,
  0b01101111,
  0b10001000,
  0b10001111,
  0b10001000,
  0b01101111,
  0b00111000,
  0b00001000
};
const uint8_t lobeam_bitmap[] PROGMEM = {
  0b00001000,
  0b00111000,
  0b01111100,
  0b11001010,
  0b10001001,
  0b11001100,
  0b01111010,
  0b00111001,
  0b00001000
};
#endif
#ifdef smallarrow
const uint8_t rArr_bitmap[] PROGMEM = {
  0b00001000,
  0b00001100,
  0b01111110,
  0b01111111,
  0b01111110,
  0b00001100,
  0b00001000
};
const uint8_t lArr_bitmap[] PROGMEM = {
  0b00010000,
  0b00110000,
  0b01111110,
  0b11111110,
  0b01111110,
  0b00110000,
  0b00010000
};
#endif
const uint8_t chkeng_bitmap[] PROGMEM = {
  0b00000111, 0b11000000,
  0b00000001, 0b00000000,
  0b00011111, 0b11100000,
  0b00100000, 0b00010000,
  0b00100001, 0b00010111,
  0b10100001, 0b00011101,
  0b11100001, 0b00000001,
  0b10100000, 0b00011101,
  0b00110001, 0b00010111,
  0b00001000, 0b00010000,
  0b00000111, 0b11100000
};

const uint8_t BigRArr_bitmap[] PROGMEM = {
  0b00001110, 0b00000000,
  0b00000111, 0b00000000,
  0b00000011, 0b10000000,
  0b00000001, 0b11000000,
  0b11111111, 0b11100000,
  0b01111111, 0b11110000,
  0b00111111, 0b11111000,
  0b00011111, 0b11111100,
  0b00111111, 0b11111000,
  0b01111111, 0b11110000,
  0b11111111, 0b11100000,
  0b00000001, 0b11000000,
  0b00000011, 0b10000000,
  0b00000111, 0b00000000,
  0b00001110, 0b00000000,
};

const uint8_t BigLArr_bitmap[] PROGMEM = {

  0b00000001,  0b11000000,
  0b00000011, 0b10000000,
  0b00000111, 0b00000000,
  0b00001110, 0b00000000,
  0b00011111, 0b11111100,
  0b00111111, 0b11111000,
  0b01111111, 0b11110000,
  0b11111111, 0b11100000,
  0b01111111, 0b11110000,
  0b00111111, 0b11111000,
  0b00011111, 0b11111100,
  0b00001110, 0b00000000,
  0b00000111, 0b00000000,
  0b00000011, 0b10000000,
  0b00000001, 0b11000000

};
const uint8_t BigHiBeam_bitmap[] PROGMEM = {
0b00000000,  0b01000000,
0b00000001, 0b11000000,
0b00000111, 0b11000000,
0b00011111, 0b11111000,
0b00111111, 0b11000000,
0b01111111, 0b11000000,
0b01101010, 0b11111000,
0b11100010, 0b11000000,
0b01101010, 0b11000000,
0b01111111, 0b11111000,
0b00111111, 0b11000000,
0b00011111, 0b11000000,
0b00000111, 0b11111000,
0b00000001, 0b11000000,
0b00000000, 0b01000000
};

const uint8_t BigLoBeam_bitmap[] PROGMEM = {
0b00000000,  0b01000000,
0b00000001, 0b11000000,
0b00000111, 0b11100000,
0b00011111, 0b11010000,
0b00111111, 0b11001000,
0b01111111, 0b11100000,
0b01101100, 0b01010000,
0b11101101, 0b01001000,
0b01100100, 0b01100000,
0b01111111, 0b11010000,
0b00111111, 0b11001000,
0b00011111, 0b11100000,
0b00000111, 0b11010000,
0b00000001, 0b11001000,
0b00000000, 0b01000000

};

//end bitmap


void u8g_prepare(void) {
  //u8g.setFont(u8g_font_6x10);
  //u8g.setFont(u8g_font_04b_24n);
  u8g.setFont(u8g_font_5x8); //~2kb
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

void u8g_1screen(uint8_t a) {


  char buf[8];
#ifndef heightLCD
  char heightLCD, widthLCD;

  heightLCD = u8g.getHeight(); //высота
  widthLCD = u8g.getWidth(); //ширина
#endif
#ifdef ADC_debug
  //u8g.drawFrame(0,0,84,48);nokia 5110 lcd frame
  sprintf (buf, "a0=%d", sensorValue);
  u8g.drawStr(widthLCD / 2 - 10, 17, buf);
#endif
  // fuel LVL
  // xx% near bar
  sprintf (buf, "%d%%", FuelLVLProcent);
  u8g.drawStr(widthLCD - HBarW - 16, heightLCD - 8 , buf);
  //end  xx% near bar
  //bar
  u8g.drawFrame(widthLCD - HBarW , 0, HBarW, heightLCD - DrawH);
  u8g.drawBox(widthLCD - HBarW , heightLCD - DrawH, HBarW, heightLCD);
#ifdef Fuel_Bar_mark
  //fuel bar mark
  u8g.drawPixel(widthLCD - HBarW - 1, heightLCD / 2);
  u8g.drawPixel(widthLCD - HBarW - 2, heightLCD / 2);

  u8g.drawPixel(widthLCD - HBarW + 1, heightLCD / 2);
  u8g.drawPixel(widthLCD - HBarW + 3, heightLCD / 2);
  u8g.drawPixel(widthLCD - HBarW + 5, heightLCD / 2);

  u8g.drawPixel(widthLCD - HBarW - 1, heightLCD / 4);
  u8g.drawPixel(widthLCD - HBarW + 1, heightLCD / 4);
  u8g.drawPixel(widthLCD - HBarW + 3, heightLCD / 4);
  u8g.drawPixel(widthLCD - HBarW + 5, heightLCD / 4);

  u8g.drawPixel(widthLCD - HBarW - 1, (heightLCD / 4) * 3);
  u8g.drawPixel(widthLCD - HBarW + 1, (heightLCD / 4) * 3);
  u8g.drawPixel(widthLCD - HBarW + 3, (heightLCD / 4) * 3);
  u8g.drawPixel(widthLCD - HBarW + 5, (heightLCD / 4) * 3);

  u8g.setColorIndex(0);
  u8g.drawPixel(widthLCD - HBarW, heightLCD / 2);
  u8g.drawPixel(widthLCD - HBarW + 2, heightLCD / 2);
  u8g.drawPixel(widthLCD - HBarW + 4, heightLCD / 2);

  u8g.drawPixel(widthLCD - HBarW, heightLCD / 4);
  u8g.drawPixel(widthLCD - HBarW + 2, heightLCD / 4);
  u8g.drawPixel(widthLCD - HBarW + 4, heightLCD / 4);

  u8g.drawPixel(widthLCD - HBarW, (heightLCD / 4) * 3);
  u8g.drawPixel(widthLCD - HBarW + 2, (heightLCD / 4) * 3);
  u8g.drawPixel(widthLCD - HBarW + 4, (heightLCD / 4) * 3);

  u8g.setColorIndex(1);
  //end fuel bar mark
#endif
  //end bar
#ifdef Fuel_mark
  u8g.setColorIndex(0);
  u8g.drawBitmapP( widthLCD - 9, heightLCD - 8, 1, 8, fuel_bitmap);
  u8g.setColorIndex(1);
#endif
  //end fuel LVL

#ifdef Accum_mark
  u8g.drawBitmapP(HBarW + 1 + acuOffsetW, heightLCD - 8 + acuOffsetH, 2, 8, accum_bitmap);
  sprintf (buf, "%d.%dV", VoltValue / 10, VoltValue % 10);
  u8g.drawStr(HBarW + 18 + acuOffsetW, heightLCD - 8 + acuOffsetH, buf);

  u8g.drawLine(HBarW + 1, heightLCD - AccHeight, widthLCD - (HBarW + 2), heightLCD - AccHeight);
  u8g.drawLine(55, heightLCD, 55, heightLCD - AccHeight);


#endif
#ifdef HeadLight

  u8g.drawBitmapP(27, 0, 2, 15, BigLoBeam_bitmap);

  u8g.drawBitmapP(43, 0, 2, 15, BigHiBeam_bitmap);

#endif

#ifdef CheckEng
  u8g.drawBitmapP( HBarW + 2 + 36, heightLCD / 2, 2, 11, chkeng_bitmap);
#endif


#ifdef SigTable
  //signal table greed
  u8g.drawLine(HBarW + 1, TopPicHeight, widthLCD - (HBarW + 2), TopPicHeight);
  u8g.drawLine(26, 0, 26, TopPicHeight);
  u8g.drawLine(41, 0, 41, TopPicHeight);
  u8g.drawLine(42, 0, 42, TopPicHeight);
  u8g.drawLine(57, 0, 57, TopPicHeight);
  //end signal table greed
#endif


#ifdef TurnSig

  if (digitalRead(LTurnPin))
  {
    u8g.drawBitmapP( HBarW + 1 , ArrOffset, 2, 15, BigLArr_bitmap); //big
    //u8g.drawBitmapP( HBarW + 2, ArrOffset, 1, 7, lArr_bitmap); //small
  }

  if (digitalRead(RTurnPin))
  {
    u8g.drawBitmapP( widthLCD - HBarW - 1 - 14 , ArrOffset, 2, 15, BigRArr_bitmap);//big
    //u8g.drawBitmapP( widthLCD - HBarW - 2 - 8, ArrOffset, 1, 7, rArr_bitmap);//small
  }

#ifdef TurnDebug //hardcodec small arrow only
  switch (TurnBlink) { //cycle show for test
    case 1: u8g.drawBitmapP( HBarW + 2, ArrOffset, 1, 7, lArr_bitmap); break;
    case 2: u8g.drawBitmapP( widthLCD - HBarW - 2 - 8, ArrOffset, 1, 7, rArr_bitmap); break;
    case 3: {
        u8g.drawBitmapP( HBarW + 2, ArrOffset, 1, 7, lArr_bitmap);
        u8g.drawBitmapP( widthLCD - HBarW - 2 - 8, ArrOffset, 1, 7, rArr_bitmap);
      }; break;
  }
#endif
#endif

#ifdef Left_Bar
  //left bar
  u8g.drawFrame(0 , 0, HBarW, heightLCD - LbarH );
  u8g.drawBox(0, heightLCD - LbarH, HBarW, heightLCD);

#ifdef Left_Bar_mark
  //Left bar mark
  u8g.drawPixel(HBarW + 1, heightLCD / 2);
  
  u8g.drawPixel(HBarW - 1 - 5, heightLCD / 2);
  u8g.drawPixel(HBarW + 1 - 5, heightLCD / 2);
  u8g.drawPixel(HBarW + 3 - 5, heightLCD / 2);
  u8g.drawPixel(HBarW + 5 - 5, heightLCD / 2);

  u8g.drawPixel(HBarW - 1 - 5, heightLCD / 4);
  u8g.drawPixel(HBarW + 1 - 5, heightLCD / 4);
  u8g.drawPixel(HBarW + 3 - 5, heightLCD / 4);
  u8g.drawPixel(HBarW + 5 - 5, heightLCD / 4);

  u8g.drawPixel(HBarW - 1 - 5, (heightLCD / 4) * 3);
  u8g.drawPixel(HBarW + 1 - 5, (heightLCD / 4) * 3);
  u8g.drawPixel(HBarW + 3 - 5, (heightLCD / 4) * 3);
  u8g.drawPixel(HBarW + 5 - 5, (heightLCD / 4) * 3);

  u8g.setColorIndex(0);
  u8g.drawPixel(HBarW - 5, heightLCD / 2);
  u8g.drawPixel(HBarW + 2 - 5, heightLCD / 2);
  u8g.drawPixel(HBarW + 4 - 5, heightLCD / 2);

  u8g.drawPixel(HBarW - 5, heightLCD / 4);
  u8g.drawPixel(HBarW + 2 - 5, heightLCD / 4);
  u8g.drawPixel(HBarW + 4 - 5, heightLCD / 4);

  u8g.drawPixel(HBarW - 5, (heightLCD / 4) * 3);
  u8g.drawPixel(HBarW + 2 - 5, (heightLCD / 4) * 3);
  u8g.drawPixel(HBarW + 4 - 5, (heightLCD / 4) * 3);

  u8g.setColorIndex(1);
  //end Left bar mark
#endif
  //end of Left bar
#endif


#ifdef ADC_debug
  //Raw a1 out
  sprintf (buf, "a1=%d", PowerValue);
  u8g.drawStr(widthLCD / 2 - 10, 25, buf);
#endif
  //sensorValue
}



uint8_t draw_state = 0;

void draw(void) {
  u8g_prepare();
  u8g_1screen(draw_state);
}


void setup(void) {

  // flip screen, if required
  //u8g.setRot180();

  pinMode(13, OUTPUT); //arduino led pin
  digitalWrite(13, HIGH);

  pinMode(backlight, OUTPUT); //backlight
  analogWrite(backlight, 1);

  pinMode(RTurnPin, INPUT);
  pinMode(LTurnPin, INPUT);

}

void loop(void) {


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

  delay(50);

  //   digitalWrite(13,digitalRead(2));

  if (latency < Latcount)
  {
    latency++;
    sensorLat = sensorLat + analogRead(sensorPin);
    PowerLat = PowerLat + analogRead(PowerPin);
    VoltLat = VoltLat + analogRead(VoltPin);

  } else
  {
    latency = 0;
    DrawH = map(sensorLat, FuelStart * Latcount, FuelEnd * Latcount, 1, 47);
    FuelLVLProcent = map(sensorLat, FuelStart * Latcount, FuelEnd * Latcount, 0, 100);
    sensorValue = sensorLat / Latcount;
    sensorLat = 0;

    PowerValue = PowerLat / Latcount;
    PowerLat = 0;

    VoltValue = map(VoltLat / Latcount, 404, 978, 70, 170); //r1=100k r2=39k koff=3.56 7v=1.97(RAW=404) 17v=4.78(RAW=978)
    //
    //VoltValue = VoltLat / Latcount;
    VoltLat = 0;
  }

  if (LbarH < 48)
  {
    LbarH++ ;
  } else
  {
    LbarH = 0;
#ifdef TurnDebug
    TurnBlink++;
    if (TurnBlink > 3 ) {
      TurnBlink = 0;
    }
#endif
  }
}


