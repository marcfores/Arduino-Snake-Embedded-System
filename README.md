# Arduino Embedded Snake: Hardware Implementation

A complete, interactive hardware implementation of the classic "Snake" video game. 

Developed as a final project for the Embedded Systems course, this system integrates analog input handling, digital matrix control, real-time event processing, and structured C++ programming.

## System Architecture & Hardware

The project relies on a robust combination of microcontrollers and peripherals:

*   **Microcontroller:** Arduino Mega 2560, responsible for game logic, joystick input reading, and LED matrix control.
*   **Visual Output:** Four 8x8 LED matrices (MAX7219) cascaded to create a unified 32x8 display. The MAX7219 chip facilitates efficient control via SPI communication.
*   **User Interface:** An analog joystick for real-time directional control (X and Y axes), featuring an integrated push-button for pause, resume, and restart functions.

## Software Logic & Features

The C++ (`.ino`) firmware was developed using the Arduino IDE and features:

*   **Dynamic Gameplay:** The snake grows upon consuming randomly generated food, with game speed automatically increasing as the snake lengthens.
*   **Collision Detection:** Real-time spatial tracking to prevent the snake from crossing the screen boundaries or colliding with its own body.
*   **Non-Blocking Logic:** Implementation of debounce techniques for accurate button readings and time-based progression loops.
*   **Game States:** Full state machine managing gameplay, pause modes, Game Over visual animations, and automatic resets.

## Repository Structure
*   `/src`: Contains the `snake_game.ino` source code and necessary dependencies.
*   `/docs`: Includes the detailed project report (Memoria), circuit schematics, and pseudocode algorithms.

## Libraries Used
*   `LedControl.h`: For SPI communication and matrix cascading.
