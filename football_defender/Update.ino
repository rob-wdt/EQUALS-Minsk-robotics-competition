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
  //    Serial.print(angle_gate);
  //    Serial.print("  ");
  Serial.print(abs_backward_angle);
  Serial.print("  ");
  Serial.print(angle_gate_ball);
  //Serial.print("  ");
  //    Serial.print(spdX);
  Serial.print("  ||  ");
  Serial.print(spdY);
  Serial.print("  ");
  Serial.print(spdX);
  Serial.print("  ||  ");
  Serial.print(spdGLK);
  Serial.print("  ");
  Serial.print(alphaGLK);
  Serial.print("  ||  ");
  Serial.print(x);
  Serial.print("  ");
  Serial.println(y);
}
