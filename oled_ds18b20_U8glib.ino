#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h> //to many memoru used!
//#include <Adafruit_SSD1306.h>
#include <U8glib.h>
#include <OneWire.h>
#include <IRremote.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <EEPROM.h>


#define Clock
#define TempGraph
//#define StartXGraph 50
#define logsize 50
//#define Params[12] 1 //secs
//30 sec*45 pos = 22 min
//240 sec*45 pos = 3 hour
//#define BigTemp
//#define LoopGraph
//#define LineGraph
#define HeaterPin 13
#define IR_Keys

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI

//begin of bitmap images
//u8g.drawBitmapP( 0, 0, 1, 8, rook_bitmap);
//x: X-position (left position of the bitmap).
//y: Y-position (upper position of the bitmap).
//cnt: Number of bytes of the bitmap in horizontal direction. The width of the bitmap is cnt*8.
//h: Height of the bitmap.
//pointer name(BMP_Hot_Heater)
const uint8_t BMP_Hot_Termometr[] U8G_PROGMEM = {
  0b00100010,
  0b01100111,
  0b00100010,
  0b01100000,
  0b00100000,
  0b01010000,
  0b01110000,
  0b00100000

};

const uint8_t BMP_Cold_Termometr[] U8G_PROGMEM = {
  0b00100000,
  0b01100000,
  0b00100000,
  0b01100000,
  0b00100000,
  0b01010111,
  0b01110000,
  0b00100000

};
const uint8_t BMP_Termometr[] U8G_PROGMEM = {
  0b00100010,
  0b01100101,
  0b00100100,
  0b01100101,
  0b00100010,
  0b01010000,
  0b01110000,
  0b00100000

};
const uint8_t BMP_Hot_Heater[] U8G_PROGMEM = {
  0b00000000,
  0b01000010,
  0b10000100,
  0b01000010,
  0b10000100,
  0b01000010,
  0b11111111,
  0b01100110

};
const uint8_t BMP_Cold_Heater[] U8G_PROGMEM = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b01111110,
  0b11111111,
  0b01100110

};
const uint8_t BMP_GoodTemp[] U8G_PROGMEM = {
  0b00100010,
  0b01100111,
  0b00100010,
  0b01100000,
  0b00100000,
  0b01010111,
  0b01110000,
  0b00100000


};




//end of bitmap images
//#define OLED_RESET 4
//Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


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


int address = 0; //for EEProm


char TempLogIndex, TimeLogCounter = 0, TimeLogPosition;
signed int TempLog[logsize];
char StartXGraph;
char aac = 0;
char aab;
//char HeaterState = 0;
//char GraphTop  = 40; //ToDo: написать изменение настройки - пока жестко фиксируется
//char TimeLogPosition;
float celsius;
char Params[13] =
{ 0, 0,
  1, 10, 30, 20,//график
  31, 29, 1, //термостат
  0,//нагрузка
  1, 1,
  5
  // 0-menu pos,1-признак меню,
  //2-тип графика,3-высота графика,4-максимум графика,5-минимум графика,
  //6-выключение термостата,7-включение термостата,8-темостат используется?,
  //9-нагрузка сейчас включена?
  //,10-показывать пределы термостатирования,11-показывать перделы графика
  //12 - LogTime(сек)
};
const char * const MenuParamsName[] PROGMEM = {" nop", " nop1", "GType", "GHeight", "GMax", "Gmin", "T Off", "T ON", "TInUse", "LoadOn", "Sh T", "Sh G", "LogSec", "@EOM"};


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
  u8g.setFont(u8g_font_6x10r);

EELoad(13);

//reading EEProm start values
//for (address = 0; address<13; address++)
//{
//Params[address] = EEPROM.read(address);
//delay(50);
//}
 //Serial.begin(9600);
  irrecv.enableIRIn();

