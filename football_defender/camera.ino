void Camera() {
  if (Serial3.available() > 6)
  {
    //Serial.println("BEGIN");
    byte First_bayt = Serial3.read();
    //Serial.println(First_bayt);
    if (First_bayt == 255) {
      //Serial.println("FIRST BYTE RECIEVED");
      // switchT_C = Serial3.read();
      for (int i = 0; i < 7; i++) {
        //Serial.println(Serial1.peek());
        data_cam[i] = Serial3.read();
        //Serial.println(data_cam[i]);

      }
      byte crc = crc8(data_cam, 6);
      //Serial.println(data_cam[6]);
      //Serial.println(crc);
      if (crc == data_cam[6])
      {
        //Serial.println("q ");
        yel_angle = data_cam[0] * 3;
        yel_dist = data_cam[1] * 3;
        blue_angle = data_cam[2] * 3;
        blue_dist = data_cam[3] * 3;
        ball_cam_dist = data_cam[4] * 3;
        ball_cam_angle = data_cam[5] * 3;

#if OWN_GOAL == GOAL_YELLOW
        own_goal_angle = yel_angle;
        own_goal_distance = yel_distance;
#elif OWN_GOAL == GOAL_BLUE
        own_goal_angle = blue_angle;
        own_goal_distance = blue_dist;
#endif
        
        //        Serial.print("  ball_cam_dist  ");
        //        Serial.print(ball_cam_dist);//180-
        //        Serial.print("  blue_angle\t  ");
        //        Serial.print(blue_angle);
        //        Serial.println("  ");
      }
    }
  }
}
uint8_t crc8(uint8_t* data, int len)
{
  uint8_t crc = 0xFF, i, j;
  for (i = 0; i < len; i++) {
    crc ^= data[i];
    for (j = 0; j < 8; j++) {
      if (crc & 0x80) crc = (char)((crc << 1) ^ 0x31);
      else crc <<= 1;
    }
  }

  return crc;
}

float normalize_angle(float angle) // from the camera we recieve the angle between 0 and 360. if the ball is leftside (the angle should be negative), it is between 180 and 360. so, this function converts the angle from the camera to the real angle.
{
    if (angle > 0 && angle <= 180)
    {
        return angle;
    }
    else if (angle >= 180 && angle < 360)
    {
        return angle - 360;
    }
}
