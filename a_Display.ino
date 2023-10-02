// ******** DISPLAY Functions **********
/* ----- MENU ------
 * Creates a menu using a string array 
 * Display which option is being selected
 * 
 * [Top of screen] -header {Will be dependent on the menu}
 * Shutter Speed: __
 * Rear Motor: ____ Front Motor: ____
 * 
 * [Middle of screen] 
 * |----< Menu Name >----| -meun_name
 * | Option 1 -string_tablep[]
 * | Option 2
 * | ...
 * 
 * [Bottom of screen] -footer
 * < > Move the lens          {Will be dependent on the menu}
 * ^v  Select the options
 *  o  Select current option
 */

int menu(int array_size,const char *menu_name ,const char *const string_table[], int option_selected, int footer, uint16_t color) {
  int total_num = array_size;
  if (!updateMenu) {
    //updateMenu false
    return total_num; 
  }
  updateMenu = false;
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.println(menu_name);
  tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(2);
  int rect_y = 12;

  for (int i=0; i<total_num; i++) {
    tft.drawRect(0,rect_y,tft.width(),23,WHITE);
    tft.setCursor(2,rect_y+4);
    if (i == option_selected) {
      tft.setTextColor(BLACK,WHITE);
    } else {
      tft.setTextColor(WHITE,BLACK);
    }
    tft.print(string_table[i]);
    rect_y = rect_y+23;
  }
  tft.setTextColor(WHITE,BLACK);
  return total_num; 
}

int menu_with_header(int array_size,const char *menu_name ,const char *const string_table[], int option_selected, int header, int footer, uint16_t color){
  int total_num = array_size;
  if (!updateMenu) {
    //updateMenu false
    return total_num; 
  }
  updateMenu = false;
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  int rect_y = 5;

  if (header != 0) {
    if (header > 0) {      
      tft.setTextColor(AQUA);
      tft.print(F("Shutter Time: "));
      tft.setTextColor(WHITE);
      tft.print(shutter_time);
      tft.setCursor(105,0);
      tft.setTextColor(AQUA);
      tft.print(F("Motor Time: "));
      tft.setTextColor(WHITE);
      tft.println(motor_time);

    }
    switch (header) {
      case -1:
      case 1: {
        tft.setTextColor(AQUA);
        tft.print(F("Front Motor: "));
        tft.setTextColor(WHITE,BLACK);
        tft.print(orientation ? "Zoom " : "Focus");
        tft.print("    ");
        tft.setTextColor(AQUA);
        tft.print(F("Rear Motor: "));
        tft.setTextColor(WHITE,BLACK);
        tft.print(orientation ? "Focus" : "Zoom ");
        break;
      }

      case 2: {
        tft.setTextColor(AQUA);
        tft.print(F("Zoom Range: "));
        tft.setTextColor(WHITE,BLACK);
        tft.print(zoom_range);
        tft.print("    ");
        tft.setTextColor(AQUA);
        tft.print(F("Focus Range: "));
        tft.setTextColor(WHITE,BLACK);
        tft.print(focus_range);
        break;
      }
      
      //exposure option 
      case 3: {  
        tft.setTextColor(AQUA);
        tft.print(F("Exposure Option: "));
        tft.setTextColor(WHITE,BLACK);
        switch (exposure_option_set) {
          //pre
          case 0:{
            tft.print("Pre");
            break;
          }
          //split
          case 1:{
            tft.print("Split");
            break;
          }
          //after
          case 2:{
            tft.print("After");
            break;
          }
        }
        // tft.print("    ");
        // tft.setTextColor(AQUA);
        // tft.print(F("Focus Range: "));
        // tft.setTextColor(WHITE,BLACK);
        // tft.print(focus_range);
        break;
      }
      case 4: { // focus
        // tft.setTextColor(AQUA);
        // tft.print(F("Focus Range: "));
        // tft.setTextColor(WHITE,BLACK);
        // tft.println(focus_range);
        // tft.setTextColor(AQUA);
        // tft.print(F("Focus Position: "));
        // tft.setTextColor(WHITE,BLACK);
        // tft.println(focus_current);
        break;
      }
      case 5: { //zoom
        // tft.setTextColor(AQUA);
        // tft.print(F("Zoom Range: "));
        // tft.setTextColor(WHITE,BLACK);
        // tft.println(zoom_range);
        // tft.setTextColor(AQUA);
        // tft.print(F("Zoom Position: "));
        // tft.setTextColor(WHITE,BLACK);
        // tft.println(zoom_current);
        break;
      }
      default:{ 
        break;
      }
    }
    tft.println();
    tft.println(); 
  }

  rect_y = 35;
  tft.setTextColor(color);
  tft.println(menu_name);
  tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(2);
  

  for (int i=0; i<total_num; i++) {
    tft.drawRect(0,rect_y,tft.width(),23,WHITE);
    tft.setCursor(2,rect_y+4);
    if (i == option_selected) {
      tft.setTextColor(BLACK,WHITE);
    } else {
      tft.setTextColor(WHITE,BLACK);
    }
    tft.print(string_table[i]);
    rect_y = rect_y+23;
  }
  tft.setTextColor(WHITE,BLACK);
  return total_num; 
}

