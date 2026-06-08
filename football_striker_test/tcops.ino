int16_t lead_to_degree_borders(int angle)
{
  if (angle > 180) angle -= 360;
  else if (angle < -180) angle += 360;
  return angle;

}
void data_tcops() {
  for (int i = 0; i < 16; i++)
  {
    digitalWrite(ADDR_P1, (i & 0b0001));
    digitalWrite(ADDR_P2, (i & 0b0010) >> 1);
    digitalWrite(ADDR_P3, (i & 0b0100) >> 2);
    digitalWrite(ADDR_P4, (i & 0b1000) >> 3);

    ball_data[ir_addr3[i]] = 1 - digitalRead(BALL_SEN_SIGNAL_1);
    ball_data[ir_addr3[i+16]] = 1 - digitalRead(BALL_SEN_SIGNAL_2);
    
    //delayMicroseconds(10);
    if (ball_data[11] == 1 && ball_data[13] == 1){
      ball_data[12] = 1;
    }
    if (ball_data[7] == 1 && ball_data[9] == 1){
      ball_data[8] = 1;
    }
  }

  double x = 0;
  double y = 0;
  for (int i = 0; i < 32; i++)
  {
    x += ball_data[i] * sin((d_alpha * i) / 57.3);
    y += ball_data[i] * cos((d_alpha * i) / 57.3);
  }
  ball_ts_angle = lead_to_degree_borders(atan2(x, y) * -57.3);
  ball_ts_dist = sqrt(x * x + y * y);
}
void readSensors(){
//  Left_dist = analogRead(DATCHIK_DIST_LEFT);
//  Right_dist = analogRead(DATCHIK_DIST_RIGHT);
//  Forward_dist = analogRead(DATCHIK_DIST_FORWARD);
//  Back_dist = analogRead(DATCHIK_DIST_BACK);
  ball_retention = analogRead(FOTOTRANZ);
  
}
