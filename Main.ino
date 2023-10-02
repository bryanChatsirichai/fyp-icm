#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <math.h>
#include <EEPROM.h>
#include "SoftwareSerial.h"
#include "rp2040_pwm_audio.h"

// Motor Parameters
#define MOTOR_STEPS 200 //
#define RPM 1000
#define FOCUS 0
#define ZOOM 1
#define MS_STEP 8 // default 1/8 microstep, 200*8 = 1600 microstepping
#define CALI_ACCEL 400

//Display Pins
#define TFT_CS   9
#define TFT_RST  12  // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC   8
#define TFT_MOSI 11  // Data out
#define TFT_SCLK 10 // Clock out

/* Motor Pins */
#define rear_DIR    0
#define rear_STEP   1
#define front_DIR   6
#define front_STEP  7

//Display Joystick 
#define UP_BUTTON 2
#define DOWN_BUTTON 18
#define LEFT_BUTTON 16
#define RIGHT_BUTTON 20
#define A_BUTTON 15
#define B_BUTTON 17

/*RX and TX for JQ8900 communication*/
//It's important to note that those RX and TX labels are with respect to the device itself. 
//So the RX from one device should go to the TX of the other, and vice-versa
#define HBYTE(X) ((X & 0xFF00) >> 8) 
#define LBYTE(X) (X & 0xFF) 
#define RX 5
#define TX 4
//#define BUSY_PIN 27

/* Buzzer Signal Pin */
//#define BUZZER 22
/* Speaker Signal Pin */
//#define PIN_BUZZER 14

//To control camera shutter
#define FOCUS_CAMERA 21
#define SHUTTER_CAMERA 22



// Strings
/* Colour Strings */
#define WHITE     0xFFFF
#define BLACK     0x0000
#define AQUA      0xFEC0
#define DEEPPINK  0xF8B2  
#define DARKGREEN 0x0320
#define RED       0xF800 
#define LIGHTSKYBLUE 0x867F 
#define SNOW 0xFFDF
#define VIOLET 0xEC1D 
#define YELLOWGREEN 0x9E66
#define GOLDENROD 0xF81F
#define AZURE 0xF7FF
#define CORAL 0xFBEA 
#define CADETBLUE 0x5CF4
#define LIME 0x07E0

/* Symbols */
#define UP_ARROW    0x18
#define DOWN_ARROW  0x19
#define RIGHT_ARROW 0x1A
#define LEFT_ARROW  0x1B
#define SELECT      0x2A

/* Defining Strings on Display*/
const char back[] PROGMEM = "Back";

//home_screen options focus on configuration,
const char home_header[] PROGMEM = "|- Home Menu -|";
const char home_0[] PROGMEM = "Camera Callibration";
const char home_1[] PROGMEM = "Camera Actions - 1";
const char home_2[] PROGMEM = "Camera Actions - 2";

//main_menu1 options focus on configuration,
const char mm_configuration_header[] PROGMEM = "|- Configuration Menu -|";
const char mm_configuration_0[] PROGMEM = "Camera Settings";
const char mm_configuration_1[] PROGMEM = "Motor Calibration";
const char mm_configuration_2[] PROGMEM = "POV Calibration";
const char mm_configuration_3[] PROGMEM = "Back Page";

////main_menu2 options focus on actions,
const char mm_action1_header[] PROGMEM = "|- Action Menu-1 -|";
const char mm_action1_0[] PROGMEM = "Zoom Movements";
const char mm_action1_1[] PROGMEM = "Focus Movements";
const char mm_action1_2[] PROGMEM = "ZoomFocus Movements";
const char mm_action1_3[] PROGMEM = "Back Page";

////main_menu3 options focus on patterns actions,
const char mm_action2_header[] PROGMEM = "|- Action Menu-2 -|";
const char mm_action2_0[] PROGMEM = "Fixed Patterns";
const char mm_action2_1[] PROGMEM = "Customise Patterns";
const char mm_action2_2[] PROGMEM = "Back Page";

/////Calibration/////
const char cs_name[] PROGMEM = "|- Camera Settings -|";
const char cs_0[] PROGMEM = "Zoom/Focus Position";
const char cs_1[] PROGMEM = "Shutter Time"; //Camera Shutter Time, Shutter Speed
const char cs_2[] PROGMEM = "Motor Movement Time";
const char cs_3[] PROGMEM = "Back Page";


const char pm_name[] PROGMEM = "|- Positioning Setting -|";
const char pm_0[] PROGMEM = "Zoom at the Front";
const char pm_1[] PROGMEM = "Zoom at the Back";

const char shutter_menu[] PROGMEM = "|- Shutter Time(s) -|";
const char motor_time_menu[] PROGMEM = "Motor Move Time(s)";

const char mc1_name[] PROGMEM = "|- Calibration -|";
const char mc1_0[] PROGMEM = "Zoom Calibration";
const char mc1_1[] PROGMEM = "Focus Calibration";
const char mc1_2[] PROGMEM = "Next Page";
const char mc1_3[] PROGMEM = "Back Page";

const char mc2_name[] PROGMEM = "|- Calibration -|";
const char mc2_0[] PROGMEM = "Exposure Setting";
const char mc2_1[] PROGMEM = "RESET all values";
const char mc2_2[] PROGMEM = "Back Page";

const char cali_zoom[] PROGMEM = "|--Calibrate Zoom --|";
const char cali_focus[] PROGMEM = "|--Calibrate Focus--|";
const char string_cali[] PROGMEM = "Move joystick to extreme";
const char string_left[] PROGMEM = "  left";
const char string_right[] PROGMEM = "  right";

const char adjust_zoom[] PROGMEM = "|--- Adjust Zoom ---|";
const char adjust_focus[] PROGMEM = "|---Adjust Focus ---|";
const char string_36[] PROGMEM = "Adjust [ZOOM] lens";
const char string_36_1[] PROGMEM = "Adjust [FOCUS] lens";
const char string_37[] PROGMEM = "to the desired Image";
const char string_38[] PROGMEM = "to desired Outcome";

const char exposure_option_name[] PROGMEM = "|- Exposure -|";
const char exposure_option_0[] PROGMEM = "Pre";
const char exposure_option_1[] PROGMEM = "Split";
const char exposure_option_2[] PROGMEM = "After";
const char exposure_option_3[] PROGMEM = "Back Page";

/////Actions//////

//Focus page1
const char fm1_name[] PROGMEM = "|- Focus Movements -|";
const char fm1_0[] PROGMEM = "Focus Max";
const char fm1_1[] PROGMEM = "Focus Min";
const char fm1_2[] PROGMEM = "Focus Value";
const char fm1_3[] PROGMEM = "Next Page";
const char fm1_4[] PROGMEM = "Back Page";

