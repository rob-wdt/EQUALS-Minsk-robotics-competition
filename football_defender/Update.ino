void updates() {
  digitalWrite(LED_BUILTIN, millis() % 200 > 100);
  corAng = lead_to_degree_borders(angleGyro - errAngleGyro);
  data_tcops();
  Camera();
  gyro();
  readSensors();

  coordinates();
  kick_Del();
  outs();
  tmrSTRK();
  
  Serial.print(backward_dist);
  Serial.print(' ');
  Serial.print(backward_dist <= backward_out);
  Serial.print("  ");
  Serial.print(forward_dist);
  Serial.print(' ');
  Serial.print(forward_dist <= forward_out);
  Serial.print("\t");
  Serial.print(x);
  Serial.print(' ');
  Serial.print(x >= right_out);
  Serial.print(' ');
  Serial.print(x <= left_out);
  Serial.print(' ');
  Serial.println(y);
}
