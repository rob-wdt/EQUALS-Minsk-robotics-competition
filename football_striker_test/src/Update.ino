void updates() {
  //digitalWrite(LED_BUILTIN, millis() % 200 > 100);
  curAngle = lead_to_degree_borders(angleGyro - errAngleGyro);
  data_tcops();
  Camera();
  gyro();
  readSensors();
  coordinates();
  kick_Del();
  KickPosition();
  //GoBall_short();
  outs();
  

//  Serial.print(lead_to_degree_borders(ball_cam_angle));
//  Serial.print("  ||  ");
//  Serial.print(ball_cam_dist);
//  Serial.print("  ||  ");
//  Serial.print(exponential_detour(lead_to_degree_borders(ball_cam_angle), ball_cam_dist, 0.45, 0.15, 0.35, 0.7));
//  Serial.print("  ||  ");
//  Serial.print(ball_cam_dist);
//  Serial.println(" ");

}