//Focus page2
const char fm2_name[] PROGMEM = "|- Focus Movements -|";
const char fm2_0[] PROGMEM = "Focus Max & back";
const char fm2_1[] PROGMEM = "Focus Min & back";
const char fm2_2[] PROGMEM = "Focus Value & back";
const char fm2_3[] PROGMEM = "Back Page";

//Zoom page1
const char zm1_name[] PROGMEM = "|- Zoom Movements -|";
const char zm1_0[] PROGMEM = "Zoom Max";
const char zm1_1[] PROGMEM = "Zoom Min";
const char zm1_2[] PROGMEM = "Zoom Value";
const char zm1_3[] PROGMEM = "Next Page";
const char zm1_4[] PROGMEM = "Back Page";

//Zoom page2
const char zm2_name[] PROGMEM = "|- Zoom Movements -|";
const char zm2_0[] PROGMEM = "Zoom Max & back";
const char zm2_1[] PROGMEM = "Zoom Min & back";
const char zm2_2[] PROGMEM = "Zoom Value & back";
const char zm2_3[] PROGMEM = "Back Page";

//need seaperate as options too many

//Zoom_Focus page1
const char zf1_name[] PROGMEM = "|- Zoom&Focous Movements -|";
const char zf1_0[] PROGMEM = "Z[MAX] F[MAX]";
const char zf1_1[] PROGMEM = "Z[MIN] F[MIN]";
const char zf1_2[] PROGMEM = "ZF to Value";
const char zf1_3[] PROGMEM = "Next Page";
const char zf1_4[] PROGMEM = "Back Page";

//Zoom_Focus page2
const char zf2_name[] PROGMEM = "|- Zoom&Focous Movements -|";
const char zf2_0[] PROGMEM = "Z[MAX] F[MIN]";
const char zf2_1[] PROGMEM = "Z[Min] F[MAX]";
const char zf2_2[] PROGMEM = "Next Page";
const char zf2_3[] PROGMEM = "Back Page";

//Zoom_Focus page3
const char zf3_name[] PROGMEM = "|- Zoom&Focous Movements -|";
const char zf3_0[] PROGMEM = "Z[MAX] F[MAX] back";
const char zf3_1[] PROGMEM = "Z[MIN] F[MIN] back";
const char zf3_2[] PROGMEM = "ZF Value & back";
const char zf3_3[] PROGMEM = "Next Page";
const char zf3_4[] PROGMEM = "Back Page";

//Zoom_Focus page4
const char zf4_name[] PROGMEM = "|- Zoom&Focous Movements -|";
const char zf4_0[] PROGMEM = "Z[MAX] F[MIN] back";
const char zf4_1[] PROGMEM = "Z[MIN] F[MAX] back";
const char zf4_2[] PROGMEM = "Back Page";

//presets
const char preset1_name[] PROGMEM = "|----- Presets -----|";
const char preset1_0[] PROGMEM = "Bokeh Effect";
const char preset1_1[] PROGMEM = "Firework Effect";
const char preset1_2[] PROGMEM = "Zoom Blur Effect";
const char preset1_3[] PROGMEM = "Next Page";
const char preset1_4[] PROGMEM = "Back Page";

const char preset2_name[] PROGMEM = "|----- Presets -----|";
const char preset2_0[] PROGMEM = "Sine Wave Effect";
const char preset2_1[] PROGMEM = "ZigZag Out Effect";
const char preset2_2[] PROGMEM = "Back Page";

//
const char counttext_1[] PROGMEM = "Get Ready!";
const char counttext_2[] PROGMEM = "3";
const char counttext_3[] PROGMEM = "2";
const char counttext_4[] PROGMEM = "1";
const char counttext_5[] PROGMEM = "SNAP!";


/* String Table */
const char *const home_menu[] PROGMEM = {home_0, home_1,home_2}; //main_menu1 table
const char *const main_menu_1[] PROGMEM = {mm_configuration_0, mm_configuration_1, mm_configuration_2,mm_configuration_3}; //main_menu1 table
const char *const main_menu_2[] PROGMEM = {mm_action1_0, mm_action1_1, mm_action1_2,mm_action1_3}; //main_menu2 table
const char *const main_menu_3[] PROGMEM = {mm_action2_0, mm_action2_1, mm_action2_2}; //main_menu3 table

const char *const camera_settings_menu[] PROGMEM = {cs_0, cs_1, cs_2,cs_3};
const char *const positioning_menu[] PROGMEM = {pm_0,pm_1, back};
const char *const motor_calibration_menu1[] PROGMEM {mc1_0, mc1_1, mc1_2, mc1_3};
const char *const motor_calibration_menu2[] PROGMEM {mc2_0, mc2_1, mc2_2};
const char *const exposure_option_menu[] PROGMEM {exposure_option_0, exposure_option_1,exposure_option_2,exposure_option_3};

const char *const focus_menu1[] PROGMEM = {fm1_0,fm1_1,fm1_2,fm1_3,fm1_4};
const char *const focus_menu2[] PROGMEM = {fm2_0,fm2_1,fm2_2,fm2_3};

const char *const zoom_menu1[] PROGMEM = {zm1_0,zm1_1,zm1_2,zm1_3,zm1_4};
const char *const zoom_menu2[] PROGMEM = {zm2_0,zm2_1,zm2_2,zm2_3};

const char *const zoomfocus_menu1[] PROGMEM = {zf1_0,zf1_1,zf1_2,zf1_3,zf1_4};
const char *const zoomfocus_menu2[] PROGMEM = {zf2_0,zf2_1,zf2_2,zf2_3};
const char *const zoomfocus_menu3[] PROGMEM = {zf3_0,zf3_1,zf3_2,zf3_3,zf3_4};
const char *const zoomfocus_menu4[] PROGMEM = {zf4_0,zf4_1,zf4_2};

const char *const calizoom_left[] PROGMEM = {cali_zoom, string_cali, string_left};
const char *const calizoom_right[] PROGMEM = {cali_zoom, string_cali, string_right};

const char *const califocus_left[] PROGMEM = {cali_focus, string_cali, string_left};
const char *const califocus_right[] PROGMEM = {cali_focus, string_cali, string_right};

const char *const focus_adjust[] PROGMEM = {adjust_focus, string_36_1, string_37};
const char *const focus_dist[] PROGMEM = {fm1_2, string_36, string_38};

const char *const zoom_adjust[] PROGMEM = {adjust_zoom, string_36, string_37};
const char *const zoom_dist[] PROGMEM = {zm1_2, string_36, string_38};

const char *const preset1_menu[] PROGMEM = {preset1_0, preset1_1, preset1_2, preset1_3,preset1_4};
const char *const preset2_menu[] PROGMEM = {preset2_0, preset2_1, preset2_2};

const char *const countdown[] PROGMEM = {counttext_1, counttext_2, counttext_3, counttext_4, counttext_5};