StartXGraph = u8g.getWidth() - logsize - 2;
}
tmElements_t tm;
void loop(void) {
  u8g.firstPage();

  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];




  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) { //ToDo: добавить обработчик ошибок - вдруг градусник потерялся...
    //Serial.println("CRC is not valid!");
    return;
  }


  // the first ROM byte indicates which chip
  switch (addr[0]) {//todo: написать обработку исключения
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
  if (RTC.read(tm)) { //todo: нужно сделать настройку часов

  } else {
    if (RTC.chipPresent()) {//todo: добавить исключение
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


  //кольцевой график темпиратуры
  TimeLogCounter++;
  if (TimeLogCounter > Params[12])
  {
    TimeLogCounter = 0;
    if (Params[2] == 1) {
      TempLogIndex++;
      if (TempLogIndex > logsize - 1)
      {
        TempLogIndex = 0;
      }

      TempLog[TempLogIndex] = (int)(celsius * 10);
    }
    if (Params[2] == 2) {
      for (TimeLogPosition = 0; TimeLogPosition < logsize - 1 ; TimeLogPosition++)
      {
        TempLog[TimeLogPosition] = TempLog[TimeLogPosition + 1];
      }
      TempLog[logsize - 1] = (int)(celsius * 10);
    }
    if (Params[2] == 3) {
      for (TimeLogPosition = 0; TimeLogPosition < logsize - 1 ; TimeLogPosition++)
      {
        TempLog[TimeLogPosition] = TempLog[TimeLogPosition + 1];
      }
      TempLog[logsize - 1] = (int)(celsius * 10);
    }
  }
  //конец кольцевой график темпиратуры

  
  //включение-выключение термостата
    //6-выключение термостата,7-включение термостата,8-темостат используется?,
  if (Params[8] == 1)
  {
    if (Params[7] > celsius)
    {
      Params[9] = 1;
    }
    if (Params[6] < celsius)
    {
      Params[9] = 0;
    }
  }
  
  //включение-выключение нагрузки
  if (Params[9] == 1)
  {
    digitalWrite(HeaterPin, HIGH);
  }
  if (Params[9] == 0)
  {
    digitalWrite(HeaterPin, LOW);
  }
  //конец включение-выключение нагрузки

#ifdef IR_Keys
  if (irrecv.decode(&results))
  {
    // Param[2] = 1; //1-loop,2=line,3=maxmin limits
    switch (results.value)
    {
      //режим графика
      case 0xFFC23D:
        {
          //TODO:нарисовать тип графика
          //u8g.print("<>");
          //GraphType = 1;
          break;
        }
      case 0xFF02FD:
        {
          //TODO:нарисовать тип графика
          //u8g.print("->");
          //GraphType = 2;
          break;
        }
      case 0xFF22DD:
        {
          //TODO:нарисовать тип графика
          //u8g.print("(-)");
          //GraphType = 3;
          break;
        }
      //конец режим графика
      //max temp limit correction
      case 0xFFA25D:
        {
          //u8g.print("X-");
          Params[4]--;
          break;
        }
      case 0xFF629D:
        {
          Params[4]++;
          break;
        }
      //endmax temp limit correction
      //min temp limit correction
      case 0xFFE01F:
        {
          Params[5]--;
          break;
        }
      case 0xFFA857:
        {
          //u8g.print("X+");
          Params[5]++;
          break;
        }
      //end min temp limit correction
      //switch Heater
      case 0xFFE21D:
        {
          if (Params[9] == 0)
          {
            Params[9] = 1;
            break;
          }
          if (Params[9] == 1)
          {

            Params[9] = 0;
            break;
          }
          break;
        }
      //End switch heater

      case 0xFF52AD:
        {
          if (Params[0] == 0)
          {

            Params[0] = 1;

            break;
          }
          if (Params[0] >= 1)
          {

            Params[0] = 0;

            break;
          }

          break;

        }
      case 0xFF5AA5://6
        {
          if (Params[0] < 25) { //max 25 position 1 lvl menu
            Params[0]++;
          }
          break;
        }
      case 0xFF10EF://4
        {
          if (Params[0] > 1) {
            Params[0]--;
          }
          break;
        }
      case 0xFF18E7://2
        {
          if (Params[0] > 1) {
            Params[Params[0]]++;
          }

          break;
        }
      case 0xFF4AB5://8
        {
          if (Params[0] > 1) {
            Params[Params[0]]--;
          }

          break;
        }
        case 0xFF38C7://5 save to EEprom
        {
          Params[0] = 0;
          EESave(13);
          break;
        }

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

void draw(void) {
  if (Params[0] == 0) {
    // graphic commands to redraw the complete screen should be placed here
    // u8g.setFont(u8g_font_6x10);
    //u8g.setFont(u8g_font_osb21);
    //u8g.setPrintPos(4, 12);  // Устанавливаю курсор печати
    //u8g.print("ultrasonic"); // Печатаю строчку
    //  u8g.drawStr( 4, 12, "Hello World!");

#ifdef Oled_Frame
    u8g.drawFrame(0, 0, u8g.getWidth(), 16);
    u8g.drawFrame(0, 16, u8g.getWidth(), (u8g.getHeight() - 16));
#endif

#ifdef Nokia_Frame
    //nokia size screen frame
    u8g.drawFrame(0, 0, 84, 48);
#endif

#ifndef BigTemp
    u8g.drawBitmapP( 3, 4, 1, 8, BMP_Termometr);
    u8g.setPrintPos(13, 12);

    if (celsius >= 0)
    {
      u8g.print(celsius, 1);
    } else {
      u8g.print(celsius, 0);
    }
    if (Params[9] == 0)
    {
      u8g.drawBitmapP( 39, 4, 1, 8, BMP_Cold_Heater);
      //u8g.drawCircle(50, 7, 4);
    }
    if (Params[9] == 1)
    {
      u8g.drawBitmapP( 39, 4, 1, 8, BMP_Hot_Heater);
      //    u8g.drawDisc(50, 7, 4);
    }
#endif
#ifdef Clock //писать сверху время

    u8g.setPrintPos(52, 12);
    print2screen(tm.Hour);
    //u8g.print(tm.Hour);
    if (tm.Second % 2)
    {
      u8g.print( ":");
    } else
    {
      u8g.print( ".");
    }
    print2screen(tm.Minute);
    //u8g.print(tm.Minute);
    u8g.print(":");
    print2screen(tm.Second);
    //u8g.print(tm.Second);

    //print2screen(tm.Second);
#endif

    u8g.setPrintPos(4, 50);
    u8g.print(Params[2], DEC);
    //кольцевой график темпиратуры
    if (Params[2] == 1) {
      for (aab = 0; aab < logsize ; aab++)
      {
        u8g.drawPixel((StartXGraph + aab), ((u8g.getHeight() - 2) - TempLog[aab] / 10));
      }
      u8g.drawLine((StartXGraph + TempLogIndex), (u8g.getHeight() - 40) , (StartXGraph + TempLogIndex), (u8g.getHeight() - 10));
    }
    if (Params[2] == 2)
    {

      for (aab = 0; aab < logsize ; aab++)
      {
        u8g.drawPixel((StartXGraph + aab), ((u8g.getHeight() - 2) - TempLog[aab] / 10));
      }
    }
    if (Params[2] == 3)//GraphType
    {

      //horisontal lines

      //low
      u8g.drawLine((StartXGraph), (u8g.getHeight() - 2 ) , (StartXGraph + logsize), (u8g.getHeight() - 2));
      //hi
      u8g.drawLine((StartXGraph), (u8g.getHeight() - 2 - Params[3]) , (StartXGraph + logsize), (u8g.getHeight() - 2 - Params[3]));
      //end horisontal lines

      //vertical lines
      u8g.drawLine((StartXGraph), (u8g.getHeight() - 2 ) , (StartXGraph), (u8g.getHeight() - 2 - Params[3]));
      u8g.drawLine((StartXGraph + logsize), (u8g.getHeight() - 2 ) , (StartXGraph + logsize), (u8g.getHeight() - 2 - Params[3]));
      //end vertical lines

      //написали минимум
      u8g.setPrintPos((StartXGraph - 14), u8g.getHeight() - 3);
      u8g.print(Params[5], DEC);
      //написали максимум
      u8g.setPrintPos((StartXGraph - 14), u8g.getHeight() - 2 - (Params[3] - u8g.getFontAscent()));
      //display.setTextSize(1);
      u8g.print(Params[4], DEC);

      //осталось нарисовать сам график - не забыть пересчитать масштаб.
      char aab;
      char Ypos;
      char oldYpos = 0;
      for (aab = 0; aab < logsize ; aab++)
      {
        //высота графика 30 пикселей
        //макслимит-минлимит = размер графика
        //30/размер графика = цена деления графика
        //(TempLog[aab] / 10)-минлимит = количесво делений в этой позиции
        //((TempLog[aab] / 10)-минлимит) * цена деления = высота точки
        //(((TempLog[aab] / 10)-Params[5]) * (30/(Params[4]-Params[5])))
        //
        oldYpos = Ypos;
        Ypos = ((u8g.getHeight() - 2) - (((TempLog[aab] / 10) - Params[5]) * (Params[3] / (Params[4] - Params[5]))));

        if ( Ypos >= (u8g.getHeight() - 2))
        {
          Ypos = (u8g.getHeight() - 3);
        }
        if ( Ypos <= (u8g.getHeight() - 2 - Params[3]))
        {
          Ypos = (u8g.getHeight() - 2 - (Params[3] - 1));
        }
        if ( oldYpos >= (u8g.getHeight() - 2))
        {
          oldYpos = (u8g.getHeight() - 3);
        }
        if ( oldYpos <= (u8g.getHeight() - 2 - Params[3]))
        {
          oldYpos = (u8g.getHeight() - 2 - (Params[3] - 1));
        }


        u8g.drawLine((StartXGraph + aab - 1), oldYpos, (StartXGraph + aab), Ypos);
        //u8g.drawPixel((StartXGraph + aab), Ypos);


      }




    }
    //конец кольцевой график
      if (Params[8] == 1)
      {
      //6-выключение термостата,7-включение термостата,8-темостат используется?,
      u8g.setPrintPos(4, 24);
      //char * ptr = (char *) pgm_read_word (&MenuParamsName[Params[6]]);      
      //u8g.print (ptr);
      u8g.print ("T Off=");
      u8g.print(Params[6], DEC);
      u8g.setPrintPos(4, 33);
      u8g.print ("T On=");
      //char * ptr1 = (char *) pgm_read_word (&MenuParamsName[Params[7]]);      
      //u8g.print (ptr1);
      u8g.print(Params[7], DEC);
      }

  }//if Params[0] == 0 end
  else if (Params[0] > 0)
  {
    u8g.setPrintPos(13, 12);
    u8g.print("menu");

    if (Params[0] > 1) {//GraphTupe
      char * ptr = (char *) pgm_read_word (&MenuParamsName[Params[0]]);
      u8g.setPrintPos(4, 24);
      u8g.print (ptr);
      u8g.print ("=");
      u8g.print(Params[Params[0]], DEC);
      u8g.print ("|");
    }

    u8g.setPrintPos(4, 62);
    u8g.print ("ParamNum= ");
    u8g.print(Params[0], DEC);
  }

}
void EESave(int NumOfChar){
for (address = 0; address<NumOfChar; address++)
{
EEPROM[address] = Params[address];
delay(100);
//Params[aac] = EEPROM.read(aac);
}
}

void EELoad(int NumOfChar){
for (address = 0; address<NumOfChar; address++)
{
 Params[address] = EEPROM[address];
delay(100);
//Params[aac] = EEPROM.read(aac);
}
}
