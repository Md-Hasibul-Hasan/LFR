# PID Line Follower Robot

A professional Arduino Nano based PID Line Follower Robot featuring an interactive OLED dashboard, sensor calibration, EEPROM persistence, motor testing, and real-time PID tuning.

---

## Features

- PID based line following
- OLED user interface (SSD1306 128×64)
- Interactive dashboard
- Real-time PID parameter tuning
- Sensor calibration with EEPROM save
- Motor test utility
- Sensor monitor
- Automatic settings persistence
- Factory reset
- Button navigation
- Modular `.ino` project structure

---

## Hardware

| Component | Quantity |
|-----------|---------:|
| Arduino Nano | 1 |
| TB6612FNG Motor Driver | 1 |
| SSD1306 OLED (I2C) | 1 |
| CD74HC4067 16-Channel Multiplexer | 1 |
| IR Line Sensors | 8 |
| DC Gear Motors | 2 |
| Push Buttons | 3 |
| Li-ion Battery | 1 |

---

# Pin Configuration

## Analog Pins

| Pin | Function |
|------|----------|
| A0 | MUX Signal |
| A1 | UP Button |
| A2 | DOWN Button |
| A3 | OK Button |
| A4 | OLED SDA |
| A5 | OLED SCL |

---

## Digital Pins

| Pin | Function |
|------|----------|
| D2 | MUX S0 |
| D3 | MUX S1 |
| D4 | MUX S2 |
| D5 | MUX S3 |
| D6 | TB6612 AIN1 |
| D7 | TB6612 AIN2 |
| D8 | TB6612 BIN1 |
| D9 | TB6612 PWMA |
| D10 | TB6612 PWMB |
| D12 | TB6612 BIN2 |
| D13 | Built-in LED |

---

## OLED

| Pin | Connection |
|------|------------|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

---

## Button Mapping

| Button | Pin |
|---------|-----|
| UP | A1 |
| DOWN | A2 |
| OK | A3 |

---

# Project Structure

```
PID/
│
├── PID.ino                # Main Program
├── Globals.h              # Global Variables
├── Robot.ino              # Robot Control
├── Motor.ino              # Motor Driver
├── Sensor.ino             # Sensor Processing
├── Pidd.ino               # PID Controller
├── EEPROM.ino             # Save & Load Settings
├── Oled.ino               # Dashboard & UI
├── Sensor_View.ino        # Sensor Monitor
├── Motor_Test.ino         # Motor Testing
├── Button.h               # Button Library
```

---

# Dashboard

The dashboard provides real-time parameter tuning and system configuration.

Supported item types:

- Integer
- Float
- Option
- Information
- Action

Configurable parameters include:

- KP
- KI
- KD
- Base Speed
- Line Lost Recovery
- Track Direction
- Calibration
- Motor Test
- Sensor View
- Line Follow

---

# Calibration

Calibration procedure:

1. Open Calibration
2. Long press **OK**
3. Move the robot over both black and white surfaces
4. Wait until calibration completes
5. Calibration values are automatically saved

Saved values:

- Minimum sensor values
- Maximum sensor values

---

# EEPROM

The following parameters are automatically saved.

| Parameter |
|-----------|
| KP |
| KI |
| KD |
| Base Speed |
| Line Lost Recovery |
| Track Direction |
| Sensor Calibration |

Settings are automatically restored after power-up.

---

# Line Follow

During line following the robot continuously:

- Reads sensors
- Calculates line position
- Computes PID output
- Updates motor speeds
- Displays live information on OLED

Displayed information:

- Position
- Error
- Robot Status

---

# Factory Reset

Factory reset restores all settings to their default values.

Resets:

- PID values
- Base speed
- Line lost recovery
- Track direction
- Sensor calibration

---

# Navigation

| Action | Function |
|---------|----------|
| UP | Previous |
| DOWN | Next |
| OK | Select |
| Long OK | Start calibration |
| Short OK | Back |

---

# Default Parameters

| Parameter | Default |
|-----------|--------:|
| KP | 20.0 |
| KI | 0.0 |
| KD | 10.0 |
| Base Speed | 150 |
| Inverse Track | False |
| Line Lost Recovery | Left |

---

# Future Improvements

- Auto PID tuning
- Battery voltage monitor
- Bluetooth configuration
- Wireless telemetry
- OLED graphs
- Lap timer
- Speed profiles
- Maze solving
- Junction detection
- SD card logging

---

# License

This project is released under the MIT License.

---

# Author

**Md. Hasibul Hasan**

Computer Science & Engineering

Kishoreganj University

Bangladesh