/* Things that are saved
 *  - focus_range     (Max range of focus)
 *  - zoom_range      (Max range of zoom)
 *  - focus_current   (Current focus value)
 *  - zoom_current    (Current zoom value)
 *  - orientation     (Orientation of motors)
 *  - shutter_speed   (Shutter speed)
 *  - motor_time
 */
int zoom_range = 0; 
int focus_range = 0;    
int zoom_current = 0;     
int focus_current = 0;   
int orientation = 1;    
int shutter_time = 0;
int motor_time = 0;   
int exposure_option_set = 0; //default
int camera_shutter_open = 0; //default 0 is not open

// Global Variables
int option_selected = 0;
int updateMenu = true;
int home_screen = -1;
int configuration_screen = -1;
int camera_setting_screen = -1;
int camera_positioning_screen = -1;
int motor_calibration_screen1 = -1;
int motor_calibration_screen2 = -1;
int exposure_option_screen = -1;
int pov_calibrartion_screen = -1;
int action_screen_1 = -1;
int action_screen_2 = -1;
int zoom_movements_menu1 = -1;
int zoom_movements_menu2 = -1;
int focus_movements_menu1 = -1;
int focus_movements_menu2 = -1;
int zoom_focus_movements_menu1 = -1;
int zoom_focus_movements_menu2 = -1;
int zoom_focus_movements_menu3 = -1;
int zoom_focus_movements_menu4 = -1;
int fixed_paterns_menu1 = -1;
int fixed_paterns_menu2 = -1;

int max_option = 0;
bool firstTime = false;

/* PRGMEM Bugger */
char buffer[50];

// Object Declaration

/* Motor Objects to be edited*/
AccelStepper rear_motor(AccelStepper::DRIVER, rear_STEP, rear_DIR);
AccelStepper front_motor(AccelStepper::DRIVER, front_STEP, front_DIR);

/*Control multiple steppers motor*/
MultiStepper steppers;

/* Display Object, set up the displat init */
Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//Comminication to JQ8900
SoftwareSerial SoftwareSerial(RX,TX); // RX, TX

// Function Declaration
int menu(int array_size,const char *menu_header,const char *const string_table[], int option_selected, int footer=2, uint16_t color=DEEPPINK);
int menu_with_header(int array_size,const char *menu_name ,const char *const string_table[], int option_selected, int header, int footer=2, uint16_t color=DEEPPINK);
void hotbar(const char title[], int current, int max_range, int current_option=0, bool haveBack=false, int header=0, int footer=3, uint16_t color=WHITE, bool updateBar=false);
int getUpDown(int max_option, int current_option, int delay_ms);
int getLeftRight(int range, int current, int low_limit=0, int delay_ms=0);
int getUpdate(int s);
void caliMenu(const char *const string_table[], int current_step, int max_steps=200, uint16_t color=WHITE, bool updateBar=false);
int calibrate(int type, const char *const string_table[], int upper_limit, int lower_limit, uint16_t color=WHITE);
void moveMotorMenu(int count, const char *const string_table[], int current_step, int max_steps, uint16_t color=WHITE, bool updateBar=false);
int chooseDist(int type, int count, const char *const string_table[], bool goBack=false, uint16_t color=WHITE);
void updateScreen(float delay_ms=0);
void moveMotorMenu(int count, const char *const string_table[], int current_step, int max_steps, uint16_t color, bool updateBar);
void countdownMenu();
void printMoveSteps(int type, const char title[], uint16_t color, int goBack);
void setAccel(int type, float accel);
void setCurrentPos(int type, float value);

void goDist(int type, const char title[], int pos_desired, uint16_t color=WHITE, float motor_time = motor_time, bool goBack=true,bool lastSequence=true);
void moveMotor(int type, int pos_desired, float motor_time = motor_time);

void moveMultiMotor(int zoom_value, int focus_value, float motor_time = motor_time);
void goMultiDist(const char title[], int zoom_desired, int focus_desired, uint16_t color=WHITE, float motor_time = motor_time, bool goBack=true,bool lastSequence=true);
void goToHomeMenu();
void buzz(int delay_ms=500, int freq=1000);
void nikonTime(int exposure_time);
void open_Shutter();
void close_Shutter();
//////////////////////////////

/* Buzzer Function */
// void buzz(int delay_ms, int freq) {
//   tone(BUZZER, freq);   // Send freq(Hz) sound signal...
//   delay(delay_ms);      // ...for delay_ms
//   noTone(BUZZER);       // Stop sound...
// }

/*nikonTime*/
void nikonTime(int exposure_time) { // Controls the shutter of a Nikon camera
  digitalWrite(FOCUS_CAMERA, HIGH);  
  digitalWrite(SHUTTER_CAMERA, HIGH);
  delay(150);
  digitalWrite(FOCUS_CAMERA, LOW); // close shutter (fully pressed)
  digitalWrite(SHUTTER_CAMERA, LOW); // close shutter (fully pressed)
  delay(150);
  digitalWrite(FOCUS_CAMERA, HIGH);  
  digitalWrite(SHUTTER_CAMERA, HIGH);
  delay(exposure_time);//exposure time
  digitalWrite(FOCUS_CAMERA, HIGH);  
  digitalWrite(SHUTTER_CAMERA, HIGH);
  delay(150);
  digitalWrite(FOCUS_CAMERA, LOW); // close shutter (fully pressed)
  digitalWrite(SHUTTER_CAMERA, LOW); // close shutter (fully pressed)
  delay(150);
  digitalWrite(FOCUS_CAMERA, HIGH);  
  digitalWrite(SHUTTER_CAMERA, HIGH);
}
void open_Shutter() { // Controls the shutter of a Nikon camera
  Serial.println("Open");
  digitalWrite(FOCUS_CAMERA, HIGH);  
  digitalWrite(SHUTTER_CAMERA, HIGH);
  delay(150);
  digitalWrite(FOCUS_CAMERA, LOW);
  digitalWrite(SHUTTER_CAMERA, LOW);
  delay(150);
  digitalWrite(FOCUS_CAMERA, HIGH);  
  digitalWrite(SHUTTER_CAMERA, HIGH);
}
void close_Shutter() { // Controls the shutter of a Nikon camera
  Serial.println("Close");
  digitalWrite(FOCUS_CAMERA, HIGH);  
  digitalWrite(SHUTTER_CAMERA, HIGH);
  delay(150);
  digitalWrite(FOCUS_CAMERA, LOW);
  digitalWrite(SHUTTER_CAMERA, LOW);
  delay(150);
  digitalWrite(FOCUS_CAMERA, HIGH);  
  digitalWrite(SHUTTER_CAMERA, HIGH);
}


