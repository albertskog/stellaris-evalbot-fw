//*****************************************************************************
//
// project.c - Simple project to use as a starting point for more complex
//             projects.
//
// Copyright (c) 2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 10636 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include "inc/lm3s9b92.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "evalbot_drivers/io.h"
#include "evalbot_drivers/motor.h"
#include "evalbot_drivers/display96x16x1.h"
#include "evalbot_drivers/sensors.h"

#include "lib/millis.h"

#define OBSTACLE_REVERSE_TIME_MS 1000
#define OBSTACLE_TURN_TIME_MS 1000

typedef enum {
    STATE_STOP,
    STATE_FORWARD,
    STATE_OBSTACLE_LEFT,
    STATE_OBSTACLE_RIGHT,
    STATE_OBSTACLE_FRONT
} robot_state_t;

typedef struct {
    robot_state_t current;
    robot_state_t previous;
    tBoolean sensor_left;
    tBoolean sensor_right;
    tBoolean switch_1;
    tBoolean switch_2;
    unsigned long now;
    unsigned long entered;
} robot_data_t;

typedef union {
    unsigned short value;
    struct {
        unsigned char decimal;
        unsigned char integer;
    };
} motor_speed_t;


static void update_sensors(robot_data_t * state);
static void update_buttons(robot_data_t * state);
static void update_state(robot_data_t * state);
static void state_stop(robot_data_t * state);
static void state_forward(robot_data_t * state);
static void state_obstacle_left(robot_data_t * state);
static void state_obstacle_right(robot_data_t * state);
static void state_obstacle_front(robot_data_t * state);

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

int main(void)
{
    /* Set the clocking to run from the PLL at 50 MHz */
    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ );

    millis_init();
    LEDsInit();
    BumpSensorsInit();
    MotorsInit();
    PushButtonsInit();

    robot_data_t state = {0};
    state.current = STATE_FORWARD;

    while(1)
    {
        state.now = millis();
        update_buttons(&state);
        update_sensors(&state);
        update_state(&state);
    }
}


static void update_sensors(robot_data_t * state) {
    state->sensor_right = !BumpSensorGetStatus(BUMP_RIGHT);
    state->sensor_left = !BumpSensorGetStatus(BUMP_LEFT);

    if(state->sensor_right){
        LED_On(LED_1);
    }
    else {
        LED_Off(LED_1);
    }

    if(state->sensor_left){
        LED_On(LED_2);
    }
    else {
        LED_Off(LED_2);
    }
}

static void update_buttons(robot_data_t * state) {
    static unsigned long last_debounce = 0;

    if(state->now - last_debounce > 10){
        PushButtonDebouncer();
        last_debounce = state->now;
    }

    state->switch_1 = PushButtonGetDebounced(BUTTON_1);
    state->switch_2 = PushButtonGetDebounced(BUTTON_2);
}

static void update_state(robot_data_t * state) {
    switch (state->current) {
        case STATE_STOP:
            state_stop(state);
        break;
        case STATE_FORWARD:
            state_forward(state);
        break;
        case STATE_OBSTACLE_LEFT:
            state_obstacle_left(state);
        break;
        case STATE_OBSTACLE_RIGHT:
            state_obstacle_right(state);
        break;
        case STATE_OBSTACLE_FRONT:
            state_obstacle_front(state);
        break;
    }
}

static void state_stop(robot_data_t * state) {
    state->previous = state->current;

    if (state->switch_2) {
        state->current = FORWARD;
    }
}

static void state_forward(robot_data_t * state) {
    if (state->current != state->previous) {
        motor_speed_t speed = {.integer = 50};
        MotorSpeed(RIGHT_SIDE, speed.value);
        MotorSpeed(LEFT_SIDE, speed.value);
        MotorDir(RIGHT_SIDE, FORWARD);
        MotorDir(LEFT_SIDE, FORWARD);
        MotorRun(RIGHT_SIDE);
        MotorRun(LEFT_SIDE);
        state->previous = state->current;
        state->entered = state->now;
    }

    if (state->sensor_right && state->sensor_left) {
        state->current = STATE_OBSTACLE_FRONT;
    }
    else if (state->sensor_right)
    {
        state->current = STATE_OBSTACLE_RIGHT;
    }
    else if (state->sensor_left)
    {
        state->current = STATE_OBSTACLE_LEFT;
    }
}

