// Initialize Motor Interface
void Motor_Init(void);

// Update Motor Speed
void Motor_SetDutyCycle(uint8_t duty);

// Specify Motor Direction (1 = forward, -1 = backward, 0 = off)
void Motor_Direction(int8_t dir);