/* --- Hotbar Screen ---
 *  Creates a template for a bar type screen
 *  Calculates the necessary parameters:
 *  char title - The title of the screen
 *  int current - Current value
 *  int max_range - The maximum range of the bar
 *  bool header - Header for the hotbar [false] 
 *  int footer - Type of footer to display [1]
 */

void hotbar(const char title[], int current, int max_range, int current_option, bool haveBack, int header, int footer, uint16_t color, bool updateBar) {
  if (!updateMenu) {
    return;
  }
  updateMenu = false;
  
  int divs = (tft.width()-30)/(float)max_range * abs(current);
  tft.setCursor(0, 0);
  int rect_y = 37;

  // ignore for now
  // if (header != 0) {
  //   if (!updateBar) {
  //     tft.setTextColor(AQUA);
  //     tft.print(F("Shutter Speed: "));
  //     tft.setTextColor(WHITE);
  //     tft.print(shutter_speed);
  //     switch (header) {
  //       case 1: {
  //         tft.setTextColor(AQUA);
  //         tft.print(F("Front Motor: "));
  //         tft.setTextColor(WHITE);
  //         tft.println(orientation ? "Zoom" : "Focus");
  //         tft.setTextColor(AQUA);
  //         tft.print(F("Rear Motor: "));
  //         tft.setTextColor(WHITE);
  //         tft.println(orientation ? "Focus" : "Zoom");
  //       }
  //       case 2: {
  //         tft.setTextColor(AQUA);
  //         tft.print(F("Focus Range: "));
  //         tft.setTextColor(WHITE);
  //         tft.println(focus_range);
  //         tft.setTextColor(AQUA);
  //         tft.print(F("Zoom Range: "));
  //         tft.setTextColor(WHITE);
  //         tft.println(zoom_range);
  //       }
  //     }
  //     tft.println(); 
  //   }
  //   rect_y =75;
  // }

  option_selected ? tft.setTextColor(WHITE,BLACK) : tft.setTextColor(BLACK,WHITE);
  if (title != NULL) {
    tft.setTextSize(1);
    tft.print(title);
  }
  //
  tft.setTextSize(2);
  tft.println();
  tft.setTextColor(WHITE);
  tft.print(F("Max Range: "));
  tft.println(max_range);
  tft.setCursor(10, rect_y + 30);
  option_selected ? tft.setTextColor(BLACK,WHITE) : tft.setTextColor(WHITE,BLACK);
  if (haveBack) {
    //page has back button
    tft.println(F("[Go Back]"));
  }

  /* Draw Hotbar */
  tft.drawRect(0,rect_y,tft.width()-26,14,WHITE);
  tft.fillRect(2,rect_y+2,tft.width()-30,10,BLACK); // reset inner rectangle
  tft.fillRect(2,rect_y+2,divs,10,color);
  tft.setCursor(105,rect_y+3);
  tft.setTextColor(WHITE,BLACK);
  tft.print(current);
  tft.print(" ");

  if (!updateBar) {
    tft.setTextSize(1);
    switch(footer) {
      case 1:
        tft.drawChar(2,90,LEFT_ARROW,WHITE,BLACK,2);
        tft.drawChar(17,90,RIGHT_ARROW,WHITE,BLACK,2);
        tft.setCursor(32,94);
        tft.println(F(": Adjust values"));
        tft.drawChar(2,105,SELECT,WHITE,BLACK,2);
        tft.setCursor(17,109);
        tft.println(F(": Press to set"));
        break;
      case 2:
        // tft.drawChar(2,80,UP_ARROW,WHITE,BLACK,2);
        // tft.drawChar(17,80,DOWN_ARROW,WHITE,BLACK,2);
        // tft.setCursor(32,84);
        // tft.println(F(": Navigate"));
        // tft.drawChar(2,95,SELECT,WHITE,BLACK,2);
        // tft.setCursor(17,99);
        // tft.println(F(": Press to set"));
        break;
      case 3:
        //shutter speed footer, bottom of screen
        // tft.drawChar(2,85,LEFT_ARROW,WHITE,BLACK,2);
        // tft.drawChar(17,85,RIGHT_ARROW,WHITE,BLACK,2);
        // tft.setCursor(32,90);
        // tft.println(F(": Adjust values-"));
        // tft.drawChar(2,100,UP_ARROW,WHITE,BLACK,2);
        // tft.drawChar(17,100,DOWN_ARROW,WHITE,BLACK,2);
        // tft.setCursor(32,104);
        // tft.println(F(": Navigate"));
        // tft.drawChar(2,115,SELECT,WHITE,BLACK,2);
        // tft.setCursor(17,119);
        // tft.println(F(": Press to set"));
        break;
      default:
      break;
    }
  }
  return;
}

