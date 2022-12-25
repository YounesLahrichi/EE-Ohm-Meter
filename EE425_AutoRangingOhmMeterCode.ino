#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <Adafruit_ADS1X15.h>
//Defining the digital pins that will control the relays.
#define CH0 13
#define CH1 12
#define CH2 11
#define CH3 10
#define CH4 9
//Defining the digital pins that will control PGA.
#define D1 1
#define D0 0
//Defining an instance of the LCD
hd44780_I2Cexp lcd;
//Channel number for channel selecting of the relays.
byte ch_number;
//Amp number for amp selecting the amplification factor of the PGA.
byte amp_number;
//Creating an instance of the ADC so that it can read in data.
Adafruit_ADS1015 ads1115;
//Resistor value and array of all my resistor values.
uint32_t res;
const uint32_t res_table[5] = {390, 3900, 39000, 390000, 3900000};
//Character array to store formatted data to print onto the LCD.
char _buffer[11];

//This method selects the channel to turn on a specific relay based on "ch_number" that is passed in.
void ch_select(byte n) {
  //This switches the digital pin outputs based on "ch_number" that is passed in.
  switch(n) {
    //This is the first case which utilizes the relay connected to the 400 ohm Resistor.
    case 0:
      digitalWrite(CH0, HIGH);
      digitalWrite(CH1, LOW);
      digitalWrite(CH2, LOW);
      digitalWrite(CH3, LOW);
      digitalWrite(CH4, LOW);
      res = res_table[n];
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Range = 390 ohm ");
      delay(10);
      break;
    //This is the second case which utilizes the relay connected to the 4,000 ohm Resistor.
    case 1:
      digitalWrite(CH0, LOW);
      digitalWrite(CH1, HIGH);
      digitalWrite(CH2, LOW);
      digitalWrite(CH3, LOW);
      digitalWrite(CH4, LOW);
      res = res_table[n];
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Range = 3.9 Kohm");
      delay(10);
      break;
    //This is the third case which utilizes the relay connected to the 40,000 ohm Resistor.
    case 2:
      digitalWrite(CH0, LOW);
      digitalWrite(CH1, LOW);
      digitalWrite(CH2, HIGH);
      digitalWrite(CH3, LOW);
      digitalWrite(CH4, LOW);
      res = res_table[n];
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Range = 39 Kohm ");
      delay(50);
      break;
    //This is the fourth case which utilizes the relay connected to the 400,000 ohm Resistor.
    case 3:
      digitalWrite(CH0, LOW);
      digitalWrite(CH1, LOW);
      digitalWrite(CH2, LOW);
      digitalWrite(CH3, HIGH);
      digitalWrite(CH4, LOW);
      res = res_table[n];
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Range = 390 Kohm");
      delay(10);
      break;
    //This is the fifth and final case which utilizes the relay connected to the 4,000,000 ohm Resistor.
    case 4:
      digitalWrite(CH0, LOW);
      digitalWrite(CH1, LOW);
      digitalWrite(CH2, LOW);
      digitalWrite(CH3, LOW);
      digitalWrite(CH4, HIGH);
      res = res_table[n];
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Range = 3.9 Mohm");
      delay(10);
      break;
  }
}
//This method selects the amplification factor based on the "amp_number" that is passed in.
void amp_select(byte n) {
  switch(n) {
    //This is the first case which boosts the voltage signal into the ADC by 1.
    case 0:
      digitalWrite(D0, LOW);
      digitalWrite(D1, LOW);
      break;
    //This is the second case which boosts the voltage signal into the ADC by 10.
    case 1:
      digitalWrite(D0, HIGH);
      digitalWrite(D1, LOW);
      break;
    //This is the third and final case which boosts the voltage signal into the ADC by 100.
    case 2:
      digitalWrite(D0, LOW);
      digitalWrite(D1, HIGH);
      break;
  }
}

//This is the setup method of the arduino, This code will run once and it is intializing the start of the variables and everything.
void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  pinMode(CH0, OUTPUT);
  pinMode(CH1, OUTPUT);
  pinMode(CH2, OUTPUT);
  pinMode(CH3, OUTPUT);
  pinMode(CH4, OUTPUT);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  //Starting off with the lowest resistor case (4000000 ohm) and digitally writting it to turn on and start with that.
  ch_number = 4;
  ch_select(ch_number);
  //Starting off with the lowest amplification factor (1) and digitally writting it to start with that.
  amp_number = 0;
  amp_select(amp_number);
  //Intiallizing the ADC to start being able to read values.
  //ads1115.begin(0x48);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Powered on");
  lcd.setCursor(7,1);
  lcd.print("OL");
  delay(10000);

  if (!ads1115.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Failed to COM ADS.");
    while (1);
  }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("The ADC passed");
}

