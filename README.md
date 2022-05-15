# SelfBalancingRobot
Building a 2-wheeled self-balancing robot

# Notes
- Using an MSP430F5529 Launch Pad for control
- Using an MPU-6050 IMU for sensor inputs, to balance the robot

# Demo
https://user-images.githubusercontent.com/23331810/168490925-76549844-7b79-48c8-bae0-ef3949d84729.mp4

# Thoughts
- Have had trouble smoothing out control. Suspect the cheap motors are at fault, but the control loop is also somewhat convoluted due to a lack of foresight when implementing the fixed-point 16-bit work arounds. Next iteration will likely use better motors and an RTOS to allow a cleaner implementation that can also handle logging & remote-control
