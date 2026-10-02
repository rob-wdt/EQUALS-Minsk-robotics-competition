double PDcube(float err)
{
  double u = 0;                        // Переменная управляющего воздействия
  u = KP * err + KD * (err - err_old); // Вычисляем управл.возд.
  err_old = err;
  // Serial.println(u);
  return u;
}

double PC(double error, double kp, double kc)
{
  double u{kp * error + kc * pow(error, 3)};
  return u;
}
