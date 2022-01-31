#include <Servo.h>
#include <LiquidCrystal_I2C.h>

Servo falcon1, falcon2;
const int speed1 = A0, speed2 = A1, forward1 = A2, backward1 = A3, forward2 = 3, backward2 = 4, both = 2;
int direction1 = 0, direction2 = 0, power1 = 0, power2 = 0, lastPower1 = 1, lastPower2 = 1, lastDirection1 = 1, lastDirection2 = 1, powerPercentage1 = 1, powerPercentage2 = 1, useBoth = 0; 
long lastOperationTime = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setUpScreen(){
  lcd.clear();
  lcd.print("1: ");
  lcd.setCursor(8, 0);
  lcd.print("RPM");
  lcd.setCursor(15, 0);
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  lcd.print("2: ");
  lcd.setCursor(8, 1);
  lcd.print("RPM");
  lcd.setCursor(15, 1);
  lcd.print("%");
}

void readSpeeds(){
  power1 = analogRead(speed1);
  power2 = analogRead(speed2);
  if(useBoth == 1){
    power2 = power1;
  }
  powerPercentage1 = map(power1, 1023, 0, 0, 100);
  powerPercentage2 = map(power2, 1023, 0, 0, 100);
}

void readToggles(){
  // reading toggle 1 direction
  if(!digitalRead(forward1)){
    direction1 = 1;
  }else if(!digitalRead(backward1)){
    direction1 = 2;
  }else{
    direction1 = 0;
  }

  // reading toggle 2 direction
  if(!digitalRead(forward2)){
    direction2 = 1;
  }else if(!digitalRead(backward2)){
    direction2 = 2;
  }else{
    direction2 = 0;
  }
  useBoth = !digitalRead(both);
}

void writePowerToControllers(){
  // getting output for controller 1
  int maxOut = 1500;
  if(direction1 == 1){
    maxOut = 2000;
  }else if(direction1 == 2){
    maxOut = 1000;
  }
  int output1 = map(power1, 1023, 0, 1500, maxOut);

  // getting output for controller 2
  maxOut = 1500;
  if(direction2 == 1){
    maxOut = 2000;
  }else if(direction2 == 2){
    maxOut = 1000;
  }
  int output2 = map(power2, 1023, 0, 1500, maxOut);

  // writing to controllers
  falcon1.writeMicroseconds(output1);
  falcon2.writeMicroseconds(output2);
}

void showOutputPower(){
  int rpm1 = map(power1, 1023, 0, 0, 6380);
  int rpm2 = map(power2, 1023, 0, 0, 6380);
  
  //clear screen
  for(int x = 3; x < 7; x++){
    if(x > 3){
      lcd.setCursor(x + 8, 0);
      lcd.print(" ");
      lcd.setCursor(x + 8, 1);
      lcd.print(" ");
    }
    lcd.setCursor(x, 0);
    lcd.print(" ");
    lcd.setCursor(x, 1);
    lcd.print(" ");
  }

  // print motor 1 rpm
  if(rpm1 < 10){
    lcd.setCursor(6, 0);
  }else if(rpm1 < 100){
    lcd.setCursor(5, 0);
  }else if(rpm1 < 1000){
    lcd.setCursor(4, 0);
  }else{
    lcd.setCursor(3, 0);
  }
  lcd.print(rpm1);
  
  // print motor 1 power
  if(powerPercentage1 == 100){
    lcd.setCursor(12, 0);
  }else{
    lcd.setCursor(13, 0);
  }
  lcd.print(powerPercentage1);

  // print motor 1 rpm
  if(rpm2 < 10){
    lcd.setCursor(6, 1);
  }else if(rpm2 < 100){
    lcd.setCursor(5, 1);
  }else if(rpm2 < 1000){
    lcd.setCursor(4, 1);
  }else{
    lcd.setCursor(3, 1);
  }
  lcd.print(rpm2);
  
  // print motor 2 power
  if(powerPercentage2 == 100){
    lcd.setCursor(12, 1);
  }else{
    lcd.setCursor(13, 1);
  }
  lcd.print(powerPercentage2);
}

void setup() {
  pinMode(speed1, INPUT);
  pinMode(speed2, INPUT);
  pinMode(forward1, INPUT_PULLUP);
  pinMode(backward1, INPUT_PULLUP);
  pinMode(forward2, INPUT_PULLUP);
  pinMode(backward2, INPUT_PULLUP);

  pinMode(7, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(7, 0);
  digitalWrite(5, 0);

  falcon1.attach(9);
  falcon2.attach(10);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting");
  lcd.setCursor(0, 1);
  lcd.print("BumbleTester");
  delay(2000);
  setUpScreen();
}

void loop() {
  readSpeeds();
  readToggles();
  if(powerPercentage1 != lastPower1 || powerPercentage2 != lastPower2 || direction1 != lastDirection1 || direction2 != lastDirection2){
    writePowerToControllers();
    showOutputPower();
  }
  lastPower1 = powerPercentage1;
  lastPower2 = powerPercentage2;
  lastDirection1 = direction1;
  lastDirection2 = direction2;
  delay(20);
}
