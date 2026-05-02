# GPIO State Machine with Button and LED (BeagleBone Black)

## Overview

This project implements an event-driven GPIO application on the BeagleBone Black using the `libgpiod` interface.
The system demonstrates a structured state machine approach combined with timer-based event handling.

The application supports:

* Short press detection (toggle LED ON/OFF)
* Long press detection (enter/exit blink mode)
* Periodic LED blinking using a timer
* Clean hardware abstraction via GPIO wrapper functions
* Event-driven architecture using `poll()` and file descriptors

     

## Features

* Edge-triggered button input (rising and falling edges)
* Time-based press duration measurement
* State machine design (OFF, ON, BLINK)
* Non-blocking event handling using `poll()`
* Periodic timer using `timerfd`
* Modular GPIO abstraction layer

     

## System Behavior

### Short Press

* If LED is OFF → turns ON
* If LED is ON → turns OFF
* If in BLINK mode → exits BLINK and turns OFF

### Long Press

* If not blinking → enters BLINK mode
* If already blinking → exits BLINK mode

### Blink Mode

* LED toggles every 1 second using timer events

     

## Hardware Requirements

* BeagleBone Black
* LED
* 220Ω–1kΩ resistor
* Push button
* 10kΩ resistor (pull-down)
* Breadboard and jumper wires

     

## Hardware Connections

### LED

* GPIO: `gpiochip1 line 21` → P9_25
* Connection:

  * P9_25 → resistor → LED anode (+)
  * LED cathode (–) → GND (P9_1 or P9_2)

### Button

* GPIO: `gpiochip1 line 7` → P9_42
* Connection:

  * P9_42 → one side of button
  * Other side of button → 3.3V (P9_3 or P9_4)
  * P9_42 → 10kΩ resistor → GND

     

## GPIO Mapping

| GPIO Chip | Line | GPIO Name | Header Pin |
| --------- | ---- | --------- | ---------- |
| gpiochip1 | 21   | GPIO1_21  | P9_25      |
| gpiochip1 | 7    | GPIO1_7   | P9_42      |

     

## Build Instructions


make

     

## Run Instructions

```bash
sudo ./app

     

## Pin Configuration

Ensure pins are configured as GPIO before execution:

```bash
config-pin P9_25 gpio
config-pin P9_42 gpio

     

## Software Architecture

### Main Components

* `main.c`

  * Implements state machine logic
  * Handles event loop (`poll`)
  * Manages timer and button events

* `gpio.c / gpio.h`

  * Provides abstraction for:

    * Initialization
    * Read/write operations
    * Event handling
    * Cleanup

     

## Event Flow

1. `poll()` waits for:

   * Button event (GPIO interrupt)
   * Timer event (periodic)

2. Button events:

   * Rising edge → press start time recorded
   * Falling edge → press duration calculated

3. Based on duration:

   * Short press → toggle ON/OFF
   * Long press → toggle BLINK mode

4. Timer events:

   * If in BLINK state → toggle LED

     

## Key Concepts

* File descriptor based I/O
* Event-driven programming
* State machine design
* Time measurement using `CLOCK_MONOTONIC`
* Hardware abstraction in embedded systems

     

## Cleanup

Application handles graceful termination on `SIGINT` (Ctrl+C):

* Releases GPIO lines
* Closes GPIO chip

     

## Future Enhancements

* Double-click detection
* Adjustable blink frequency
* Multiple GPIO control
* Migration to `epoll` for scalability
* Configuration via command-line arguments

     

## Notes

* Only 3.3V logic levels must be used
* Avoid using 5V directly on GPIO pins
* Ensure correct wiring before execution

     

## Author

Madhumitha Meenakshi

     


