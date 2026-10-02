void goAngle(float angle, float seeAngle, int speed)
{ // угол на который надо поехать по гироскопу, скорость
    float err = 0, speedRotate = 0;
    err = lead_to_degree_borders(curAngle - seeAngle);
    speedRotate = PD(err);
    if (flagOff == true)
    {
        motor1.setSpeed(0);
        motor2.setSpeed(0);
        motor3.setSpeed(0);
        motor4.setSpeed(0);
    }
    else
    {
        motor1.setSpeed(speed * cos((-45 + angle) / 180 * 3.14) - speedRotate);
        motor2.setSpeed(-speed * cos((-135 + angle) / 180 * 3.14) - speedRotate);
        motor3.setSpeed(speed * cos((135 + angle) / 180 * 3.14) - speedRotate);
        motor4.setSpeed(speed * cos((45 + angle) / 180 * 3.14) + speedRotate);
    }
}

void kick()
{
    digitalWrite(pinsolin, HIGH);
    kickDel = true;
    timer_kick2 = millis();
    // Serial.println("111");//4tc - 2
}

void kick_Del()
{
    if (kickDel && (millis() - timer_kick2 >= 200))
    {
        timer_kick2 = 0;
        digitalWrite(pinsolin, LOW);

        kickDel = false;
        Serial.println("222"); // 4tc - 2
    }
}
void dribler(int speeds)
{
    dribblerESC.writeMicroseconds(speeds);
}
