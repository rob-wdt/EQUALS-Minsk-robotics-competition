
//void switch_ts_or_cam() {
//  if(ball_cam_dist  == 0 && ball_ts_dist == 0){
//    ball_dist = 0;
//  }
//  else if (switchT_C == 1){
//    ball_angle = ball_ts_angle;
//    ball_dist = ball_ts_dist;
//  }
//  else if (switchT_C == 2){
//    ball_angle = ball_cam_angle;
//    ball_dist = ball_cam_dist;
//  }
//}
double convert_dist(double max_dist, double dist)
{
  _data = (max_dist-dist) / max_dist + 1;
  
  if(_data > 1)
    _data = 1;
  else if(_data < 0)
    _data = 0;
  
  return _data;
}
//относительно дист и угла на мяч выбирает более эф путь
int16_t exponential_detour(double ball_angle, double distance, double k1_angle, double k2_angle, double k1_dist, double k2_dist)
{
  angK = k1_angle * pow(ec, double(k2_angle * abs(ball_angle)));
  if(angK > 90)
    angK = 90;
  
  distance = convert_dist(5, distance);

  distK = k1_dist * pow(ec, double(k2_dist * distance));

  if(distK > 1)
    distK = 1;
  
  if(ball_angle > 0)
    _data = angK * distK;
  else
    _data = -angK * distK;
  
  return _data;//возращает угол на который надо ехать
}
//void GoBall_short(){
//  if(ball_retention > 5 && ball_cam_dist <= 10 && abs(lead_to_degree_borders(ball_cam_angle)) <= 5){
//    flagShortBall = true;
//    Priority_Angle = ball_cam_angle;
//  }
//  else{
//    flagShortBall = false;
//    Priority_Angle = forward_angle;
//  }
//}
