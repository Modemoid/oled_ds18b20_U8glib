#include "U8glib.h"

U8GLIB_PCD8544 u8g(8, 7, 11, 12, 10);    // SPI Com: SCK(clk) = 2, MOSI(din) = 11, CS(ce) = 10, A0(dc) = 7, Reset(rst) = 8
#define BLPin 9
//#define fillArrow
//#define LineBar
char x = 42, y = 24;/*центр окружности*/;
int alpha = 22;
const float pi = 3.14;

unsigned char BL = 25, f, contrast = 150;

char bytes;
char radius = 20;
unsigned int c, d, e, ADC1;

void setup() {

  LCDSetup();
  pinMode(BLPin, OUTPUT); //настроим светодиод как выход
  //digitalWrite(BLPin, LOW);    // turn the Backlight off
  analogWrite(BLPin, 50);    // turn the Backlight off
  pinMode(13, OUTPUT); //настроим светодиод как выход
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  pinMode(5, INPUT); //
  pinMode(A1, INPUT); //настроим светодиод как выход

}

void loop() {

  alpha++;
  if (alpha > 360)
  {
    alpha = 0;
  }

  //digitalWrite(13,digitalRead(2));
  bytes = Serial.available();
  if (bytes != 0)
  {
    c = Serial.parseInt();
  }
  if (0 < c && c < 255) //bscklight
  {
    BL = c;
  }


  if (1000 < c && c < 2000)
  {
    d = c - 1000;
    //d=map(c,1000,2000,0,1000);
  }

  if (2000 < c && c < 2999)
  {
    e = c - 2000;
    //e=map(c,2000,3000,0,1000);
  }
  if (2999 < c && c < 3255)
  {
    f = c - 3000;
    //e=map(c,2000,3000,0,1000);
  }
  if (3999 < c && c < 4255)
  {
    contrast = c - 4000;
    //e=map(c,2000,3000,0,1000);
  }

  if (4999 < c && c < 5181)
  {
    alpha = c - 5000;
    //e=map(c,2000,3000,0,1000);
  }

  u8g.setContrast(contrast);

  analogWrite(BLPin, BL);    // turn the Backlight off
  //f++;
  if (f > 254)
  {
    f = 0;
  }
  //delay(d);
  analogWrite(13, f);    //
  //digitalWrite(13,digitalRead(5));
  ADC1 = analogRead(A1);
  // picture loop
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );


  /*
  analogWrite(13,f);
   // rebuild the picture after some delay
   digitalWrite(13,LOW);
   delay(d);
   //delayMicroseconds(d);
   digitalWrite(13,HIGH);
   //delayMicroseconds(e);
   delay(e);
   */
}
// ScaleType 1-линия 2-сегмент размером barW*barH 3-сектор от 0 до ArrowDegre(ооочень медленно)  4- толстя стрелка
//DrawRoundScale(X центра, У центра, Радиус, угол показаний, тип шкалы, какие контуры рисовать,высота бара(на сколько отступить внутренней окружностью), ширина бара в градусах)
void DrawRoundScale(char xpos, char ypos, char Rad, int ArrowDegre, char ScaleType, char drawScale,char barW, char barH)
{
  //84x48

  //x=xpos;
  //y = ypos;
  radius = Rad;
  //alpha = ArrowDegre;

  float rx, ry; //точка  куда рисовать линию
  float alphaRad, c, s;
  float co, si;

  if (drawScale == 0)
  {
  }
  if (drawScale == 1)
  {
    u8g.drawCircle(xpos, ypos, Rad);
    u8g.drawPixel(xpos, ypos);
  }

  if (drawScale == 2)
  {
    u8g.drawCircle(xpos, ypos, Rad);
    u8g.drawCircle(xpos, ypos, Rad-barW);
    u8g.drawPixel(xpos, ypos);
  }

  if (ScaleType == 1)
  {
    alphaRad = (180 - ArrowDegre) * 0.0174; //нормальная шкала
    //alphaRad = (alpha)*0.0174;    //справа 0 слева 180!!!!!!!!!!
    co = cos(alphaRad);
    si = sin(alphaRad);
    rx = char((co * radius) + xpos);
    ry = char(ypos - (si * radius));
    u8g.drawLine(xpos, ypos, rx, ry);
  }

  if (ScaleType == 2)
  {
    char temp1, temp2;

    //u8g.drawCircle(xpos, y, radius-barW+1,U8G_DRAW_UPPER_RIGHT);
    //u8g.drawCircle(xpos, y, radius-barW+1,U8G_DRAW_UPPER_LEFT);

    for (temp2 = 0; temp2 < barH; temp2++)
    {
      alphaRad = (180 - ArrowDegre - temp2) * 0.0174; //нормальная шкала
      //alphaRad = (alpha)*0.0174;    //справа 0 слева 180!!!!!!!!!!
      co = cos(alphaRad);
      si = sin(alphaRad);
      for (temp1 = 0; temp1 < barW; temp1++)
      {
        rx = char((co * (radius - temp1)) + xpos);
        ry = char(ypos - (si * (radius - temp1)));
        u8g.drawPixel(rx, ry);
      }
    }
  }

  if (ScaleType == 3)//must be fixed! work only 0-180
  {
    unsigned char temp;
    for (temp = 0; temp < ArrowDegre; temp++)
    {
      alphaRad = (180 - temp) * 0.0174; //нормальная шкала
      //alphaRad = (alpha)*0.0174;    //справа 0 слева 180!!!!!!!!!!
      co = cos(alphaRad);
      si = sin(alphaRad);
      rx = char((co * radius) + xpos);
      ry = char(ypos - (si * radius));
      u8g.drawLine(xpos, ypos, rx, ry);

    }
  }
  if (ScaleType == 4)
  {
    ////////////// толстая стрелка
    alphaRad = (180 - ArrowDegre - 1) * 0.0174; //нормальная шкала
    //alphaRad = (alpha)*0.0174;    //справа 0 слева 180!!!!!!!!!!
    co = cos(alphaRad);
    si = sin(alphaRad);
    rx = char((co * radius) + xpos);
    ry = char(ypos - (si * radius));
    u8g.drawLine(xpos, ypos, rx, ry);
    alphaRad = (180 - ArrowDegre - 2) * 0.0174; //нормальная шкала
    //alphaRad = (alpha)*0.0174;    //справа 0 слева 180!!!!!!!!!!
    co = cos(alphaRad);
    si = sin(alphaRad);
    rx = char((co * radius) + xpos);
    ry = char(ypos - (si * radius));
    u8g.drawLine(xpos, ypos, rx, ry);

    alphaRad = (180 - ArrowDegre + 1) * 0.0174; //нормальная шкала
    //alphaRad = (alpha)*0.0174;    //справа 0 слева 180!!!!!!!!!!
    co = cos(alphaRad);
    si = sin(alphaRad);
    rx = char((co * radius) + xpos);
    ry = char(ypos - (si * radius));
    u8g.drawLine(xpos, ypos, rx, ry);
    alphaRad = (180 - ArrowDegre + 2) * 0.0174; //нормальная шкала
    //alphaRad = (alpha)*0.0174;    //справа 0 слева 180!!!!!!!!!!
    co = cos(alphaRad);
    si = sin(alphaRad);
    rx = char((co * radius) + xpos);
    ry = char(ypos - (si * radius));
    u8g.drawLine(xpos, ypos, rx, ry);
    ////////////////////// конец толстая стрелка
  }


  //u8g.drawLine(x, y, rx+1, ry+1);
  //u8g.drawLine(x, y, rx-1, ry-1);



}
void LCDSetup(void)
{
  // flip screen, if required
  //u8g.setRot180();

  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255, 255, 255);
  }
  //u8g.setFont(u8g_font_6x10);
  u8g.setFont(u8g_font_5x7); //17chars,6.5 lines
}

