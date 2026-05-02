# BeagleBone Black GPIO – LED Toggle with Button (libgpiod)

## 📌 Overview

This project demonstrates **GPIO handling in Embedded Linux (BeagleBone Black)** using **libgpiod**.

Features implemented:

* Button input using **GPIO interrupt (edge detection)**
* LED control using **GPIO output**
* **Debounce logic** for stable button press detection
* Clean GPIO abstraction using wrapper functions

      

## ⚙️ Hardware Setup

### 🔴 LED Connection

      
P9_25 (GPIO1_21) → Resistor (220Ω–1kΩ) → LED (+)
LED (–) → GND (P9_1 or P9_2)
      

      

### 🔘 Button Connection (Pull-down)

      
P9_42 (GPIO1_7) → Button → 3.3V (P9_3 or P9_4)
P9_42 → 10kΩ resistor → GND
      

      

## ⚠️ Important Notes

* Use **3.3V ONLY** (never 5V)
* LED **must have resistor**
* Button requires **pull-down resistor**
* Ensure pins are configured as GPIO:

      bash
config-pin P9_25 gpio
config-pin P9_42 gpio
      

      

## 🧠 GPIO Mapping

| Code              | GPIO     | Pin   |
|------------------ | -------- | ----- |
| gpiochip1 line 21 | GPIO1_21 | P9_25 |
| gpiochip1 line 7  | GPIO1_7  | P9_42 |



## 🏗️ Project Structure

      
.
├── src/
│   ├── main.c        # Application logic
│   └── gpio.c        # GPIO wrapper implementation
├── inc/
│   └── gpio.h        # GPIO wrapper header
├── Makefile
└── README.md
      

      

## 🔧 Build

      bash
make
      

      

## ▶️ Run

      bash
sudo ./app
      

      

## How It Works

### 🔹 Initialization

* Opens GPIO chip (`gpiochip1`)
* Configures:

  * LED → output
  * Button → rising edge interrupt


### 🔹 Main Loop

1. Waits for button event:

   
   gpio_wait_for_event()
        

2. Reads event:
        
   gpio_read_event()        

3. Applies debounce:
        
   diff < debounce_ms → ignore        

4. On valid press:
        
   led_state = !led_state
   gpio_set()        

      

### 🔹 Debounce Logic

* Uses `CLOCK_MONOTONIC`
* Filters noise within **200 ms**
* Prevents multiple triggers per press

      

## 📚 Key Concepts Used

* **libgpiod (modern GPIO interface)**
* Edge-triggered interrupts
* File-descriptor-based event handling
* Time-based debounce
* Modular driver abstraction

      

## 🧪 Expected Output
      
Press button to toggle LED (Ctrl+C to exit)
Waiting for button press...
Event detected: 1
LED: 1
Event detected: 1
LED: 0
      
      

## 🧹 Cleanup

Press `Ctrl+C`:

* Releases GPIO lines
* Closes chip properly

      

## 📈 Future Improvements

* Add **falling edge detection**
* Implement **long press / double press**
* Add **state machine architecture**
* Integrate **timerfd + poll()**
* Extend to multi-device control

      

## 👨‍💻 Author

Madhu Kaushik

      

## 📝 License

This project is for learning and experimentation purposes.
