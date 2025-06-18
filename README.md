# SmartStick
Smart Stick for Visually Impaired 

This project demonstrates an Arduino-based obstacle detector for a Smart Stick that can be used for visually impaired that combines UV, IR, and LiDAR sensors. A servo sweeps the LiDAR module horizontally at knee height to build a simple angular map of nearby objects while IR sensors watch the immediate left and right sides. UV data is logged concurrently.

## Hardware
- **Arduino** compatible board (Uno, Nano, etc.)
- **LiDAR** time-of-flight sensor (VL53L0X recommended)
- **UV sensor** (e.g. VEML6075)
- **Two IR proximity sensors** for left and right detection
- **Standard hobby servo** to rotate the LiDAR
- **Buzzer** for audible alerts
- **Vibration motor** for haptic feedback

## Wiring Overview
- **LiDAR** and **UV sensor** share the I2C bus on `SDA` and `SCL`
- **IR sensors** connect to analog inputs `A0` and `A1`
- **Servo** control wire to digital pin `D9`
- **Buzzer** to digital pin `D8`
- **Vibration motor** to digital pin `D7`
- Provide 5&nbsp;V and GND to all components as required

## How It Works
1. At startup the IR sensors take a few readings to establish a baseline.
2. In the main loop the servo sweeps the LiDAR from 0–180° in 15° steps. At each
   angle several LiDAR samples are averaged for a smoother reading.
3. Left and right IR sensors measure relative changes from their baselines to
   detect nearby obstacles that the LiDAR may miss.
4. Whenever an obstacle is within 80&nbsp;cm (or the IR values exceed the set
   threshold) the sketch triggers a short beep on the buzzer and powers the
   vibration motor so the user feels and hears the warning.
5. For each servo position a CSV-style line of data is printed to the serial
   monitor including angle, averaged distance, IR deltas and UV levels. This data
   can be logged to build a simple polar map of the surroundings at knee height.

## Dependencies
Install these libraries from the Arduino Library Manager:
- **Adafruit VL53L0X**
- **Adafruit VEML6075**
- **Servo** (built-in with most Arduino IDE installs)

## Usage
1. Wire up the sensors and servo as described above.
2. Open `SmartStick.ino` in the Arduino IDE.
3. Select your board and port, then upload.
4. Open the Serial Monitor at 9600 baud to observe the streaming sensor data.
5. Attach the rig at knee level and walk around. The serial output can be
   imported into a spreadsheet or visualisation tool for a rudimentary map.