static void state_obstacle_left(robot_data_t * state) {
    if (state->current != state->previous) {
        motor_speed_t speed = {.integer = 50};
        MotorSpeed(RIGHT_SIDE, speed.value);
        MotorSpeed(LEFT_SIDE, speed.value);
        MotorDir(RIGHT_SIDE, REVERSE);
        MotorDir(LEFT_SIDE, REVERSE);
        MotorRun(RIGHT_SIDE);
        MotorRun(LEFT_SIDE);
        state->previous = state->current;
        state->entered = state->now;
    }

    if (state->now - state->entered > OBSTACLE_REVERSE_TIME_MS) {
        motor_speed_t speed = {.integer = 50};
        MotorSpeed(RIGHT_SIDE, speed.value);
        MotorSpeed(LEFT_SIDE, speed.value);
        MotorDir(RIGHT_SIDE, REVERSE);
        MotorDir(LEFT_SIDE, FORWARD);
        MotorRun(RIGHT_SIDE);
        MotorRun(LEFT_SIDE);
    }

    if (state->now - state->entered > OBSTACLE_REVERSE_TIME_MS + OBSTACLE_TURN_TIME_MS) {
        state->current = STATE_FORWARD;
    }
}

static void state_obstacle_right(robot_data_t * state) {
    if (state->current != state->previous) {
        motor_speed_t speed = {.integer = 50};
        MotorSpeed(RIGHT_SIDE, speed.value);
        MotorSpeed(LEFT_SIDE, speed.value);
        MotorDir(RIGHT_SIDE, REVERSE);
        MotorDir(LEFT_SIDE, REVERSE);
        MotorRun(RIGHT_SIDE);
        MotorRun(LEFT_SIDE);
        state->previous = state->current;
        state->entered = state->now;
    }

    if (state->now - state->entered > OBSTACLE_REVERSE_TIME_MS) {
        motor_speed_t speed = {.integer = 50};
        MotorSpeed(RIGHT_SIDE, speed.value);
        MotorSpeed(LEFT_SIDE, speed.value);
        MotorDir(RIGHT_SIDE, FORWARD);
        MotorDir(LEFT_SIDE, REVERSE);
        MotorRun(RIGHT_SIDE);
        MotorRun(LEFT_SIDE);
    }

    if (state->now - state->entered > OBSTACLE_REVERSE_TIME_MS + OBSTACLE_TURN_TIME_MS) {
        state->current = STATE_FORWARD;
    }
}

static void state_obstacle_front(robot_data_t * state) {
    if (state->current != state->previous) {
        motor_speed_t speed = {.integer = 50};
        MotorSpeed(RIGHT_SIDE, speed.value);
        MotorSpeed(LEFT_SIDE, speed.value);
        MotorDir(RIGHT_SIDE, REVERSE);
        MotorDir(LEFT_SIDE, REVERSE);
        MotorRun(RIGHT_SIDE);
        MotorRun(LEFT_SIDE);
        state->previous = state->current;
        state->entered = state->now;
    }

    if (state->now - state->entered > OBSTACLE_REVERSE_TIME_MS) {
        motor_speed_t speed = {.integer = 50};
        MotorSpeed(RIGHT_SIDE, speed.value);
        MotorSpeed(LEFT_SIDE, speed.value);
        MotorDir(RIGHT_SIDE, REVERSE);
        MotorDir(LEFT_SIDE, FORWARD);
        MotorRun(RIGHT_SIDE);
        MotorRun(LEFT_SIDE);
    }

    if (state->now - state->entered > OBSTACLE_REVERSE_TIME_MS + 2 * OBSTACLE_TURN_TIME_MS) {
        state->current = STATE_FORWARD;
    }
}
