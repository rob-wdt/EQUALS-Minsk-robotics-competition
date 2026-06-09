//BNO
void gyro() {
  static uint32_t tmr;
  if (millis() - tmr >= 11) {
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    angleGyro = euler.x();
    tmr = millis();  // сброс таймера
  }
}
