# SelfBalancingRobot
Building a 2-wheeled self-balancing robot

# Notes
- Using an MSP430F5529 Launch Pad for control
- Using an MPU-6050 IMU for sensor inputs, to balance the robot


https://user-images.githubusercontent.com/23331810/168491202-3b0eaddd-8d95-4853-9843-647400195993.mp4


# Demo


# Thoughts
- Have had trouble smoothing out control. Suspect the cheap motors are at fault, but the control loop is also somewhat convoluted due to a lack of foresight when implementing the fixed-point 16-bit work arounds. Next iteration will likely use better motors and an RTOS to allow a cleaner implementation that can also handle logging & remote-control
