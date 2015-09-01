#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <U8glib.h>
#include <OneWire.h>
#include <IRremote.h>
#include <Time.h>
#include <DS1307RTC.h>


#define Clock

#define TempGraph
//#define StartXGraph 50
#define logsize 40
#define logtime 1 //secs 
//30 sec*45 pos = 22 min
//240 sec*45 pos = 3 hour
//#define BigTemp
//#define LoopGraph
//#define LineGraph
#define HeaterPin 13
#define DEBUG_IR_Last_Line

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI


//#define OLED_RESET 4
//Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

//#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!

// http://milesburton.com/Dallas_Temperature_Control_Library

OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)

int RECEIVE_PIN = 7;
IRrecv irrecv(RECEIVE_PIN);
decode_results results;
int a, b, c, d;


char TempLogIndex, TimeLogCounter = 0, TimeLogPosition;
signed int TempLog[logsize];
char GraphType = 1; //1-loop,2=line,3-line with limits
char MaxTempLimit = 28 , minTempLimit = 22;
char StartXGraph;
char aac = 0;
char aab;
char HeaterState = 0;
float celsius;


void setup(void) {
  //pin setup for ds18b20 - power from 8 pin
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  //pin setup for irreciver - power from d5 pin gnd - d6 amd siglai is d7 pin
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);

  //heater
  pinMode(HeaterPin, OUTPUT);
  digitalWrite(HeaterPin, LOW);

  //U8glib setup
  u8g.setColorIndex(1);         // pixel on


  //Serial.begin(9600);
  irrecv.enableIRIn();

  //  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3c (for the 128x64)
  //  display.setRotation(0); //(0 -normal, 2 - rotated 180)
  //  display.display();


  //  delay(100);
  // Clear the buffer.
  //  display.clearDisplay();


}
tmElements_t tm;
void loop(void) {
  u8g.firstPage();

  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];


  StartXGraph = u8g.getWidth() - logsize - 2;

  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    //Serial.println("CRC is not valid!");
    return;
  }


  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //     Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //     Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //     Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //     Serial.println("Device is not a DS18x20 family device.");
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end


#ifdef Clock
  if (RTC.read(tm)) {
    //    Serial.print("Ok, Time = ");
    //    print2digits(tm.Hour);
    //    Serial.write(':');
    //    print2digits(tm.Minute);
    //    Serial.write(':');
    //    print2digits(tm.Second);
    //    Serial.print(", Date (D/M/Y) = ");
    //    Serial.print(tm.Day);
    //    Serial.write('/');
    //   Serial.print(tm.Month);
    //    Serial.write('/');
    //    Serial.print(tmYearToCalendar(tm.Year));
    //    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      //      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      //      Serial.println("example to initialize the time and begin running.");
      //      Serial.println();
    } else {
      //      Serial.println("DS1307 read error!  Please check the circuitry.");
      //      Serial.println();
    }
  }
#endif
  delay(600);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  //  Serial.print("  Data = ");
  //  Serial.print(present, HEX);
  //  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //    Serial.print(data[i], HEX);
    //    Serial.print(" ");
  }
  //  Serial.print(" CRC=");
  //  Serial.print(OneWire::crc8(data, 8), HEX);
  //  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;


  //#ifdef BigTemp
  //  display.setCursor(2, 19);
  //  display.setTextSize(2);
  //  display.println(celsius, 1);
  //  display.setTextSize(1);
  //  display.println(" Celsius degree");
  //#endif

  //кольцевой график темпиратуры
  TimeLogCounter++;
  if (TimeLogCounter > logtime)
  {
    TimeLogCounter = 0;
    if (GraphType == 1) {
      TempLogIndex++;
      if (TempLogIndex > logsize - 1)
      {
        TempLogIndex = 0;
      }

      TempLog[TempLogIndex] = (int)(celsius * 10);
    }
    if (GraphType == 2) {
      for (TimeLogPosition = 0; TimeLogPosition < logsize-1 ; TimeLogPosition++)
      {
        TempLog[TimeLogPosition] = TempLog[TimeLogPosition + 1];
      }
      TempLog[logsize-1] = (int)(celsius * 10);
    }
  }
  //конец кольцевой график темпиратуры

#ifdef LineGraph //линейный график
#endif

  //#endif
  //конец линейный график
