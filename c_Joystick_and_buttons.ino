// ******** JOYSTICK Functions **********
/*
 * checks for up and down joystick movement
 */
int getUpDown(int max_option, int current_option, int delay_ms) {
  if (digitalRead(A_BUTTON) == LOW) {
    return current_option;
  }
  if (digitalRead(DOWN_BUTTON) == LOW) {
    delay(delay_ms);
    updateMenu = true;
    return (current_option == max_option-1) ? 0 : ++current_option;
  }

  if (digitalRead(UP_BUTTON) == LOW) {
    delay(delay_ms);
    updateMenu = true;
    return (current_option == 0) ? max_option-1 : --current_option;
  }
  return current_option;
}

/*
 * checks for left and right joystick
 * able to set to lower limit if required
 */
int getLeftRight(int range, int current, int low_limit, int delay_ms) {
  if (digitalRead(A_BUTTON) == LOW){
    return current;
  } 
  if (digitalRead(RIGHT_BUTTON) == LOW) {
    delay(delay_ms);
    updateMenu = true;
    return (current == range) ? current : ++current; 
  }
  if (digitalRead(LEFT_BUTTON) == LOW) {
    delay(delay_ms);
    updateMenu = true;
    return (current == low_limit) ? low_limit : --current;
  }
  
  return current;
}

/*
 * Checks if A_BUTTON is pressed
 */ 
int getUpdate(int s) {
  if (digitalRead(A_BUTTON) == LOW) {
    //update 
    s = option_selected;
    option_selected = 0; //resets options
    tft.fillScreen(ST77XX_BLACK);//clear screen
    updateMenu = true;
  }
  return s;
}

/*
 * Checks if B_BUTTON is pressed
 */

 void goToHomeMenu(){
    if (digitalRead(B_BUTTON) == LOW) {
      Serial.println("Go to home B_BUTTON PRESSED");
      // Global Variables
      max_option = 0;
      updateMenu = true;
      option_selected = 0;
      home_screen = -1;
      configuration_screen = -1;
      camera_setting_screen = -1;
      camera_positioning_screen = -1;
      motor_calibration_screen1 = -1;
      pov_calibrartion_screen = -1;
      action_screen_1 = -1;
      action_screen_2 = -1;
      focus_movements_menu1 = -1;
      zoom_movements_menu1 = -1;
      zoom_focus_movements_menu1 = -1;
      zoom_focus_movements_menu2 = -1;
      fixed_paterns_menu1 = -1;
      fixed_paterns_menu2 = -1;
      tft.fillScreen(ST77XX_BLACK);//clear screen

    }
    else{
      //Serial.println("B_BUTTON NOT PRESSED");
    }
 }