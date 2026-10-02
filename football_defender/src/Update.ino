void updates()
{
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
}
