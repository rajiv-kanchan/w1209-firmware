/*
 * This file is part of the W1209 firmware replacement project
 * (https://github.com/mister-grumbler/w1209-firmware).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Control functions for buttons.
 * The EXTI2 interrupt (5) is used to get signal on changing buttons state.
 */

#include "buttons.h"
#include "stm8s003/gpio.h"
#include "menu.h"

/* Definition for buttons */
// Port C control input from buttons.
#define BUTTONS_PORT   PC_IDR
// PC.3
#define BUTTON1_BIT    0x08
// PC.4
#define BUTTON2_BIT    0x10
// PC.5
#define BUTTON3_BIT    0x20


#define MAX_CHECKS 10                   // No. of checks before a button is debounced.
static unsigned char debouncedState;    // Debounced state of the buttons.
static unsigned char changedState;      // stores change in state of the buttons.
static unsigned char state[MAX_CHECKS]={0}; // Array that maintains bounce status.
static unsigned char index=0;             // pointer into state.

unsigned char rawButtonPress() {
    return ~ (BUTTONS_PORT & (BUTTON1_BIT | BUTTON2_BIT | BUTTON3_BIT) );
}


// debounce algorithm taken from http://www.ganssle.com/debouncing.htm
// debounces all 3 button presses and stores the debounced state in variable debouncedState.
// Also stores any button state change in variable changedState.
// MAX_CHECKS decides the no. of checks before a button is debounced.
void debounceButton() {
    unsigned char i,j;
    state[index] = rawButtonPress();
    ++index;
    j=0xff;
    for (i=0; i<MAX_CHECKS; i++) {
        j = j & state[i];
    }
    // update changed state
    changedState = debouncedState ^ j;
    debouncedState = j;
    if (index>=MAX_CHECKS) {
        index=0;
    }
}

// event returns corresponding event for button press.
// returns MENU_EVENT_NONE if no button is pressed.
unsigned char event() {
    if (isButton1() ) {
        if (getButton1() ) {
            return MENU_EVENT_PUSH_BUTTON1;
        } else {
            return MENU_EVENT_RELEASE_BUTTON1;
        }
    } else if (isButton2() ) {
        if (getButton2() ) {
            return MENU_EVENT_PUSH_BUTTON2;
        } else {
            return MENU_EVENT_RELEASE_BUTTON2;
        }
    } else if (isButton3() ) {
        if (getButton3() ) {
            return MENU_EVENT_PUSH_BUTTON3;
        } else {
            return MENU_EVENT_RELEASE_BUTTON3;
        }
    } else {
        return MENU_EVENT_NONE;
    }
}

/**
 * @brief Configure approptiate pins of MCU as digital inputs. Set
 *  initial value of status by getting current state of buttons.
 *  Configure external interrupt for these inputs.
 */
void initButtons()
{
    PC_CR1 |= BUTTON1_BIT | BUTTON2_BIT | BUTTON3_BIT;
}

/**
 * @brief Gets value of buttons status which was when last interrupt
 *  request being handled.
 * @return status byte of buttons
 */
unsigned char getButton()
{
    return debouncedState;
}

/**
 * @brief
 * @return
 */
unsigned char getButtonDiff()
{
    return changedState;
}

/**
 * @brief
 * @return
 */
bool getButton1()
{
    return debouncedState & BUTTON1_BIT;
}

/**
 * @brief
 * @return
 */
bool getButton2()
{
    return debouncedState & BUTTON2_BIT;
}

/**
 * @brief
 * @return
 */
bool getButton3()
{
    return debouncedState & BUTTON3_BIT;
}

/**
 * @brief isButton1 returns true if the button state has changed.
 * @return
 */
bool isButton1()
{
    if (changedState & BUTTON1_BIT) {
        changedState &= ~BUTTON1_BIT;
        return true;
    }

    return false;
}

/**
 * @brief isButton2 returns true if the button state has changed.
 * @return
 */
bool isButton2()
{
    if (changedState & BUTTON2_BIT) {
        changedState &= ~BUTTON2_BIT;
        return true;
    }

    return false;
}// runs 2 times a second

/**
 * @brief isButton3 returns true if the button state has changed.
 * @return
 */
bool isButton3()
{
    if (changedState & BUTTON3_BIT) {
        changedState &= ~BUTTON3_BIT;
        return true;
    }

    return false;
}