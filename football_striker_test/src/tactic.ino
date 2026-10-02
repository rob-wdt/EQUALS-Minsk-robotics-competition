void tactic()
{
    if (flagOut == false)
    {
        if (flagBadZone == true)
        {
            goAngle(180, curAngle, 115);
        }
        if (y <= abs_backward_out && abs_ball_angle > 80)
            goAngle(0, 0, 107);
        else if (y >= abs_forward_out && abs_ball_angle > 30 && abs_ball_angle < 80)
            goAngle(180, 0, 107);

        if (ball_retention <= 3)
            goAngle(0, abs_forward_angle, 110);
        else if (abs(lead_to_degree_borders(ball_cam_angle)) <= 10)
        {
            goAngle(ball_cam_angle, abs_forward_angle, 110);
            // Serial.println(" for1 ");
        }
        else
        {
            if (ball_cam_dist > 35)
            {
                // Serial.println(" Dist ");
                goAngle(ball_cam_angle, abs_forward_angle, 110);
            }
            else
            {
                if (lead_to_degree_borders(ball_cam_angle) < 0)
                {                                                                                                                                                                                // Serial.println(" Left ");
                    goAngle(lead_to_degree_borders(ball_cam_angle + exponential_detour(lead_to_degree_borders(ball_cam_angle), ball_cam_dist, 0.62, 0.365, 0.45, 0.8)), abs_forward_angle, 107); // 0.61, 0.35, 0.45, 0.8
                }
                else
                { // Serial.println(" Right ");
                    goAngle(lead_to_degree_borders(ball_cam_angle - exponential_detour(lead_to_degree_borders(ball_cam_angle), ball_cam_dist, 0.63, 0.35, 0.45, 0.8)), abs_forward_angle, 107);
                }
            }
        }
        if (flagKick == true && flagKickPosition == true && ball_retention <= 3 && (millis() - timer_kick) >= 5000)
        { //
            kick();
            flagKick = false;
            timer_kick = millis();
            flagOne = false;
        }
        if (ball_cam_dist <= 14 && ball_retention <= 3)
        {
            dribler(1614); // 1617
            driblerON = true;
            flagKick = true;
        }
        else if (ball_cam_dist <= 18)
            dribler(1614);
        else
            dribler(0);
    }
    else if (flagOut == true)
    {
        goAngle(out_angle, 0, 110);
        flagOut = false;
    }
}