//This is the loop method of the arduino, This code will constantly run while the arduino is powered on and this is the main code of operation.
void loop() {
  //Once again we reset the amplification factor every time it enters the loop.
  amp_number = 0;
  amp_select(amp_number);
  delay(10);
  //Takes an ADC measurement.
  uint16_t adcVoltage = ads1115.readADC_SingleEnded(0);
  Serial.print(adcVoltage);
  //uint16_t adcVoltage = 15000;
  if((adcVoltage >= 39700) && (ch_number == 4)){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Range = 3.9 Mohm");
    lcd.setCursor(7,1);
    lcd.print("OL");
    delay(1000);
    return;
  }
  //If the measurement is above 2V and is not at the 4,000,000 ohm resistor relay then it will move up to a higher resistor/relay and run the loop again from the start.
  if((adcVoltage >= 39700) && (ch_number < 4)){
    ch_number++;
    ch_select(ch_number);
    delay(50);
    return;
  }
  //if the measurement is below or equal to 0.0045V and above the 400 ohm resistor then it will move down to a lower resistor/relay and run the loop again from the start.
  if((adcVoltage <= 590) && (ch_number > 0)){
    ch_number--;
    ch_select(ch_number);
    delay(50);
    return;
  }
  //This is the case where it is still below or equal to 0.0045V but it is on the 400 ohm resistor
  if(adcVoltage <= 590){
    //This amplifies the voltage by 100
    amp_number = 2;
    amp_select(amp_number);
    //Then it reads the ADC voltage again
    delay(10);
    adcVoltage = ads1115.readADC_SingleEnded(0);
    //Then value is the resistance calculation it is divided by the amplification factor at the end.
    float value = (((float)adcVoltage*res/(65536 - adcVoltage))/100);
    //Then the following if statements decide the formatting of the writting to the display whether its ohms, kohms, Mohms.
    if(value < 1000.0)
      sprintf(_buffer, "%03u.%1u Ohm ", (uint16_t)value, (uint16_t)(value*10)%10);
    else if(value < 10000.0)
      sprintf(_buffer, "%1u.%03u kOhm", (uint16_t)(value/1000), (uint16_t)value%1000);
    else if(value < 100000.0)
      sprintf(_buffer, "%02u.%02u kOhm", (uint16_t)(value/1000), (uint16_t)(value/10)%100);
    else if(value < 1000000.0)
      sprintf(_buffer, "%03u.%1u kOhm", (uint16_t)(value/1000), (uint16_t)(value/100)%10);
    else
      sprintf(_buffer, "%1u.%03u MOhm", (uint16_t)(value/1000000), (uint16_t)(value/1000)%1000);
    //Moves the cursor to the second line and prints the resistance value and returns to run the loop again.
    lcd.setCursor(0, 1); 
    lcd.print(_buffer);
    //Serial.println(_buffer);
    //Serial.println();
    delay(50);
    return;
  }
  //This is the case where it is still between 0.0045V and 0.045V and on any channel applicable.
  if(adcVoltage >= 590 && adcVoltage <= 5800){
    //This amplifies the voltage by 10
    amp_number = 1;
    amp_select(amp_number);
    //Then it reads the ADC voltage again
    delay(10);
    adcVoltage = ads1115.readADC_SingleEnded(0);
    //Then value is the resistance calculation it is divided by the amplification factor at the end.
    float value = (((float)adcVoltage*res/(65536 - adcVoltage))/10);
    //Then the following if statements decide the formatting of the writting to the display whether its ohms, kohms, Mohms.
    if(value < 1000.0)
      sprintf(_buffer, "%03u.%1u Ohm ", (uint16_t)value, (uint16_t)(value*10)%10);
    else if(value < 10000.0)
      sprintf(_buffer, "%1u.%03u kOhm", (uint16_t)(value/1000), (uint16_t)value%1000);
    else if(value < 100000.0)
      sprintf(_buffer, "%02u.%02u kOhm", (uint16_t)(value/1000), (uint16_t)(value/10)%100);
    else if(value < 1000000.0)
      sprintf(_buffer, "%03u.%1u kOhm", (uint16_t)(value/1000), (uint16_t)(value/100)%10);
    else
      sprintf(_buffer, "%1u.%03u MOhm", (uint16_t)(value/1000000), (uint16_t)(value/1000)%1000);
    //Moves the cursor to the second line and prints the resistance value and returns to run the loop again.
    lcd.setCursor(0, 1);
    lcd.print(_buffer);
    Serial.println(_buffer);
    Serial.println();
    delay(50);
    return;
  }
  //This is the case where it is still between 0.045V and 2V and on any channel applicable.
  if(adcVoltage >= 5900 && adcVoltage <= 39700){
    //Value is the resistance calculation.
    float value = (float)adcVoltage*res/(65536 - adcVoltage);
    //Then the following if statements decide the formatting of the writting to the display whether its ohms, kohms, Mohms.
    if(value < 1000.0)
      sprintf(_buffer, "%03u.%1u Ohm ", (uint16_t)value, (uint16_t)(value*10)%10);
    else if(value < 10000.0)
      sprintf(_buffer, "%1u.%03u kOhm", (uint16_t)(value/1000), (uint16_t)value%1000);
    else if(value < 100000.0)
      sprintf(_buffer, "%02u.%02u kOhm", (uint16_t)(value/1000), (uint16_t)(value/10)%100);
    else if(value < 1000000.0)
      sprintf(_buffer, "%03u.%1u kOhm", (uint16_t)(value/1000), (uint16_t)(value/100)%10);
    else
      sprintf(_buffer, "%1u.%03u MOhm", (uint16_t)(value/1000000), (uint16_t)(value/1000)%1000);
    //Moves the cursor to the second line and prints the resistance value and returns to run the loop again.
    lcd.setCursor(0, 1);
    lcd.print(_buffer);
    //Serial.println(_buffer);
    //Serial.println();
    delay(50);
    return;
  }
}