//////////////////////////////
void setup() {
  //for print statement
  Serial.begin(9600);
  EEPROM.begin(512);

  //for jp8900-16pin
  SoftwareSerial.begin(9600);
  //pinMode(BUSY_PIN,INPUT_PULLUP);

  //camera shutter and focus for capturing pictures 
  pinMode(FOCUS_CAMERA, OUTPUT);
  pinMode(SHUTTER_CAMERA, OUTPUT);
  digitalWrite(FOCUS_CAMERA, HIGH);  
  digitalWrite(SHUTTER_CAMERA, HIGH);
  delay(150);
 

  // **** Buzzer ****
  //pinMode(BUZZER, OUTPUT);

  // **** Camera Controls ****
  //.....

  //// ***** Joystick and Buttons *****
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(A_BUTTON, INPUT_PULLUP);
  pinMode(B_BUTTON, INPUT_PULLUP);

  // ***** Motor *****
  // motor using accelstepper lib
  rear_motor.setMaxSpeed(RPM);
  front_motor.setMaxSpeed(RPM);

  steppers.addStepper(rear_motor);
  steppers.addStepper(front_motor);

  // ***** Display *****
  tft.init(135, 240);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);//clear screen
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);

  // ***** EEPROM Read *****
  // reads the stored memory
  // if empty (==255), setting default values to 0
  focus_range = EEPROM.read(0);
  zoom_range = EEPROM.read(1);
  focus_current = EEPROM.read(2);
  zoom_current = EEPROM.read(3);
  orientation = EEPROM.read(4);
  shutter_time = EEPROM.read(5);
  motor_time = EEPROM.read(6);
  exposure_option_set = EEPROM.read(7);
  // focus_current = 0;
  // zoom_current = 0;

}

