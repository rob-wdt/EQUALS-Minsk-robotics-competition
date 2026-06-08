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
