#include <Arduino.h>
#include <TFT_eSPI.h>
#include "GUI.h"
#include "FS.h"

#include "Wire.h"

#include <SPI.h>
#include "Adafruit_MAX31865.h"

#define CALIBRATION_FILE "/TouchCalData1"
#define REPEAT_CAL false


// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0


const int selectPin = 15;
const int dataPin   = 12;
const int clockPin  = 14;

Adafruit_MAX31865 thermo = Adafruit_MAX31865(15, 13, 12, 14);

const int red_led = 25, green_led = 26, ssr = 27, buzzer = 16, cooling_fan = 27, h_fan = 21;

const int SSR_FREQ = 10, ssr_channel = 0, resolution = 8;

int seconds_elapsed = 0, seconds_previous_time = 0; //Keeps track of the seconds elaspsed

void r_led(bool input);
void g_led(bool input);
void PID_control(float s_p);
void heat_fan(bool input);



//PID variables
float PID_p, PID_i, PID_d, set_point, PID_error, PID_prev_error, PID_total;
float kp = 10, kd = 10, ki = 0; 
int refresh_rate = 250;

int ssr_pwm_value;

float current_temp, target_temp;

unsigned long int elapsed_time, previous_time = 0, temperature_time = 0;

//Bake variables
int bake_temp, bake_time, bake_time_left;



// put function declarations here:
void touch_calibrate();

enum Reflow_Mode {
  PREP,
  PREHEAT,
  SOAK,
  REFLOW,
  COOLING
};
enum Reflow_Mode r_mode = PREHEAT;

enum States {
  NONE,
  HOME,
  PROFILES,
  PROFILE_START_SCREEN_1,
  PROFILE_START_SCREEN_LEAD,
  REFLOW_1,
  REFLOW_LEAD,
  DONE,
  BAKE_START_SCREEN,
  BAKE
};
enum States state, prior_state;


TFT_eSPI tft = TFT_eSPI(240,320);
bool pressed;
uint16_t t_x, t_y;