void loop() {
  switch (home_screen) {
    case 0: {
      //configuration menu
      switch (configuration_screen) {
        //camera_setting
        case 0: {
          // Zoom-Focus position
          switch (camera_setting_screen) {
            // Zoom-Focus position screen
            case 0: {
              switch (camera_positioning_screen) {
                // zoom at the Front
                case 0:{
                  Serial.println("zoom at the Front");
                  orientation = 0;
                  EEPROM.write(4,orientation);
                  EEPROM.commit();
                  camera_positioning_screen = -1;
                  //delay(100);
                  break;
                }
                // zoom at the Back
                case 1:{
                  Serial.println("zoom at the Back");
                  orientation = 1;
                  EEPROM.write(4,orientation);
                  EEPROM.commit();
                  camera_positioning_screen = -1;
                  //delay(100);
                  break;                
                }
                // go back
                case 2:{
                  camera_setting_screen = -1;
                  camera_positioning_screen = resetScreen(camera_positioning_screen);//set conguration_screen -1
                  break;
                }
                // show [positioning settings menu]
                default: 
                  max_option = menu(3,pm_name ,positioning_menu, option_selected);
                  camera_positioning_screen = getUpdate(camera_positioning_screen);
                  break;
              }
              break;
            }
            // Set Shutter Time screen - shutter time of the DSLR camera
            case 1:{
                //display shutter speed bar (motor calibration).
                option_selected = 0; //set selected option on shutter menu
                int max_shutter_time = 40;
                //hotbar(shutter_menu,cur_shutter_time,max_shutter_time,option_selected,has_back)
                hotbar(shutter_menu, shutter_time, max_shutter_time, option_selected, true);
                do {
                //hotbar(shutter_menu,cur_shutter_time,max_shutter_time,option_selected,has_back,header,footer,color,updatebar)
                  hotbar(shutter_menu, shutter_time, max_shutter_time, option_selected, true, 0, 3, GOLDENROD, true);
                  option_selected = getUpDown(2, option_selected, 0);
                  if (!option_selected) {
                    // !0 - true in C, only update when option_selected is at 'option-0'
                    shutter_time = getLeftRight(max_shutter_time, shutter_time,0, 0);
                  }
                } while(!(digitalRead(A_BUTTON) == LOW && option_selected));
                EEPROM.write(5, shutter_time);
                EEPROM.commit();
                camera_setting_screen = resetScreen(camera_setting_screen); //set camera_setting_screen -1
                break;
            } 
            // set motor movement time -  time needed to execute a sequence
            case 2: {
              option_selected = 0; //set selected option on shutter menu
              int motor_time_max = 40;
               hotbar(motor_time_menu, motor_time, motor_time_max, option_selected, true);
              do {
                hotbar(motor_time_menu, motor_time, motor_time_max, option_selected, true, 0, 3, GOLDENROD, true);
                option_selected = getUpDown(2, option_selected, 0);
                if (!option_selected) {
                  // !0 - true in C, only update when option_selected is at 'option-0'
                  motor_time = getLeftRight(motor_time_max, motor_time, 0, 0);
                }
              } while(!(digitalRead(A_BUTTON) == LOW && option_selected));
              EEPROM.write(6, motor_time);
              EEPROM.commit();
              camera_setting_screen = resetScreen(camera_setting_screen); //set camera_setting_screen -1
              break;
            }
            //go back
            case 3: {  
              configuration_screen = -1;
              camera_setting_screen = resetScreen(camera_setting_screen);//set conguration_screen -1
            break;
            }
            //show [camera settings menu]
            default:
              //max_option = menu(4,cs_name ,camera_settings_menu, option_selected);
              max_option = menu_with_header(4,cs_name ,camera_settings_menu, option_selected,1);
              camera_setting_screen = getUpdate(camera_setting_screen);
              break;
          }
          break;
        }
        //calibration,(MOTOR CALLIBRATION)
        case 1: {
          switch (motor_calibration_screen1) {
            //zoom calibration
            case 0: {         
              //go to zoom calibrate bar
              setCurrentPos(ZOOM, zoom_current * MS_STEP);
              setAccel(ZOOM, CALI_ACCEL);

              // set to maximum right, set motor speed 0 as calibration use default speed not motor speed
              moveMotor(ZOOM, zoom_range, 0);
              zoom_current = zoom_range;
              zoom_current = calibrate(ZOOM, calizoom_right, MOTOR_STEPS, 0);
              int maxZoom = zoom_current;
              updateScreen(100);

              // set to minimum left
              moveMotor(ZOOM, 0, 0); // returns back to 0
              zoom_current = 0;
              zoom_current = calibrate(ZOOM, calizoom_left, maxZoom, maxZoom-MOTOR_STEPS);
              zoom_range = maxZoom - zoom_current;
              updateScreen(100);
              EEPROM.write(1, zoom_range);
              
              zoom_current = 0; // minimum becomes absolute min pos
              setCurrentPos(ZOOM, zoom_current);
              EEPROM.write(3, zoom_current);
              motor_calibration_screen1 = resetScreen(motor_calibration_screen1);
              EEPROM.commit();
              break;
            } 
            //focus calibration
            case 1: {
              //go to focus calibrate bar
              
              setCurrentPos(FOCUS, focus_current * MS_STEP);
              setAccel(FOCUS, CALI_ACCEL);

              // set to maximum right, set motor speed 0 as calibration use default speed not motor speed
              moveMotor(FOCUS, focus_range, 0);
              focus_current = focus_range;
              focus_current = calibrate(FOCUS, califocus_right, MOTOR_STEPS, 0);
              int maxFocus = focus_current;
              updateScreen(100);

              moveMotor(FOCUS, 0, 0);
              focus_current = 0;
              focus_current = calibrate(FOCUS, califocus_left, maxFocus, maxFocus-MOTOR_STEPS);
              focus_range = maxFocus - focus_current;
              updateScreen(100);
              EEPROM.write(0, focus_range);
              
              focus_current = 0; // minimum becomes absolute min pos
              setCurrentPos(ZOOM, focus_current);
              EEPROM.write(2, focus_current);
              motor_calibration_screen1 = resetScreen(motor_calibration_screen1);
              EEPROM.commit();
              break;
            }
            //next page
            case 2: {
              switch (motor_calibration_screen2) {
      
                //edit exposure page
                case 0:{
                  switch(exposure_option_screen){
                    //pre
                    case 0:{
                      exposure_option_set = 0;
                      exposure_option_screen = -1;
                      EEPROM.write(7, exposure_option_set);
                      EEPROM.commit();
                      break;
                    }
                    //split
                    case 1:{
                      exposure_option_set = 1;
                      exposure_option_screen = -1;
                      EEPROM.write(7, exposure_option_set);
                      EEPROM.commit();
                      break;
                    }
                    //after
                    case 2:{
                      exposure_option_set = 2;
                      exposure_option_screen = -1;
                      EEPROM.write(7, exposure_option_set);
                      EEPROM.commit();
                      break;
                    }
                    //go back
                    case 3:{
                    motor_calibration_screen2 = -1;
                    exposure_option_screen = resetScreen(exposure_option_screen);
                    break;
                    }

                    default:
                    max_option = menu_with_header(4,exposure_option_name,exposure_option_menu, option_selected,3);  
                    exposure_option_screen = getUpdate(exposure_option_screen);//screen set to base on mainmenu option_selected
                    break;
                  }
                  break;
                }
                
                //reset all calibration
                case 1: {
                  EEPROM.write(0,0);
                  EEPROM.write(1,0);
                  EEPROM.write(2,0);
                  EEPROM.write(3,0);
                  EEPROM.write(4,0);
                  EEPROM.write(5,0);
                  EEPROM.write(6,0);
                  focus_range = 0;
                  zoom_range = 0;
                  focus_current = 0;
                  zoom_current = 0;
                  orientation = 0;
                  shutter_time = 0;
                  motor_time = 0;
                  EEPROM.commit();
                  motor_calibration_screen2 = resetScreen(motor_calibration_screen2);
                  Serial.println("reset all calibration");
                  break;
                }

                //go back
                case 2: {
                  motor_calibration_screen1 = -1;
                  motor_calibration_screen2 = resetScreen(motor_calibration_screen2);
                  break;
                }

                default:
                  max_option = menu_with_header(3,mc2_name,motor_calibration_menu2, option_selected,2);  
                  motor_calibration_screen2 = getUpdate(motor_calibration_screen2);//screen set to base on mainmenu option_selected
                  break;
              }
              break;
            }
            //go back
            case 3: {  
              configuration_screen = -1;
              motor_calibration_screen1 = resetScreen(motor_calibration_screen1);
              break;
            }
            //show motor callibration menu
            default:  
              max_option = menu_with_header(4,mc1_name,motor_calibration_menu1, option_selected,2);  
              motor_calibration_screen1 = getUpdate(motor_calibration_screen1);//screen set to base on mainmenu option_selected
              break;
          }
          break;
        }
        //POV calibration
        case 2: {
          //go to zoom POV bar
          //Ideally shd be 0 & minimum becomes absolute min pos
          //zoom_current = 0;
          //focus_current = 0;
        
          setAccel(ZOOM, CALI_ACCEL);
          setAccel(FOCUS, CALI_ACCEL);
      
          zoom_current = chooseDist(ZOOM, 3, zoom_adjust, false, AQUA);
          EEPROM.write(3, zoom_current);
          updateScreen(100);
          focus_current = chooseDist(FOCUS, 3, focus_adjust, false, DEEPPINK);
          EEPROM.write(2, focus_current);
          updateScreen(100);
          //Serial.println("pov calibration");
          configuration_screen = resetScreen(configuration_screen);
          EEPROM.commit();
          break;
        }  
        //go back
        case 3:{
          home_screen = -1;
          configuration_screen = resetScreen(configuration_screen);//set conguration_screen -1
          break;
        }
        //Show configuation_menu
        default:
          max_option = menu(4,mm_configuration_header,main_menu_1, option_selected);  
          configuration_screen = getUpdate(configuration_screen);//screen set to base on mainmenu option_selected
          break;
      }
      break;
    }
    //action menu-1
    case 1:  {
        //main_menu_2
        switch (action_screen_1) {          
          //zoom movement menu
          case 0: {
            switch(zoom_movements_menu1){  
              //zoom to max 
              case 0: {
                Serial.println("Zoom to max");
                countdownMenu();
                //global motor time pass in by default
                //return to starting position by default
                goDist(ZOOM, zm1_0, zoom_range, SNOW);
                zoom_movements_menu1 = resetScreen(zoom_movements_menu1);
                break;
              }
              // zoom to min
              case 1: {
                Serial.println("Zoom to min");
                countdownMenu();
                //global motor time pass in by default
                //return to starting position by default
                goDist(ZOOM, zm1_1, 0, SNOW);
                zoom_movements_menu1 = resetScreen(zoom_movements_menu1);
                break;
              }
              // zoom to value
              case 2: {
                Serial.println("Zoom to a value");
                int pos_desired;
                //choose dist and reset back to starting pos
                pos_desired = chooseDist(ZOOM, 3, zoom_dist, true, YELLOWGREEN);
                updateScreen(100);
                countdownMenu();
                //return to starting position by default
                goDist(ZOOM, zm1_2, pos_desired, YELLOWGREEN);
                zoom_movements_menu1 = resetScreen(zoom_movements_menu1);
                break;
              }
              //zoom_movements_menu2
              case 3:{
                switch(zoom_movements_menu2){             
                  //zoom max and back
                  case 0: {
                    Serial.println("Zoom to max and back");                    
                    // Serial.print("previous_pos");
                    // Serial.println(previous_pos);
                    int previous_pos = zoom_current;
                    countdownMenu();
                    //going back is now part of motor_time
                    goDist(ZOOM, zm2_0, zoom_range, SNOW, motor_time/2,false,false);
                    goDist(ZOOM, zm2_0, previous_pos, SNOW, motor_time/2,false,true);
                    zoom_movements_menu2 = resetScreen(zoom_movements_menu2);
                    break;
                  }
                  //zoom min and back
                  case 1: {
                    Serial.println("Zoom to min and back");                    
                    // Serial.print("previous_pos");
                    // Serial.println(previous_pos);
                    int previous_pos = zoom_current;
                    countdownMenu();
                    //going back is now part of motor_time
                    goDist(ZOOM, zm2_1, 0, SNOW, motor_time/2,false,false);
                    goDist(ZOOM, zm2_1, previous_pos, SNOW, motor_time/2,false,true);
                    zoom_movements_menu2 = resetScreen(zoom_movements_menu2);
                    break;
                  }              
                  //zoom to value and back
                  case 2: {
                    Serial.println("Zoom to a value and back");
                    // Serial.print("previous_pos");
                    // Serial.println(previous_pos);
                    int pos_desired;
                    int previous_pos = zoom_current;

                    //choose dist and reset back to starting pos
                    pos_desired = chooseDist(ZOOM, 3, zoom_dist, true, YELLOWGREEN);
                    updateScreen(100);
                    countdownMenu();

                    //going back is now part of motor_time
                    goDist(ZOOM, zm2_2, pos_desired, SNOW, motor_time/2,false,false);
                    goDist(ZOOM, zm2_2, previous_pos, SNOW, motor_time/2,false,true);
                    zoom_movements_menu2 = resetScreen(zoom_movements_menu2);
                    break;
                  } 
                  //back
                  case 3: {
                    zoom_movements_menu1 = -1;
                    zoom_movements_menu2 = resetScreen(zoom_movements_menu2);
                    break;
                    break;
                  }
                  //show zoom_movements_menu2
                  default:
                    max_option = menu(4,zm2_name ,zoom_menu2, option_selected);
                    zoom_movements_menu2 = getUpdate(zoom_movements_menu2);
                    break;

                }
                break;
              }
              //go back
              case 4: {  
                action_screen_1 = -1;
                zoom_movements_menu1 = resetScreen(zoom_movements_menu1);
                break;
              }
              //show zoom_movements_menu
              default:
                max_option = menu(5,zm1_name ,zoom_menu1, option_selected);
                zoom_movements_menu1 = getUpdate(zoom_movements_menu1);
                break;
            }
            break;
          }
          //focus movement menue
          case 1: {
            switch (focus_movements_menu1) {
              // focus to max
              case 0: {
                Serial.println("Focus to max");
                countdownMenu();
                goDist(FOCUS, fm1_0, focus_range, SNOW);
                focus_movements_menu1 = resetScreen(focus_movements_menu1);
                break;
              }
              // focus to min
              case 1: {
                Serial.println("Focus to min");
                countdownMenu();
                goDist(FOCUS, fm1_1, 0, SNOW);
                focus_movements_menu1 = resetScreen(focus_movements_menu1);
                break;
              }
              // focus to dist
              case 2: {
                Serial.println("Focus to a value");
                int pos_desired;
                pos_desired = chooseDist(FOCUS, 3, focus_dist, true, YELLOWGREEN);
                updateScreen();
                countdownMenu();
                goDist(FOCUS, fm1_2, pos_desired, YELLOWGREEN);
                focus_movements_menu1 = resetScreen(focus_movements_menu1);
                break;
              }
              //focus_movements_menu2
              case 3:{
                switch(focus_movements_menu2){          
                  //focus max and back
                  case 0: {
                    Serial.println("Focus to max and back");                    
                    // Serial.print("previous_pos");
                    // Serial.println(previous_pos);
                    int previous_pos = focus_current;
                    countdownMenu();
                    //going back is now part of motor_time
                    goDist(FOCUS, fm2_0, focus_range, SNOW, motor_time/2,false,false);
                    goDist(FOCUS, fm2_0, previous_pos, SNOW, motor_time/2,false,true);
                    focus_movements_menu2 = resetScreen(focus_movements_menu2);
                    break;
                  }

                  //Focus min and back
                  case 1: {
                    Serial.println("Focus to min and back");                    
                    // Serial.print("previous_pos");
                    // Serial.println(previous_pos);
                    int previous_pos = focus_current;
                    countdownMenu();
                    //going back is now part of motor_time
                    goDist(FOCUS, fm2_1, 0, SNOW, motor_time/2,false,false);
                    goDist(FOCUS, fm2_1, previous_pos, SNOW, motor_time/2,false,true);
                    focus_movements_menu2 = resetScreen(focus_movements_menu2);
                    break;
                  }
                  
                  //Focus to value and back
                  case 2: {
                    Serial.println("Focus to a value and back");
                    int pos_desired;
                    int previous_pos = focus_current;
                    pos_desired = chooseDist(FOCUS, 3, focus_dist, true, YELLOWGREEN);
                    updateScreen();
                    countdownMenu();
                    //going back is now part of motor_time
                    goDist(FOCUS, fm2_1, pos_desired, SNOW, motor_time/2,false,false);
                    goDist(FOCUS, fm2_1, previous_pos, SNOW, motor_time/2,false,true);
                    focus_movements_menu2 = resetScreen(focus_movements_menu2);
                    break;
                  } 
                  //back
                  case 3: {
                    focus_movements_menu1 = -1;
                    focus_movements_menu2 = resetScreen(focus_movements_menu2);
                    break;
                  }
                  //show zoom_movements_menu2
                  default:
                    max_option = menu(4,fm2_name ,focus_menu2, option_selected);
                    focus_movements_menu2 = getUpdate(focus_movements_menu2);
                    break;
                }
                break;
              }
              //go back
              case 4: {
                action_screen_1 = -1;
                focus_movements_menu1 = resetScreen(focus_movements_menu1);
                break;
              }
              //show focus_movements_menu
              default:
                max_option = menu(5,fm1_name ,focus_menu1, option_selected);
                focus_movements_menu1 = getUpdate(focus_movements_menu1);
                break;
            }
            break;
          }
          //zoom-focus movement menu
          case 2: {
            switch (zoom_focus_movements_menu1) {
              //ZoomFocus to max
              case 0: {
                Serial.println("ZF both to max");
                countdownMenu();
                goMultiDist(zf1_0, zoom_range, focus_range, VIOLET);
                zoom_focus_movements_menu1 = resetScreen(zoom_focus_movements_menu1);
                break;
              }
              //ZoomFocus to min
              case 1: {
                Serial.println("ZF both to min");
                countdownMenu();
                goMultiDist(zf1_1, 0, 0, AZURE);
                zoom_focus_movements_menu1 = resetScreen(zoom_focus_movements_menu1);
                break;
              } 
              // ZoomFocus to certain dist
              case 2: {
                Serial.println("ZF to certain dist");
                int zoom_desired, focus_desired;
                zoom_desired = chooseDist(ZOOM, 3, zoom_dist, true, YELLOWGREEN);
                if (zoom_desired == zoom_current) {
                  zoom_desired = -1; // failsafe
                }
                updateScreen(100);
                focus_desired = chooseDist(FOCUS, 3, focus_dist, true, RED);
                if (focus_desired == focus_current) {
                  focus_desired = -1; // failsafe
                }
                updateScreen(100);
                countdownMenu();
                goMultiDist(zf1_2, zoom_desired, focus_desired, LIME);
                zoom_focus_movements_menu1 = resetScreen(zoom_focus_movements_menu1);
                break;          
              }
              //show zoom_focus_page2
              case 3: {
                switch (zoom_focus_movements_menu2) {
                  // zoom to max, focus to min
                  case 0: {
                    Serial.println("zoom to max, focus to min");
                    countdownMenu();
                    goMultiDist(zf2_0, zoom_range, 0, CORAL);
                    zoom_focus_movements_menu2 = resetScreen(zoom_focus_movements_menu2);
                    break;
                  } 
                  // zoom to min, focus to max
                  case 1: {
                    Serial.println("zoom to min, focus to max");
                    countdownMenu();
                    goMultiDist(zf2_1, 0, focus_range, CORAL);
                    zoom_focus_movements_menu2 = resetScreen(zoom_focus_movements_menu2);
                    break;
                  }
                  
                  //zoom_focus_movements_menu3
                  case 2: {
                    switch (zoom_focus_movements_menu3) {

                      //ZF max and back
                      case 0: { 
                        Serial.println("ZF max and back");
                        int previous_zoom_pos = zoom_current;
                        int previous_focus_pos = focus_current;
                        // Serial.print("previous_zoom_pos");
                        // Serial.println(previous_zoom_pos);
                        // Serial.print("previous_focus_pos");
                        // Serial.println(previous_focus_pos);
                        countdownMenu();
                        //going back is now part of motor_time
                        goMultiDist(zf3_0, zoom_range, focus_range, VIOLET, motor_time/2, false, false);
                        goMultiDist(zf3_0, previous_zoom_pos, previous_focus_pos, VIOLET, motor_time/2, false, true);
                        zoom_focus_movements_menu3 = resetScreen(zoom_focus_movements_menu3);
                        break;
                      }
                      //ZF min and back
                      case 1: {
                        Serial.println("ZF min and back");
                        int previous_zoom_pos = zoom_current;
                        int previous_focus_pos = focus_current;
                        // Serial.print("previous_zoom_pos");
                        // Serial.println(previous_zoom_pos);
                        // Serial.print("previous_focus_pos");
                        // Serial.println(previous_focus_pos);
                        countdownMenu();
                        //going back is now part of motor_time
                        goMultiDist(zf3_1, 0, 0, VIOLET, motor_time/2, false, false);
                        goMultiDist(zf3_1, previous_zoom_pos, previous_focus_pos, VIOLET, motor_time/2, false, true);
                        zoom_focus_movements_menu3 = resetScreen(zoom_focus_movements_menu3);
                        break;
                      }
                      //ZF value and back
                      case 2: {
                        Serial.println("ZF value and back");
                        int previous_zoom_pos = zoom_current;
                        int previous_focus_pos = focus_current;
                        // Serial.print("previous_zoom_pos");
                        // Serial.println(previous_zoom_pos);
                        // Serial.print("previous_focus_pos");
                        // Serial.println(previous_focus_pos);
                        int zoom_desired, focus_desired;
                        zoom_desired = chooseDist(ZOOM, 3, zoom_dist, true, YELLOWGREEN);
                        if (zoom_desired == zoom_current) {
                          zoom_desired = -1; // failsafe
                        }
                        updateScreen(100);
                        focus_desired = chooseDist(FOCUS, 3, focus_dist, true, RED);
                        if (focus_desired == focus_current) {
                          focus_desired = -1; // failsafe
                        }
                        updateScreen(100);
                        countdownMenu();
                        //going back is now part of motor_time
                        goMultiDist(zf3_2, zoom_desired, focus_desired, VIOLET, motor_time/2, false, false);
                        goMultiDist(zf3_2, previous_zoom_pos, previous_focus_pos, VIOLET, motor_time/2, false, true);
                        zoom_focus_movements_menu3 = resetScreen(zoom_focus_movements_menu3);
                        break;
                      }
                      //zoom_focus_movements_menu4
                      case 3: {
                        switch(zoom_focus_movements_menu4){
                          //Z[max] F[min] and back
                          case 0: {
                            Serial.println("Z[max] F[min] and back");
                            int previous_zoom_pos = zoom_current;
                            int previous_focus_pos = focus_current;
                            // Serial.print("previous_zoom_pos");
                            // Serial.println(previous_zoom_pos);
                            // Serial.print("previous_focus_pos");
                            // Serial.println(previous_focus_pos);
                            countdownMenu();
                            //going back is now part of motor_time
                            goMultiDist(zf4_0, zoom_range, 0, VIOLET, motor_time/2, false, false);
                            goMultiDist(zf4_0, previous_zoom_pos, previous_focus_pos, VIOLET, motor_time/2, false, true);
                            zoom_focus_movements_menu4 = resetScreen(zoom_focus_movements_menu4);
                            break;
                          }                  
                          //Z[min] F[max] and back
                          case 1: {
                            Serial.println("Z[min] F[max] and back");
                            int previous_zoom_pos = zoom_current;
                            int previous_focus_pos = focus_current;
                            // Serial.print("previous_zoom_pos");
                            // Serial.println(previous_zoom_pos);
                            // Serial.print("previous_focus_pos");
                            // Serial.println(previous_focus_pos);
                            countdownMenu();
                            //going back is now part of motor_time
                            goMultiDist(zf4_1, 0, focus_range, VIOLET, motor_time/2, false, false);
                            goMultiDist(zf4_1, previous_zoom_pos, previous_focus_pos, VIOLET, motor_time/2, false, true);
                            zoom_focus_movements_menu4 = resetScreen(zoom_focus_movements_menu4);
                            break;
                          }
                          //go back
                          case 2: {
                            zoom_focus_movements_menu3 = -1;
                            zoom_focus_movements_menu4 = resetScreen(zoom_focus_movements_menu4);
                            break;
                          }
                          //show zoom_focus_movements_menu4
                          default:
                            max_option = menu(3,zf3_name ,zoomfocus_menu4, option_selected);
                            zoom_focus_movements_menu4 = getUpdate(zoom_focus_movements_menu4);
                            break;                
                        }
                        break;
                      }                                          
                      //go back
                      case 4: {
                        zoom_focus_movements_menu2 = -1;
                        zoom_focus_movements_menu3 = resetScreen(zoom_focus_movements_menu3);
                        break;
                      }
                      //show zoom_focus_movements_menu3
                      default:
                        max_option = menu(5,zf3_name ,zoomfocus_menu3, option_selected);
                        zoom_focus_movements_menu3 = getUpdate(zoom_focus_movements_menu3);
                        break;
                    }
                    break;
                  }

                  //go back
                  case 3: { 
                    zoom_focus_movements_menu1 = -1;
                    zoom_focus_movements_menu2 = resetScreen(zoom_focus_movements_menu2);
                    break;
                }
                  //show zoom_focus_page2
                  default:
                    max_option = menu(4,zf2_name ,zoomfocus_menu2, option_selected);
                    zoom_focus_movements_menu2 = getUpdate(zoom_focus_movements_menu2);
                    break;
                }
                break;
              } 
              //go back
              case 4: {
                action_screen_1 = -1;
                zoom_focus_movements_menu1 = resetScreen(zoom_focus_movements_menu1);
                break;
              }
              //show zoom_focus_page1
              default:
                max_option = menu(5,zf1_name ,zoomfocus_menu1, option_selected);
                zoom_focus_movements_menu1 = getUpdate(zoom_focus_movements_menu1);
                break;
            }
            break;
          }
          // go back
          case 3: {   
            home_screen = -1;
            action_screen_1 = resetScreen(action_screen_1);//set conguration_screen -1
            break;
          }
          default:
            max_option = menu(4,mm_action1_header,main_menu_2, option_selected);  
            action_screen_1 = getUpdate(action_screen_1);
            break;
        }
        break;
    }
    //action menu-2
    case 2: {
      //main_menu_3
      switch (action_screen_2) {      
        //fixed pattern
        case 0: {
          switch (fixed_paterns_menu1) {
            // Bokeh Effect (S: Focus Max, Zoom Current. F: Focus Current, Zoom Widest)
            case 0: {
              Serial.println("Bokeh Effect");
              int previous_pos = focus_current;
              // setting lens to starting position
              printMoveSteps(-1, preset1_0, CADETBLUE, 2); 
              //moving motor, haven start pattern yet so use default motor speed
              moveMotor(FOCUS, focus_range, 0);
              focus_current = focus_range;
              //start pattern sequence
              updateScreen(1000);
              countdownMenu();
              goDist(FOCUS, preset1_0, previous_pos, VIOLET, motor_time/2,false,false);
              goDist(ZOOM, preset1_0, 0, VIOLET, motor_time/2,true,true);
              fixed_paterns_menu1 = resetScreen(fixed_paterns_menu1);
              break;
            }

            // Firework Effect (Focus Max, then min, then return to original)
            case 1: {  
              Serial.println("Firework Effect");
              int previous_pos = focus_current;
              // setting lens to starting position
              printMoveSteps(-1, preset1_1, CADETBLUE, 2);
              //moving motor, haven start pattern yet so use default motor speed 
              moveMotor(FOCUS, focus_range,0);
              focus_current = focus_range;
              updateScreen(1000);
              countdownMenu();
              goDist(FOCUS, preset1_1, 0, AZURE, ((float)3/4)*motor_time, false,false);
              goDist(FOCUS, preset1_1, previous_pos, AZURE, ((float)1/4)*motor_time,false,true);
              fixed_paterns_menu1 = resetScreen(fixed_paterns_menu1);
              break;
            }
            // Zoom Blur Effect (Focus & Zoom Max then back to original)
            // Is going back to original part of motor time or not?
            case 2: {
              Serial.println("ZoomBlur Effect");
              int previous_zoom_pos = zoom_current;
              int previous_focus_pos = focus_current;
              // setting lens to starting position
              printMoveSteps(-1, preset1_2, CADETBLUE, 2); // setting lens to starting position
              //asume going back part of motor time
              updateScreen(1000);
              countdownMenu();
              goMultiDist(preset1_2, zoom_range, focus_range, VIOLET, motor_time/2, false, false);
              goMultiDist(preset1_2, previous_zoom_pos, previous_focus_pos, VIOLET, motor_time/2, false, true);
              fixed_paterns_menu1 = resetScreen(fixed_paterns_menu1);
              break;
            }

            //next page
            case 3: {
              switch (fixed_paterns_menu2) {
                //Sine Wave Effect
                case 0: {
                  Serial.println("Sine Wave Effect");
                  int previous_zoom = zoom_current;
                  int previous_focus = focus_current;
                  Serial.print("zoom_current");
                  Serial.println(zoom_current);
                  Serial.print("zoom_current");
                  Serial.println(zoom_current);
                  countdownMenu();
                  goDist(ZOOM, preset2_0, zoom_range, CORAL, motor_time/4, false,false);
                  goDist(FOCUS, preset2_0, focus_range, CORAL, motor_time/4, false,false);
                  goDist(ZOOM, preset2_0, 0, CORAL, motor_time/4,false,false);
                  goDist(FOCUS, preset2_0, 0, CORAL, motor_time/4,false,true);
                  //end of pattern
                  // return to initial position
                  updateScreen(1000);
                  printMoveSteps(1, preset2_0, CADETBLUE, 1); 
                  Serial.print("previous_zoom");
                  Serial.println(previous_zoom);
                  Serial.print("previous_focus");
                  Serial.println(previous_focus);
                  moveMultiMotor(previous_zoom,previous_focus,0);
                  zoom_current = previous_zoom;
                  focus_current = previous_focus;
                  fixed_paterns_menu2 = resetScreen(fixed_paterns_menu2);
                  break;
                }
                //Zigzagger Out Effect
                case 1: {
                  fixed_paterns_menu2 = -1;
                  Serial.println("Zigzagger Out Effect");
                  break;
                }
                case 2: {
                  fixed_paterns_menu1 = -1;
                  fixed_paterns_menu2 =  resetScreen(fixed_paterns_menu2);
                  break;
                }
                default:
                  max_option = menu(3,preset2_name,preset2_menu, option_selected);  
                  fixed_paterns_menu2 = getUpdate(fixed_paterns_menu1);
                  break;
              }
              break;
            }

            //go back

            case 4: {
              action_screen_2 = -1;
              fixed_paterns_menu1 =  resetScreen(fixed_paterns_menu1);
              break;
            }

            default:
              max_option = menu(5,preset1_name,preset1_menu, option_selected);  
              fixed_paterns_menu1 = getUpdate(fixed_paterns_menu1);
              break;
          }

          break;
        }


        //custome pattern
        case 1: {
          action_screen_2 = resetScreen(action_screen_2);
          break;
        }

        //go back
        case 2: {
          home_screen = -1;
          action_screen_2 = resetScreen(action_screen_2);
          break;
        }
        
        default:
          max_option = menu(3,mm_action2_header,main_menu_3, option_selected);  
          action_screen_2 = getUpdate(action_screen_2);
          break;

        
        }
      break;
    }
    //Home page
    default:
        max_option = menu(3,home_header,home_menu, option_selected);  
        home_screen = getUpdate(home_screen);//screen set to base on mainmenu option_selected
        break;
  }
  option_selected = getUpDown(max_option, option_selected, 0);
  //goToHomeMenu();



  //Serial.println("option_selected");
  // Serial.println(option_selected);
  //Serial.println("==================");

}