/* Reset Screen */
int resetScreen(int s) {
  tft.fillScreen(ST77XX_BLACK);//clear screen
  s = -1;
  updateMenu = true;
  option_selected = 0;
  return s;
}

void updateScreen(float delay_ms) {
  delay(delay_ms);
  tft.fillScreen(ST77XX_BLACK);//clear screen
  updateMenu = true;
}

/* --- Calibrate Screen ---
 String_table will determine if zoom/focus
 */
 void caliMenu(const char *const string_table[], int current_step, int max_steps, uint16_t color, bool updateBar) {
  if (!updateMenu) {
    return;
  }
  hotbar(NULL, current_step, max_steps, 0, false, false, 1, color, updateBar);
  tft.setTextSize(1);
  int i = 0;
  tft.setCursor(0,0);
  tft.setTextColor(color);
  tft.println(string_table[i++]);
  tft.setCursor(0, 59);
  tft.setTextColor(WHITE);
  tft.print(string_table[i++]);
  tft.setTextColor(DARKGREEN);
  tft.println(string_table[i++]);
  tft.setTextColor(WHITE);
  
  return;
}

/* --- Move Motor Screen ---
 * Similar to cali Menu 
 */
void moveMotorMenu(int count, const char *const string_table[], int current_step, int max_steps, uint16_t color, bool updateBar) {
  if (!updateMenu) return;
  int i=0;
  hotbar(NULL, current_step, max_steps, 0, false, 0, 1, color, updateBar);
  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.setTextColor(color);
  tft.println(string_table[i]);
  tft.setTextColor(WHITE);
  tft.setCursor(0, 59);
  for (i=1; i<count; i++) {
    tft.println(string_table[i]);
  }
  tft.setTextColor(WHITE);
}

/* A simple way to print a countdown menu
 */
