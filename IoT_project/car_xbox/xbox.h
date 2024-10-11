#include <XboxSeriesXControllerESP32_asukiaaa.hpp>

XboxSeriesXControllerESP32_asukiaaa::Core xboxController; // any xbox controller

void xbox_setup(){
  Serial.println("Starting NimBLE Client");
  xboxController.begin();
}

boolean xbox_connect(){
    xboxController.onLoop();
    boolean is_XBOX_connect;
    if (xboxController.isConnected()) {
        is_XBOX_connect = false;
        if (xboxController.isWaitingForFirstNotification()) {
        Serial.println("waiting for first notification");
        }
        else{
        is_XBOX_connect = true;
        }
    } 
    else {
        is_XBOX_connect = false;
        Serial.println("not connected");
        if (xboxController.getCountFailedConnection() > 2) {
        ESP.restart();
        }
    }
    return is_XBOX_connect;
}

void xbox_control(boolean is_User_mode){
    if(xboxController.xboxNotif.btnY){
        is_User_mode = !is_User_mode;
        Serial.println(is_User_mode);
      }
      if(is_user_mode){
        forward_button = ((xboxController.xboxNotif.trigRT-xboxController.xboxNotif.trigLT)*255)/1023;
        float mappedValue = mapFloat(xboxController.xboxNotif.joyLHori, 0, 65535, -1, 1);
        forward(forward_button, mappedValue);
      }else{
        d = dis_cm();
        Serial.print(d,1);
        Serial.println("cm");
        if(d>15){
          forward(255, 0);
        }
        else{
          forward(255, 0, true);
        }
      }
}

