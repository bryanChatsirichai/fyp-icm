// ******** INTERFACE Functions **********
/*
 * Functions here integrates all the 3 modules before this
 * This allows creation of certain patterns or outcomes
 * much easier without a messy code in the main file
 */

 int calibrate(int type, const char *const string_table[], int upper_limit, int lower_limit, uint16_t color) {
  int pos_current;
  pos_current = type ? zoom_current : focus_current;
  caliMenu(string_table, pos_current, upper_limit, color);
  do {
    caliMenu(string_table, pos_current, upper_limit, color, true);
    pos_current = getLeftRight(upper_limit, pos_current, lower_limit, 0);
    Serial.print("Pos current: ");
    Serial.println(pos_current);
    //set motor speed 0 as calibration use default speed not motor speed
    moveMotor(type, pos_current,0);
  } while(digitalRead(A_BUTTON) != LOW);
  
  return pos_current;
}

int chooseDist(int type, int count, const char *const string_table[], bool goBack, uint16_t color) {
  int pos_current, upper_limit;
  pos_current = type ? zoom_current : focus_current;
  upper_limit = type ? zoom_range : focus_range;
  moveMotorMenu(count, string_table, pos_current, upper_limit, color);
  do {
    moveMotorMenu(count, string_table, pos_current, upper_limit, color, true);
    pos_current = getLeftRight(upper_limit, pos_current, 0, 0);
    //set motor speed 0 as calibration use default speed not motor speed
    moveMotor(type, pos_current,0);
  } while(digitalRead(A_BUTTON) != LOW);

  if (goBack) {
    // needs to go back to original spot
    Serial.println("needs to go back to original spot");
    delay(500);
    tft.setCursor(0, 59);
    tft.setTextColor(WHITE,BLACK);
    tft.println(F("Returning to         "));
    tft.setTextColor(RED,BLACK);
    tft.print(F("PREVIOUS "));
    tft.setTextColor(WHITE,BLACK);
    tft.println(F("location          "));
    for (int i=2; i<count; i++) {
      tft.println("                  ");
    }
    // returns to original spot
    //go back acceleration and speed could be faster
    moveMotor(type, type ? zoom_current : focus_current, 0);
  }
  
  return pos_current;
}

void goDist(int type, const char title[], int pos_desired, uint16_t color, float motor_time, bool goBack, bool lastSequence) {
  
  Serial.print("@goDist motor_time = ");
  Serial.println(motor_time);
  
  int pos_current, upper_limit;
  pos_current = type ? zoom_current : focus_current;
  upper_limit = type ? zoom_range : focus_range;

  //start sound
  //buzz();
  //play_start_sound();
  //play_notification_sound_v2();
  //delay(3000);//for sound to complete
  play_sound_1();
  printMoveSteps(type, title, color, 0); 
  //moveMotor(type, pos_desired, motor_time);

  //move motor depending on exposure_option_set
  switch (exposure_option_set){
    //pre
    case 0:{
      float remainder_time = shutter_time - motor_time;
      if(camera_shutter_open == 0){
        open_Shutter();
        camera_shutter_open = 1;
      }
      moveMotor(type, pos_desired, motor_time);
      delay(remainder_time);
      break;
    }
    //split
    case 1:{
      float remainder_time = shutter_time - motor_time;
      float front_remainder_time = remainder_time / 2;
      float back_remainder_time = remainder_time / 2;
      if(camera_shutter_open == 0){
        open_Shutter();
        camera_shutter_open = 1;
      }
      delay(front_remainder_time);
      moveMotor(type, pos_desired, motor_time);
      delay(back_remainder_time);
      break;
    }
    //after
    case 2 :{
      float remainder_time = shutter_time - motor_time;
      if(camera_shutter_open == 0){
        open_Shutter();
        camera_shutter_open = 1;
      }
      delay(remainder_time);
      moveMotor(type, pos_desired, motor_time);
      break;
    }
    default:{
      break;
    }
    
  }

  //zoom / focus current postion after moving to desired position
  if (type) { // ZOOM
    zoom_current = pos_desired;
  } else { // FOCUS
    focus_current = pos_desired;
  }
  
  if(lastSequence){
    //end sound  
    //buzz();
    play_sound_2();
    if(camera_shutter_open == 1){
        close_Shutter();
        camera_shutter_open = 0;
      }
    //close shutter(); //nikonTime(1000);
    updateScreen(100);
  }


  // returns to original spot
  //go back acceleration and speed could be faster
  if (goBack) {
    printMoveSteps(type, title, color, 1);
    moveMotor(type, pos_current, 0);
    //zoom / focus current postion after moving to desired position
    if (type) { // ZOOM
      zoom_current = pos_current;
    } else { // FOCUS
      focus_current = pos_current;
    }
  }
  updateScreen(100);
}

void goMultiDist(const char title[], int zoom_desired, int focus_desired, uint16_t color, float motor_time, bool goBack,bool lastSequence) {

  Serial.print("@goMultiDist motor_time = ");
  Serial.println(motor_time);

  int prev_zoom, prev_focus;
  prev_zoom = zoom_current;
  prev_focus = focus_current;

  //start sound
  //buzz();
  //play_start_sound();
  //play_notification_sound_v2();
  //delay(5000);
  play_sound_1();
  printMoveSteps(3, title, color, 0);
  //Serial.println("Both moving to position");
  //moveMultiMotor(zoom_desired, focus_desired, motor_time);

  //move motor depending on exposure_option_set
  switch (exposure_option_set){
    //pre
    case 0:{
      float remainder_time = shutter_time - motor_time;
      if(camera_shutter_open == 0){
        open_Shutter();
        camera_shutter_open = 1;
      }
      moveMultiMotor(zoom_desired, focus_desired, motor_time);
      delay(remainder_time);
      break;
    }
    //split
    case 1:{
      float remainder_time = shutter_time - motor_time;
      float front_remainder_time = remainder_time / 2;
      float back_remainder_time = remainder_time / 2;
      if(camera_shutter_open == 0){
        open_Shutter();
        camera_shutter_open = 1;
      }
      delay(front_remainder_time);
      moveMultiMotor(zoom_desired, focus_desired, motor_time);
      delay(back_remainder_time);
      break;
    }
    //after
    case 2 :{
      float remainder_time = shutter_time - motor_time;
      if(camera_shutter_open == 0){
        open_Shutter();
        camera_shutter_open = 1;
      }
      delay(remainder_time);
      moveMultiMotor(zoom_desired, focus_desired, motor_time);
      break;
    }
    default:{
      break;
    }
    
  }


  //zoom / focus current postion after moving to desired position
  zoom_current = zoom_desired;
  focus_current = focus_desired;

  if(lastSequence){
    //nikonTime(1000);
    //end sound  
    //buzz();
    //play_stop_sound();
    play_sound_2();
    if(camera_shutter_open == 1){
        close_Shutter();
        camera_shutter_open = 0;
      }
    //close shutter(); //nikonTime(1000);
    updateScreen(100);
  }



  // returns to original spot
  if (goBack) {
    Serial.println("Both going back to position");
    printMoveSteps(3, title, color, 1);
    //moveMultiMotor(zoom_current, focus_current, motor_time);
    //moveMultiMotor(prev_zoom, prev_focus,0);

    //see this got any differentce .....
    zoom_current = prev_zoom;
    focus_current = prev_focus;
  }
  updateScreen();
}
