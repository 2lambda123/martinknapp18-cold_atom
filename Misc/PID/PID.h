#include "mbed.h"


#ifndef PID_H
#define PID_H

// Controller Gains
double Kp = 0.5;
double Ki = 0.5;
double Kd = 0;

// Derivative low-pass filter time constant
// double tau = 1;

// Output limits
double limMin = -2;
double limMax = 2;

// Integrator limits
double limMinInt = -5;
double limMaxInt = 5;

// Sample time (in seconds)
double T = 0.560;

// Controller "memory"
double integrator;
double prevError;           /* Required for integrator */
double prevIntegrator;           /* Required for integrator */
double differentiator;
double prevMeasurement;     /* Required for differentiator */

// Controller setpoint and output
double setpoint = 0;
double output;


// Functions
void PIDController_Init(void){

    // Clear controller variables
    integrator = 0;
    differentiator  = 0;

    prevMeasurement = 0;
    prevError  = 0;
    prevIntegrator  = 0;

    output = 0;

}


double PIDController_Update(double measurement){

    // Error signal
    double error = -1*(setpoint - measurement);     // changing the sign here flips the error signal


    // Proportional
    double proportional = error;


    // Integral
    // double cycle_integrator = T * (prevError + error);
    double cycle_integrator = T * (error);
    integrator = (Ki*cycle_integrator) + prevIntegrator;

    // Anti-wind-up via integrator clamping
    if (integrator > limMaxInt)
    {
        integrator = limMaxInt;
    }
    if (integrator < limMinInt) {
        integrator = limMinInt;
    }


    // Derivative
    differentiator = (error - prevError) / T;


    // Compute PID output, apply limits
    output = (Kp*proportional) + (integrator) + (Kd*differentiator);

    if (output > limMax)
    {
        output = limMax;
    }
    if (output < limMin)
    {
        output = limMin;
    }

    // Store results to memory for future feedback
    prevError       = error;
    prevMeasurement = measurement;
    prevIntegrator = integrator;

    // Return controller output
    return output;

}

#endif // _PID_H_
