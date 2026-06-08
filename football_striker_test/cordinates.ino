void coordinates()
{
    int sideGame = 1;
    if (sideGame == 1)
    {
        forward_dist = blue_dist;   // blue_dist
        backward_dist = yel_dist;   // yel_dist
        forward_angle = blue_angle; // blue_angle
        backward_angle = yel_angle; // yel_angle

        right_out2G = 67;
        left_out2G = -75;
        right_out1G = 48;
        left_out1G = -65;

        forward_out = 45;
        backward_out = 40;

        abs_forward_out = 160;
        abs_backward_out = 34;
    }
    else
    {
        right_out2G = 65;
        left_out2G = -60;
        right_out1G = 60;
        left_out1G = -40;
        forward_out = 40;
        abs_forward_out = 177;
        backward_out = 45;
        abs_backward_out = 50;
        forward_dist = yel_dist;     // blue_dist
        backward_dist = blue_dist;   // yel_dist
        forward_angle = yel_angle;   // blue_angle
        backward_angle = blue_angle; // yel_angle
    }

    if (forward_dist == 0) forward_angle = 0;

    abs_ball_angle = lead_to_degree_borders(ball_cam_angle + curAngle);
    abs_forward_angle = lead_to_degree_borders(forward_angle + curAngle);
    abs_backward_angle = lead_to_degree_borders(backward_angle + curAngle);

    x_forward = -forward_dist * sin(abs_forward_angle * DEG2RAD);
    x_backward = -backward_dist * sin(abs_backward_angle * DEG2RAD);
    y_forward = 230 - forward_dist * cos(abs_forward_angle * DEG2RAD);
    y_backward = -backward_dist * cos(abs_backward_angle * DEG2RAD);

    if ((forward_dist == 0 || backward_dist < 100) && backward_dist != 0)
    {
        Correct_coef = (backward_dist) / (2 * 220);
        x = (x_backward * (1 - Correct_coef));
        y = (y_backward * (1 - Correct_coef));

        flagOneGate = true;
        // Serial.println("backward");
    }
    else if ((backward_dist == 0 || forward_dist < 100) && forward_dist != 0)
    {
        Correct_coef = (forward_dist) / (2 * 220);
        x = (x_forward * (1 - Correct_coef));
        y = (y_forward * (1 - Correct_coef));

        flagOneGate = true;
        // Serial.println("forward");
    }
    else
    {
        if (forward_dist + backward_dist != 0)
        {
            Correct_coef = (backward_dist - forward_dist) / (2 * (forward_dist + backward_dist));
        }
        else
        {
            Correct_coef = 0;
        }
        
        x = (x_forward * Correct_coef) + (x_backward * (1 - Correct_coef));
        y = (y_forward * Correct_coef) + (y_backward * (1 - Correct_coef));
        flagOneGate = false;
        // Serial.println("2 goals");
    }
    if (flagOneGate == false)
    {
        right_out = right_out2G;
        left_out = left_out2G;
        // Serial.println(" 2gate ");
    }
    else
    {
        right_out = right_out1G;
        left_out = left_out1G;
        // Serial.println(" 1gate ");
    }
}

void KickPosition()
{
    if (abs(x) >= abs_backward_out && y >= abs_forward_out && ball_retention <= 3)
    {
        flagKickPosition = false;
        flagBadZone = true;
    }
    else if (forward_dist > 65 || abs(abs_forward_angle) > 70)
    {
        flagKickPosition = false;
        flagBadZone = false;
    }
    else
    {
        flagKickPosition = true;
        flagBadZone = false;
    }
}
void outs()
{
    if ((right_out <= x) || (left_out >= x) || (backward_out >= backward_dist && backward_dist != 0) || (forward_out >= forward_dist && forward_dist != 0))
    { //
        flagOut = true;

        if (left_out >= x)
        {
            out_angle = 90;
        }
        else if (right_out <= x)
        {
            out_angle = 270;
        }
        else if (backward_out >= backward_dist && backward_dist != 0)
            out_angle = 0; // Serial.println("back");
        else if (forward_out >= forward_dist && forward_dist != 0)
            out_angle = 180; // Serial.println("for");
    }
    else
        flagOut = false;
}
