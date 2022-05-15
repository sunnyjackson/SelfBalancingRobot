# SelfBalancingRobot
Building a 2-wheeled self-balancing robot

# Notes
- Using an MSP430F5529 Launch Pad for control
- Using an MPU-6050 IMU for sensor inputs, to balance the robot

# Demo
![SelfBalancingRobotDemo](https://user-images.githubusercontent.com/23331810/168491316-6a78296b-9da6-4d3f-8eeb-de83e6512e96.gif)

# Thoughts
- Have had trouble smoothing out control. Suspect the cheap motors are at fault, but the control loop is also somewhat convoluted due to a lack of foresight when implementing the fixed-point 16-bit work arounds. Would love for next iteration to use better motors, a microcontroller with floating point support, and an RTOS to make maintaining the system easier