void draw(void) {
  /*
  u8g.setPrintPos(0,6);
  u8g.print(c, DEC);
  u8g.print(":");
  u8g.print(bytes, DEC);
  u8g.print(":");
  u8g.print(BL, DEC);
  u8g.print(" A1:");
  u8g.print(ADC1, DEC);

  u8g.setPrintPos(0,13);
  u8g.print("D");
  u8g.print(d, DEC);
  u8g.print(" E");
  u8g.print(e, DEC);
  u8g.print(" f");
  u8g.print(f, DEC);
  u8g.print(" c");
  u8g.print(contrast, DEC);

  */

  DrawRoundScale(12, 12, 12, alpha, 1, 2,5,3);
  DrawRoundScale(41, 12, 12, alpha, 4, 2,5,3);
  DrawRoundScale(70, 12, 12, alpha, 2, 2,5,3);
  //DrawRoundScale(12, 36, 12, alpha, 3, 2,5,3);
  //DrawRoundScale(36, 36, 12, alpha, 2, 2,5,3);
  
DrawRoundScale(48, 47, 18, map(ADC1,0,1025,0,180),1,2,5,3);

  u8g.setPrintPos(0, 7);
  //u8g.print("xr" );
  //u8g.print(rx, 0);
  //u8g.print(" ry ");
  //u8g.print(ry, 0);
  //u8g.print("alpha =");
  //u8g.print(alpha, DEC);
  //u8g.setPrintPos(0,48);
  //u8g.print(alphaRad,DEC);

  //u8g.drawLine(x1, y1, x, y);
  //u8g.drawLine(13, 13, 30, 33);
  //u8g.drawLine(40, 40, 15, 15);




#ifdef fonttest
  char lines = 1;
  for (lines = 1; lines < 7; lines++)
  {
    u8g.setPrintPos(0, 7 * lines);
    u8g.print("Test5678901234567");
  }
#endif
  //u8g.drawStr( 0,40 , "test");

}
