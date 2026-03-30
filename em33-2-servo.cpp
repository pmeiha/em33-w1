/*
 * em33-2-servo.cpp
 *
 * Created By: Pius Meier
 * Created on: 20.02.2026
 *
 */

#include "em33-2.h"

#define EM33_2_SERVO_LOG true

Servo servoBrake;
Servo servoSpeed;

void servoSetup()
{
    servoBrake.attach(EM33_SERVO_BRAKE_PIN, 1000, 2000); // attach servo to pin with min and max pulse width
    servoSpeed.attach(EM33_SERVO_SPEED_PIN, 1000, 2000); // attach servo to pin with min and max pulse width
    LOG_I(EM33_2_SERVO_LOG, "Servo setup completed");
}

void servoMain(void *parameter)
{
    // This function can be used to control the servos based on the received data from the mesh network or web socket
    // For example, you can set the servo positions based on the speed and brake values received in the em33Msg_t structure
    int lastbrakep = 1500; // Initialize last brake pulse width to center position

    servoSetup();

    for (;;)
    {

        if (em33MsgLok.newData)
        {
            // Set the break servo positions
            if (em33MsgLok.brakep > 1800)
            {
                servoBrake.writeMicroseconds(2000); // Full brake
                lastbrakep = 2000;
            }
            else if (em33MsgLok.brakep < 1200)
            {
                servoBrake.writeMicroseconds(1000); // brake release
                lastbrakep = 1000;
            }
            else if (em33MsgLok.brakep < 1700 && em33MsgLok.brakep > 1300)
            {
                servoBrake.writeMicroseconds(1500); // brake stop
                lastbrakep = 1500;
            }

            servoSpeed.writeMicroseconds(em33MsgLok.speedp); // Assuming speedp is the desired pulse width for the speed servo
            // servoBrake.writeMicroseconds(em33MsgLok.brakep); // Assuming brakep is the desired pulse width for the brake servo

            LOG_II(EM33_2_SERVO_LOG,
                   "Empfangene BremsPulsebreite: ", String(String(em33MsgLok.brakep) + " gesetzte Bremspulsbreite " + String(lastbrakep) + " Geschwindikeit Pulsbreite " + String(em33MsgLok.speedp)).c_str());

            // LOG_II(EM33_2_SERVO_LOG, "Updated servo positions - Speed Pulse Width: ", String(em33MsgLok.speedp).c_str());
            // LOG_II(EM33_2_SERVO_LOG, "Updated servo positions - Brake Pulse Width: ", String(em33MsgLok.brakep).c_str());

            em33MsgLok.newData = false; // Reset the newData flag after processing
        }

        tDelay(100); // Add a small delay to avoid overwhelming the loop
    }
}