#ifdef LineGraph //линейный график
  //#ifdef LineGraph //линейный график с лимитами
  if (GraphType == 3) {
    TimeLogCounter++;
    //char MaxTempLimit,minTempLimit;
    char TimeLogPosition;
    //horisontal lines
    //low
    display.drawLine((StartXGraph), (u8g.getHeight() - 2 ) , (StartXGraph + logsize), (u8g.getHeight() - 2), WHITE);
    //hi
    display.drawLine((StartXGraph), (u8g.getHeight() - 2 - 30) , (StartXGraph + logsize), (u8g.getHeight() - 2 - 30), WHITE);
    //end horisontal lines

    //vertical lines
    display.drawLine((StartXGraph), (u8g.getHeight() - 2 ) , (StartXGraph), (u8g.getHeight() - 2 - 30), WHITE);
    display.drawLine((StartXGraph + logsize), (u8g.getHeight() - 2 ) , (StartXGraph + logsize), (u8g.getHeight() - 2 - 30), WHITE);
    //end vertical lines
    //написали минимум
    display.setCursor((StartXGraph - 14), u8g.getHeight() - 2 - 8);
    display.setTextSize(1);
    display.print(minTempLimit, DEC);
    //написали максимум
    display.setCursor((StartXGraph - 14), u8g.getHeight() - 2 - 3 - 30);
    display.setTextSize(1);
    display.print(MaxTempLimit, DEC);


    //осталось нарисовать сам график - не забыть пересчитать масштаб.
    //display.drawLine((StartXGraph), (u8g.getHeight()- logsize+1 ) , (StartXGraph + TempLogIndex), (u8g.getHeight()- 1), WHITE);
    for (TimeLogPosition = 0; TimeLogPosition < logsize ; TimeLogPosition++)
    {
      TempLog[TimeLogPosition] = TempLog[TimeLogPosition + 1];
    }
    TempLog[logsize] = (int)(celsius * 10);







    char aab;
    int Ypos;
    for (aab = 0; aab < logsize ; aab++)
    {
      //высота графика 30 пикселей
      //макслимит-минлимит = размер графика
      //30/размер графика = цена деления графика
      //(TempLog[aab] / 10)-минлимит = количесво делений в этой позиции
      //((TempLog[aab] / 10)-минлимит) * цена деления = высота точки
      //(((TempLog[aab] / 10)-minTempLimit) * (30/(MaxTempLimit-minTempLimit)))
      //




      Ypos = ((u8g.getHeight() - 2) - (((TempLog[aab] / 10) - minTempLimit) * (30 / (MaxTempLimit - minTempLimit))));
      if ( Ypos >= (u8g.getHeight() - 2))
      {
        Ypos = (u8g.getHeight() - 3);
      }
      if ( Ypos <= (u8g.getHeight() - 2 - 30))
      {
        Ypos = (u8g.getHeight() - 2 - 29);
      }
#ifdef graphdebug
      if (aab == logsize - 1)
      {
        display.setCursor((u8g.getWight() - 125), u8g.getHeight() - 2 - 24);
        display.setTextSize(1);
        display.print((u8g.getHeight() - 2 - 30) , DEC);
        display.setCursor((u8g.getWight() - 125), u8g.getHeight() - 2 - 16);
        display.print(Ypos , DEC);
        display.setCursor((u8g.getWight() - 125), u8g.getHeight() - 2 - 8);
        display.print((u8g.getHeight() - 2) , DEC);
      }
#endif
      display.drawPixel((StartXGraph + aab), Ypos, WHITE);


    }
  }
  //#endif
  //конец линейный график с лимитами



#endif
  //#define DEBUG_DISP_Last_Line
#ifdef DEBUG_DISP_Last_Line



  int aaa ;
  aaa = (int)(celsius * 1000);
  // display.setCursor(display.width()/2,display.height()-9);

  display.setCursor(2, u8g.getHeight() - 9);
  display.print(".");
  display.print(aaa % 1000);
  display.print(" ");
#endif

