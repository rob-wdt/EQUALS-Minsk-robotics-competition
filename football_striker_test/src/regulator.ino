double PD(float err){
  double u = 0;        //Переменная управляющего воздействия

  u = KP * err + KD * (err - err_old); //Вычисляем управл.возд.
  err_old = err;
  //Serial.println(u);
  return u;
}
