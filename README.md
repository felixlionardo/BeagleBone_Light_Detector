# Light Dip Detector

## Overview

The Light Dip Detector is a project for the CMPT 433 course, instructed by Brian Fraser. The assignment involves creating a program named `light_sampler` that runs on a BeagleBone and monitors light levels using a light sensor. The project includes several functionalities such as displaying the number of light dips on a 14-segment display, controlling an LED using PWM based on a potentiometer, and responding to UDP commands.

## Features

- **Light Sensor Monitoring**: Continuously samples light levels using an analog to digital converter (A2D).
- **14-Segment Display**: Displays the number of light dips detected in the previous second.
- **PWM Control**: Uses the potentiometer to control the blink frequency of an LED.
- **UDP Communication**: Listens for commands and responds with relevant data.
- **Multithreading**: Utilizes pthreads for concurrent execution of tasks.
- **Exponential Smoothing**: Computes the average light level with exponential smoothing.

## Project Details

- **Sampling Light Levels**: Runs in a separate thread, storing samples and computing statistics.
- **Light Dips Detection**: Analyzes light level samples to detect dips and displays the count.
- **UDP Commands**: Responds to commands such as `count`, `length`, `dips`, `history`, and `stop`.
- **Terminal Output**: Prints sample data and statistics to the terminal each second.
- **Hardware Interaction**: Interfaces with the light sensor, potentiometer, LED, and 14-segment display.