#ifdef DEBUG_IR_Last_Line
  if (irrecv.decode(&results))
  {
    //Serial.println("0x");
    u8g.setPrintPos(4, (u8g.getHeight() - 9));  // Устанавливаю курсор печати
    //display.setCursor(4, u8g.getHeight()- 9);
    u8g.print(results.value, HEX);
    u8g.print(" ");

    //char GraphType = 1; //1-loop,2=line,3=maxmin limits
    switch (results.value)
    {
      //режим графика
      case 0xFFC23D:
        {
          u8g.print("<>");
          GraphType = 1;
          break;
        }
      case 0xFF02FD:
        {
          u8g.print("->");
          GraphType = 2;
          break;
        }
      case 0xFF22DD:
        {
          u8g.print("(-)");
          GraphType = 3;
          break;
        }
      //конец режим графика
      //max temp limit correction
      case 0xFFA25D:
        {
          u8g.print("X+");
          MaxTempLimit++;
          break;
        }
      case 0xFF629D:
        {
          u8g.print("m-");
          MaxTempLimit--;
          break;
        }
      //endmax temp limit correction
      //min temp limit correction
      case 0xFFE01F:
        {
          u8g.print("m+");
          minTempLimit++;
          break;
        }
      case 0xFFA857:
        {
          u8g.print("X-");
          minTempLimit--;
          break;
        }
      //end min temp limit correction
      //switch Heater
      case 0xFFE21D:
        {
          if (HeaterState == 0)
          {
            u8g.print("on");
            HeaterState = 1;
            digitalWrite(HeaterPin, HIGH);
            break;
          }
          if (HeaterState == 1)
          {
            u8g.print("oFF");
            HeaterState = 0;
            digitalWrite(HeaterPin, LOW);
            break;
          }
          break;
        }
        //End switch heater

    }
    delay(50);
    irrecv.resume();
  }
#endif

  do {
    draw();
  } while ( u8g.nextPage() );

}//loop end
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
void print2screen(int number) {
  if (number >= 0 && number < 10) {
    u8g.print('0');
  }
  u8g.print(number);
}
/*void TemLogPrint(void)
{
  display.setCursor((u8g.getWight()- 125), u8g.getHeight()- 2 - 16);
  display.setTextSize(1);
  display.print(aac , DEC);
}
*/
void draw(void) {
  // graphic commands to redraw the complete screen should be placed here
  // u8g.setFont(u8g_font_6x10);
  //u8g.setFont(u8g_font_osb21);
  //u8g.setPrintPos(4, 12);  // Устанавливаю курсор печати
  //u8g.print("ultrasonic"); // Печатаю строчку


  //  u8g.drawStr( 4, 12, "Hello World!");
  u8g.drawFrame(0, 0, u8g.getWidth(), 16);
  u8g.drawFrame(0, 16, u8g.getWidth(), (u8g.getHeight() - 16));

#ifndef BigTemp
  u8g.setPrintPos(4, 12);
  //display.setCursor(4, 4);
  //display.setTextSize(1);
  u8g.print("T=");
  u8g.print(celsius, 1);
  u8g.print(" ");
  //display.println(" |");
  //display.setCursor(4, 4);
  if (HeaterState == 0)
  {
    //u8g.setFont(u8g_font_unifont_75r);
    u8g.setFont(u8g_font_unifont_76);
    char rrrrrr = 0x29;//empty doted circle
    //char rrrrrr = 0x4c;//empty doted circle
    u8g.print(rrrrrr);
    //u8g.drawCircle(50, 7, 4);
    u8g.setFont(u8g_font_6x10);
  }
  if (HeaterState == 1)
  {
    u8g.setFont(u8g_font_unifont_76);
    char rrrrrr = 0x35;// circle
    
    //u8g.setFont(u8g_font_unifont_75r);
    //char rrrrrr = 0x4F;// circle
    u8g.print(rrrrrr);
    //u8g.drawCircle(50, 7, 4);
    u8g.setFont(u8g_font_6x10);

//    u8g.drawDisc(50, 7, 4);
  }

  //display.setTextSize(1);
  //  display.println(" Celsius degree");
#endif
#ifdef Clock //писать сверху время
  u8g.setFont(u8g_font_6x10);
  u8g.setPrintPos(76, 12);
  print2screen(tm.Hour);
  //u8g.print(tm.Hour);
  u8g.print( ":");
  print2screen(tm.Minute);
  //u8g.print(tm.Minute);
  u8g.print(":");
  print2screen(tm.Second);
  //u8g.print(tm.Second);

  //print2screen(tm.Second);
#endif

  u8g.setPrintPos(4, 50);
  u8g.print(GraphType, DEC);
  //кольцевой график темпиратуры
  if (GraphType == 1) {
    for (aab = 0; aab < logsize ; aab++)
    {
      u8g.drawPixel((StartXGraph + aab), ((u8g.getHeight() - 2) - TempLog[aab] / 10));
    }
    u8g.drawLine((StartXGraph + TempLogIndex), (u8g.getHeight() - 40) , (StartXGraph + TempLogIndex), (u8g.getHeight() - 10));
  }
  if (GraphType == 2)
  {

    for (aab = 0; aab < logsize ; aab++)
    {
      u8g.drawPixel((StartXGraph + aab), ((u8g.getHeight() - 2) - TempLog[aab] / 10));
    }
  }
  //конец кольцевой график


}