void setup() {
  // put your setup code here, to run once:
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(ssr, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(cooling_fan, OUTPUT);
  pinMode(h_fan, OUTPUT);
  ledcSetup(ssr_channel, SSR_FREQ, resolution);
  ledcAttachPin(ssr, ssr_channel);
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  tft.setSwapBytes(true);
  thermo.begin(MAX31865_3WIRE);

  touch_calibrate();

  prior_state = NONE;
  state = HOME;

  Serial.begin(9600);

  digitalWrite(buzzer, HIGH);
}




void loop() {

  //Place get touch cordinate here
  pressed = tft.getTouch(&t_x, &t_y);
  
  
  //Place function that would use the touch inputs to change states here
  if (pressed){
    // tft.setCursor(61, 260, 2);
    // tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(1);
    // tft.print(t_x); tft.print(", "); tft.println(t_y); 
    if (state == HOME){
      if (t_x >= 45 &&  t_x <= 194 && t_y >= 150 && t_y <= 192){         //Profile Button
        state = PROFILES;
      }
      if (t_x >= 45 &&  t_x <= 194 && t_y >= 201 && t_y <= 243){         //Profile Button
        state = BAKE_START_SCREEN;
      }
    }
    
    else if (state == BAKE_START_SCREEN){
      if (t_x >= 9 &&  t_x <= 55 && t_y >= 94 && t_y <= 136){         //Temp - Button
        bake_temp -= 5;
        tft.setTextDatum(TC_DATUM);
        tft.drawString(" " + String(bake_temp) + " " , 120, 107);
      }

      if (t_x >= 182 &&  t_x <= 228 && t_y >= 94 && t_y <= 136){         //Temp + Button
        bake_temp += 5;
        tft.setTextDatum(TC_DATUM);
        tft.drawString(" "  + String(bake_temp) + " " , 120, 107);
      }

      if (t_x >= 9 &&  t_x <= 55 && t_y >= 177 && t_y <= 218){         //Time - Button
        bake_time -= 5;
        tft.setTextDatum(TC_DATUM);
        tft.drawString(" "  +String(bake_time) + " " , 120, 187);
      }
      if (t_x >= 182 &&  t_x <= 228 && t_y >= 177 && t_y <= 218){         //Time + Button
        bake_time += 5;
        tft.setTextDatum(TC_DATUM);
        tft.drawString(" " + String(bake_time)+ " " , 120, 187);
      }
      if (t_x >= 129 &&  t_x <= 223 && t_y >= 237 && t_y <= 279){         //START BUTTON
        state = BAKE;
      }
      if (t_x >= 73 &&  t_x <= 110 && t_y >= 237 && t_y <= 279){         //ABORT BUTTON
        state = HOME;
      }
    }

    else if (state == BAKE){
      if (t_x >= 73 &&  t_x <= 166 && t_y >= 237 && t_y <= 279){         //START BUTTON
        state = BAKE_START_SCREEN;
      }
    }

    else if (state == PROFILES){
      if (t_x >= 46 &&  t_x <= 194 && t_y >= 72 && t_y <= 120) {        //Profile 1 Button
        state = PROFILE_START_SCREEN_1;
      }

      if (t_x >= 46 &&  t_x <= 194 && t_y >= 140 && t_y <= 188) {        //Profile Lead Button
        state = PROFILE_START_SCREEN_LEAD;
      }

      if (t_x >= 46 &&  t_x <= 194 && t_y >= 213 && t_y <= 254) {       //HOME Button
        state = HOME;
      }
    }

    else if (state == PROFILE_START_SCREEN_1 ){
      if (t_x >= 13 &&  t_x <= 106 && t_y >= 248 && t_y <= 288) {       //ABORT Button
        state = PROFILES;
      }
      if (t_x >= 134 &&  t_x <= 227 && t_y >= 248 && t_y <= 288) {      //START Button
        state = REFLOW_1;
      }

    }

    else if (state == PROFILE_START_SCREEN_LEAD ){
      if (t_x >= 13 &&  t_x <= 106 && t_y >= 248 && t_y <= 288) {       //ABORT Button
        state = PROFILES;
      }
      if (t_x >= 134 &&  t_x <= 227 && t_y >= 248 && t_y <= 288) {      //START Button
        state = REFLOW_LEAD;
      }

    }

    else if (state == REFLOW_1 || state == REFLOW_LEAD){
      if (t_x >= 134 &&  t_x <= 227 && t_y >= 248 && t_y <= 288) {      //ABORT Button
        state = HOME;
      }
      
    }

    else if (state == DONE){
      if (t_x >= 70 &&  t_x <= 168 && t_y >= 248 && t_y <= 288) {      //ABORT Button
        state = HOME;
      }

    }
  }


  elapsed_time = millis();

  switch (state){
    case HOME:
      if (prior_state != state){
        //initializing code
        tft.pushImage(0, 0, 240, 320, home);
        prior_state = state;
        ledcWrite(ssr_channel, 0);
        heat_fan(false);
        g_led(1);
        r_led(0);
        tft.setCursor(134, 262, 0);
        tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  
        tft.setTextSize(2);
        tft.print(current_temp);
        tft.println("  ");
    }
      
    if (elapsed_time - seconds_previous_time > 1000){
        seconds_previous_time = elapsed_time;
        current_temp = thermo.temperature(RNOMINAL, RREF);
        elapsed_time = millis();
        tft.setCursor(134, 262, 0);
        tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  
        tft.print(current_temp);
        tft.println("  ");
    }

      break;

    case BAKE_START_SCREEN:
      if (prior_state != state){
        //initializing code
        ledcWrite(ssr_channel, 0);
        g_led(1);
        ledcDetachPin(red_led);
        heat_fan(false);
        prior_state = state;
        tft.pushImage(0, 0, 240, 320, bake);
        tft.pushImage(17, 238, 93, 40, abort_button);
        bake_temp = 60;
        bake_time = 60;
        bake_time_left = 60;

        tft.setTextDatum(TC_DATUM);
        tft.setTextSize(3);
        tft.drawString(" " + String(bake_temp) + " ", 120, 107);
        tft.drawString(" " + String(bake_time) + " ", 120, 187);

      }


    break;

    case BAKE:
    if (prior_state != state){
        //initializing code
        ledcAttachPin(red_led, ssr_channel);
        heat_fan(true);
        g_led(0);
        prior_state = state;
        bake_time_left = bake_time;
        tft.setTextSize(3);
        tft.fillRect(0, 90, 240, 56, TFT_BLACK);
        tft.fillRect(0, 175, 240, 56, TFT_BLACK);
        tft.fillRect(0, 235, 240, 60, TFT_BLACK);
        tft.pushImage(73, 238, 93, 40, abort_button);
        tft.fillRect(16, 155, 155, 22, TFT_BLACK);
        tft.fillRect(16, 68, 155, 22, TFT_BLACK);
        tft.setCursor(17, 155, 0);
        tft.setTextColor(0x057D,TFT_BLACK, true);  
        tft.setTextSize(2);
        tft.println("TIME LEFT      ");
        tft.setCursor(17, 73, 0);
        tft.println("TEMPERATURE    ");
        seconds_elapsed = 0;
        tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  
    }

    PID_control(bake_temp);

    if (elapsed_time - seconds_previous_time >1000 ){
      bake_time_left = bake_time - seconds_elapsed / 60;
      seconds_previous_time = elapsed_time;
      current_temp = thermo.temperature(RNOMINAL, RREF);
      tft.setTextDatum(TC_DATUM);
      tft.setTextSize(3);
      tft.drawString( " " + String((int)round(current_temp)) + "/" + String(bake_temp) + "c " , 120, 107);
      tft.drawString(" " + String(bake_time_left) + "min ", 120, 187);

      seconds_elapsed++;
    }

    if (bake_time_left == 0){
      state = DONE;
    }



    break;



  case PROFILES:
    if (prior_state != state){
        //initializing code
        tft.pushImage(0, 0, 240, 320, profiles_screen);
        prior_state = state;
        ledcWrite(ssr_channel, 0);
        g_led(1);
        r_led(0);
        heat_fan(false);
      }
      


    break;

  case PROFILE_START_SCREEN_1:
    if (prior_state != state){
        //initializing code
        tft.fillScreen(TFT_BLACK);
        tft.pushImage(0, 0, 240, 58, profile_start_NC191SNL50);
        tft.pushImage(0, 62, 240, 184, profile_1_graph);
        tft.pushImage(0, 246, 240, 74, profile_start_buttons);
        prior_state = state;
        ledcWrite(ssr_channel, 0);
        g_led(1);
        r_led(0);
        heat_fan(false);
      }


    break;

  case REFLOW_1:
    if (prior_state != state){
        //initializing code
        tft.pushImage(0, 245, 240, 46, reflow_buttons);
        prior_state = state;
        ledcAttachPin(red_led, ssr_channel);
        g_led(0);
        heat_fan(false);
        seconds_elapsed = 0;
        
      }

  if (elapsed_time - seconds_previous_time > 1000){

    current_temp = thermo.temperature(RNOMINAL, RREF);
    tft.setCursor(61, 246, 2);
    tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(1);
    tft.println(seconds_elapsed);

    tft.setCursor(61, 260, 2);
    tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(1);
    tft.println(current_temp);

    tft.setCursor(62, 275, 2);
    tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(1);
    if(r_mode == PREHEAT){
      tft.println("PREHEAT   ");
    }
    else if (r_mode == SOAK){
      tft.println("SOAK      ");
    }
    else if(r_mode == REFLOW){
      tft.println("REFLOW    ");
    }
    else if(r_mode == COOLING){
      tft.println("COOLING");
    }



    int pixel_y = round((-1.0/2.0)*current_temp + 214);
    int pixel_x = round((7.0/12.0)*seconds_elapsed + 34);
    tft.drawPixel(pixel_x, pixel_y, TFT_WHITE);


    seconds_elapsed++;
    seconds_previous_time = elapsed_time;

  }

  if (seconds_elapsed <= 90){
    target_temp = (125.00/90.00)*(float)seconds_elapsed + 25;
    r_mode = PREHEAT;
  }
  else if (seconds_elapsed <= 180){
    target_temp = (5.00/18.00)*(float)seconds_elapsed + 125;
    r_mode = SOAK;
  }
  else if (seconds_elapsed <= 240 && current_temp <= 249){
    target_temp = (37.00/30.00)*(float)seconds_elapsed - 47;
    r_mode = REFLOW;
  }
  else if (seconds_elapsed > 240){
    r_mode = COOLING;
  }

  PID_control(target_temp);
  if (seconds_elapsed > 240){
    state = DONE;
  }

  break;

  case PROFILE_START_SCREEN_LEAD:
    if (prior_state != state){
        //initializing code
        tft.fillScreen(TFT_BLACK);
        tft.pushImage(0, 0, 240, 60, Lead_Reflow_title);
        tft.pushImage(0, 62, 240, 184, Lead_Reflow_graph);
        tft.pushImage(0, 246, 240, 74, profile_start_buttons);
        prior_state = state;
        ledcWrite(ssr_channel, 0);
        g_led(1);
        r_led(0);
        heat_fan(false);
      }


    break;

  case REFLOW_LEAD:
    if (prior_state != state){
        //initializing code
        tft.pushImage(0, 245, 240, 46, reflow_buttons);
        prior_state = state;
        ledcAttachPin(red_led, ssr_channel);
        g_led(0);
        heat_fan(false);
        seconds_elapsed = -10;
        r_mode = PREP;
        
      }

  if (elapsed_time - seconds_previous_time > 1000){

    current_temp = thermo.temperature(RNOMINAL, RREF);

    if (r_mode != PREP){
      int pixel_y = round((-1.0/2.0)*current_temp + 214);
      int pixel_x = round((7.0/12.0)*seconds_elapsed + 34);
      tft.drawPixel(pixel_x, pixel_y, TFT_WHITE);

      tft.setCursor(61, 246, 2);
      tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(1);
      tft.println(seconds_elapsed);
    }

    tft.setCursor(61, 260, 2);
    tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(1);
    tft.println(current_temp);

    tft.setCursor(62, 275, 2);
    tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(1);

    if(r_mode == PREP){
      ledcWrite(ssr_channel,255);
      tft.println("PREP      ");
    }
    else if(r_mode == PREHEAT){
      tft.println("PREHEAT   ");
    }
    else if (r_mode == SOAK){
      tft.println("SOAK      ");
    }
    else if(r_mode == REFLOW){
      tft.println("REFLOW    ");
    }
    else if(r_mode == COOLING){
      tft.println("COOLING");
    }


    

    seconds_elapsed++;
    seconds_previous_time = elapsed_time;

  }
  if (seconds_elapsed <= 0){
    r_mode = PREP;
  }

  else if (seconds_elapsed <= 30){
    target_temp = (75.00/30.00)*(float)seconds_elapsed + 25;
    r_mode = PREHEAT;
  }
  else if (seconds_elapsed <= 120){
    target_temp = (5.00/9.00)*(float)seconds_elapsed + 83.33;
    r_mode = SOAK;
  }
  else if (seconds_elapsed <= 210 && current_temp <= 235){
    target_temp = (17.00/18.00)*(float)seconds_elapsed + 110/3;
    r_mode = REFLOW;
  }
  else if (seconds_elapsed > 210){
    r_mode = COOLING;
  }
  if (r_mode != PREP){
    PID_control(target_temp);
  }
  
  if (seconds_elapsed > 210){
    state = DONE;
  }

  break;

  case DONE:
  if (prior_state != state){
      //initializing code
      tft.pushImage(0, 54, 240, 236, done);
      prior_state = state;
      ledcWrite(ssr_channel, 0);
      g_led(1);
      r_led(0);
      heat_fan(true);
      ledcDetachPin(red_led);
      
      current_temp = thermo.temperature(RNOMINAL, RREF);
      tft.setCursor(138, 213, 0);
      tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(2);
      tft.print(current_temp);
      tft.println("   ");  

      tone(buzzer, 2000);
      delay(500);
      noTone(buzzer);
      delay(500);
      current_temp = thermo.temperature(RNOMINAL, RREF);
      tft.setCursor(138, 213, 0);
      tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(2);
      tft.print(current_temp);
      tft.println("   ");
      tone(buzzer, 2000);
      delay(500);
      noTone(buzzer);
      delay(500);
      current_temp = thermo.temperature(RNOMINAL, RREF);
      tft.setCursor(138, 213, 0);
      tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(2);
      tft.print(current_temp);
      tft.println("   ");
      tone(buzzer, 2000);
      delay(500);
      noTone(buzzer);
      delay(500);
      current_temp = thermo.temperature(RNOMINAL, RREF);
      tft.setCursor(138, 213, 0);
      tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(2);
      tft.print(current_temp);
      tft.println("   ");
      tone(buzzer, 2000);
      delay(500);
      noTone(buzzer);
      delay(500);
      current_temp = thermo.temperature(RNOMINAL, RREF);
      tft.setCursor(138, 213, 0);
      tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(2);
      tft.print(current_temp);
      tft.println("   ");
      tone(buzzer, 2000);
      delay(500);
      noTone(buzzer);
      delay(500);
      elapsed_time = millis();
    }
    if (elapsed_time - seconds_previous_time > 1000){
      current_temp = thermo.temperature(RNOMINAL, RREF);
      tft.setCursor(138, 213, 0);
      tft.setTextColor(TFT_WHITE,TFT_BLACK, true);  tft.setTextSize(2);
      tft.print(current_temp);
      tft.println("   ");
      seconds_previous_time = millis();
    }

    break;
  
  }


  delay(10);

  }

// put function definitions here:

void r_led(bool input){
  if (input == true){
    digitalWrite(red_led, HIGH);
  }
  else {
    digitalWrite(red_led, LOW);
  }
}

void g_led(bool input){
  if (input == true){
    digitalWrite(green_led, HIGH);
  }
  else {
    digitalWrite(green_led, LOW);
  }
}

void heat_fan(bool input){
  if (input == true){
    digitalWrite(h_fan, HIGH);
  }
  else {
    digitalWrite(h_fan, LOW);
  }
}

void PID_control(float s_p){
  set_point = s_p;
  if((elapsed_time - previous_time) > refresh_rate){
    
    PID_error = set_point - current_temp;
    PID_p = kp*PID_error;
    PID_d = kd*((PID_error - PID_prev_error)/refresh_rate);

    PID_total = PID_p - PID_d;

    PID_total = map(PID_total, 0, 50 , 0, 255);
    
    if (PID_total > 255){
      PID_total = 255;
    }
    else if (PID_total < 0){
      PID_total = 0;
    }

    ledcWrite(ssr_channel, (int)PID_total);

    PID_prev_error = PID_error;
    previous_time = millis();
    
  }
  
}




void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      fs::File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    fs::File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}