void countdownMenu() {

  // if (shutter_speed == 0) {
  //   updateScreen();
  //   buzz();
  //   return;
  // }

  int i=0;
  tft.setTextSize(3);
  tft.setCursor(0,0);
  tft.println(countdown[i]);
  delay(500);
  for (i=1; i<4; i++) {
    tft.setTextSize(4);
    tft.setCursor(0,30);
    tft.setTextColor(RED,BLACK);
    tft.println(countdown[i]);
    delay(1000);
  }
  tft.setTextSize(3);
  //tft.setCursor(30,75);
  tft.println(countdown[i]);
  
  //delay(500);
  //nikonTime();
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  //tft.println("Exposing camera to\nenvironment for...");
  tft.print("Exposing camera \nfor: ");
  tft.setTextSize(2);
  tft.setTextColor(AQUA);
  //shutter_time should always be more then motor time
  //int i=(shutter_time-motor_time)
  // for (int i=(shutter_time); i>=0; i--) {
  //   tft.setTextColor(AQUA,BLACK);
  //   tft.setCursor(50,105);
  //   tft.print(i);
  //   tft.print("s  ");
  //   delay(1000);
  // }
  
  tft.setTextColor(AQUA,BLACK);
  tft.setCursor(50,105);
  tft.print(shutter_time);
  tft.print("s  ");
  delay(1500);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  updateScreen();
}

void printMoveSteps(int type, const char title[], uint16_t color, int goBack) {
  tft.setTextSize(1);
  // tft.setCursor(0,0);
  // tft.setTextColor(AQUA);
  // tft.print(F("Shutter Time: "));
  // tft.setTextColor(WHITE);
  // tft.println(shutter_time);
  // if (type != 0) {
  //   tft.setTextColor(AQUA);
  //   tft.print(type ? "Zoom" : "Focus");
  //   tft.print(F(" Range: "));
  //   tft.setTextColor(WHITE);
  //   tft.println(type ? zoom_range : focus_range);
  // }
  tft.setCursor(0,0);
  tft.setTextColor(AQUA);
  tft.print(F("Shutter Time: "));
  tft.setTextColor(WHITE);
  tft.println(shutter_time);
  tft.setTextColor(AQUA);
  tft.print(F("Motor Time: "));
  tft.setTextColor(WHITE);
  tft.println(motor_time);
  if(type == -1){
      //nothing
  }
  else if (type == 3) {
    tft.setTextColor(AQUA, BLACK);
    tft.print(F("Focus Range: "));
    tft.setTextColor(WHITE, BLACK);
    tft.println(focus_range);
    tft.setTextColor(AQUA, BLACK);
    tft.print(F("Zoom Range: "));
    tft.setTextColor(WHITE, BLACK);
    tft.println(zoom_range);
  } else {
    //type
    tft.setTextColor(AQUA, BLACK);
    tft.print(type ? "Zoom" : "Focus");
    tft.print(F(" Range: "));
    tft.setTextColor(WHITE, BLACK);
    tft.println(type ? zoom_range : focus_range);
  }
  tft.println();



  //char myChar;
  // tft.setTextColor(color);
  // for (byte k=0; k<strlen_P(title); k++) {
  //   //myChar = pgm_read_byte_near(title+k);
  //   tft.print(myChar);
  // }

  tft.print(title);
  tft.drawLine(0, 75, tft.width(), 75,WHITE);
  tft.setCursor(0, 85);
  tft.setTextColor(WHITE);
  switch(goBack) {
    case 0: {
      tft.println(F("Moving to "));
      tft.setTextColor(LIGHTSKYBLUE, BLACK);
      tft.print(F("DESIRED "));
      tft.setTextColor(WHITE, BLACK);
      tft.println(F("location"));
      break;
    }
    case 1: {
      tft.println(F("Returning to "));
      tft.setTextColor(RED, BLACK);
      tft.print(F("PREVIOUS "));
      tft.setTextColor(WHITE, BLACK);
      tft.println(F("POV location"));
      break;
    }
    case 2: {
      tft.println(F("Moving to "));
      tft.setTextColor(LIME, BLACK);
      tft.print(F("STARTING "));
      tft.setTextColor(WHITE, BLACK);
      tft.println(F("position"));
      break;
    }
    default: break;
  }
}