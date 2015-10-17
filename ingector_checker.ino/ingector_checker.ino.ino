const char ledPin =  13;      // the number of the LED pin
const char IngectorPin =  12;      // the number of the Ingector pin
const char enablePin =  5;      // the number of the LED pin
void setup() {
  // initialize serial communications at 9600 bps:
//#define serial
#ifdef serial
  Serial.begin(9600);
#endif
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  pinMode(IngectorPin, OUTPUT);
  pinMode(enablePin, INPUT_PULLUP);
 
 // initialize the pushbutton pin as an input:
 // pinMode(buttonPin, INPUT);
}
void loop() {
  // put your main code here, to run repeatedly:
  // read the input on analog pin 0:
  unsigned int sensorValue0 = analogRead(A0); //work 
    // map it to the range of the analog out:
unsigned int   outputValue0 = (map(sensorValue0, 0, 1023, 36, 400));
//if (outputValue0 < 35)
//{ outputValue0 = 35;}
//unsigned int   outputValue0 = map(sensorValue0, 0, 1023, 0, 1023);
  // read the input on analog pin 1:
  int sensorValue1 = analogRead(A1);//relax
  unsigned int outputValue1 = (map(sensorValue1, 0, 1023, 0, 100));
#define ofvmult 25

#ifdef serial

  Serial.print((outputValue0*ofvmult)/1000);
  Serial.print(".");
  Serial.print((outputValue0*ofvmult)%1000);
  Serial.print("ms reax= ");
  Serial.print(outputValue1);
  Serial.print(" ms ");
  Serial.println (outputValue0*ofvmult);
#endif
 char buttonState = digitalRead(enablePin);
 if (buttonState == HIGH) {
digitalWrite(IngectorPin, HIGH);
digitalWrite(ledPin, HIGH);
delayMicroseconds(outputValue0*ofvmult);
digitalWrite(IngectorPin, LOW);
digitalWrite(ledPin, LOW);
 }
delay(outputValue1);


}
