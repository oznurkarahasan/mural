# Mural Wall Plotter — How To Use

A wall-hanging drawing robot controlled via a web browser. Mural hangs from two nails on a wall, and draws SVG images using a pen and two stepper motors that control belt lengths.

---

## Table of Contents

1. [Hardware Overview](#1-hardware-overview)
2. [Pin Assignments](#2-pin-assignments)
3. [Wiring Diagram](#3-wiring-diagram)
4. [First-Time Setup: Flashing the Firmware](#4-first-time-setup-flashing-the-firmware)
5. [Connecting to Mural's Wi-Fi](#5-connecting-to-murals-wi-fi)
6. [Using the Web Interface — Step by Step](#6-using-the-web-interface--step-by-step)
7. [Preparing an SVG for Drawing](#7-preparing-an-svg-for-drawing)
8. [Drawing Process Phases Reference](#8-drawing-process-phases-reference)
9. [Tools Modal (Advanced)](#9-tools-modal-advanced)
10. [Calibration: E-Steps](#10-calibration-e-steps)
11. [Tips & Troubleshooting](#11-tips--troubleshooting)

---

## 1. Hardware Overview

| Component | Description |
|-----------|-------------|
| ESP32 (NodeMCU ESP-WROOM-32, 30-pin) | Main microcontroller |
| 2x NEMA 17 Pancake Stepper Motors | Drive the belts |
| 2x TMC2209 Stepper Motor Drivers | Control the steppers |
| MG90S Metal Gear Servo | Raises and lowers the pen |
| SSD1306 OLED Display (128x64, I2C) | Shows status and drawing progress |
| GT2 Timing Belts + 20-tooth Pulleys | Connect motors to the gondola |
| USB-C PD power supply (30W+, 12V capable) | Powers the whole system |
| LM2596 Step-down Regulator | Converts 12V to 5V for ESP32 |
| Push Button | Connected to GPIO 32 |
| WS2812B LED (x1) | Connected to GPIO 4 |

---

## 2. Pin Assignments

### Stepper Motors

| Signal | GPIO | Motor |
|--------|------|-------|
| STEP | **13** | Left motor |
| DIR | **12** | Left motor |
| ENABLE | **14** | Left motor |
| STEP | **27** | Right motor |
| DIR | **26** | Right motor |
| ENABLE | **25** | Right motor |

> Motor microstepping is set to **1/8** — 1600 steps per full rotation.

### Servo (Pen)

| Signal | GPIO |
|--------|------|
| PWM Signal | **2** |

> Servo is attached at startup and defaults to 90 degrees (pen up).

### OLED Display (I2C)

| Signal | GPIO | Note |
|--------|------|------|
| SDA | **21** | Arduino/ESP32 Wire default |
| SCL | **22** | Arduino/ESP32 Wire default |
| I2C Address | 0x3C | 128x64 SSD1306 |

> Display is rotated 180 degrees in software (setRotation(2)).

### Button

| Signal | GPIO | Wiring |
|--------|------|--------|
| Button input | **32** | One leg to GPIO 32, other leg to GND |

> Use INPUT_PULLUP in code — no external resistor needed.

### WS2812B LED

| Signal | Connection |
|--------|-----------|
| 5V | ESP32 5V (VIN) pin |
| GND | ESP32 GND |
| DIN | **GPIO 4** |

> Only 1 LED is connected, so powering from the ESP32's VIN pin is safe (draws ~60 mA max at full brightness).

### Complete Pin Summary

```
GPIO  2  -> Servo PWM (Pen)
GPIO  4  -> WS2812B LED DIN
GPIO 12  -> Left Motor DIR
GPIO 13  -> Left Motor STEP
GPIO 14  -> Left Motor ENABLE
GPIO 21  -> OLED SDA (I2C)
GPIO 22  -> OLED SCL (I2C)
GPIO 25  -> Right Motor ENABLE
GPIO 26  -> Right Motor DIR
GPIO 27  -> Right Motor STEP
GPIO 32  -> Push Button (other leg to GND)
```

---

## 3. Wiring Diagram

```
                         +-------------------------+
                         |          ESP32          |
                         |                         |
  Left Stepper Driver ---| GPIO 13 (STEP)          |
                         | GPIO 12 (DIR)           |
                         | GPIO 14 (ENABLE)        |
                         |                         |
  Right Stepper Driver --| GPIO 27 (STEP)          |
                         | GPIO 26 (DIR)           |
                         | GPIO 25 (ENABLE)        |
                         |                         |
  Servo (MG90S) ---------| GPIO  2 (PWM)           |
                         |                         |
  OLED SSD1306 ----------| GPIO 21 (SDA)           |
                         | GPIO 22 (SCL)           |
                         |                         |
  WS2812B DIN -----------| GPIO  4                 |
  WS2812B 5V  -----------| 5V (VIN)                |
  WS2812B GND -----------| GND                     |
                         |                         |
  Button leg 1 ----------| GPIO 32                 |
  Button leg 2 ----------| GND                     |
                         +-------------------------+
```

---

## 4. First-Time Setup: Flashing the Firmware

Mural uses [PlatformIO](https://platformio.org/) for building and flashing.

### Requirements

- [VS Code](https://code.visualstudio.com/) with the PlatformIO extension, or PlatformIO CLI
- USB cable connected to the ESP32

### Steps

1. Open the project folder in VS Code.
2. Connect the ESP32 via USB.
3. **Flash the firmware:**
   ```
   pio run --target upload
   ```
4. **Flash the web interface (LittleFS filesystem):**
   ```
   pio run --target uploadfs
   ```
   > This uploads the web files from the `data/www/` folder to the ESP32's flash storage. You must do this at least once, or the web interface will not load.

5. Open the Serial Monitor (baud rate: `9600`) to watch the boot log.

---

## 5. Connecting to Mural's Wi-Fi

On first boot (or if Wi-Fi credentials are lost):

1. Mural creates a Wi-Fi access point called **`Mural`**.
2. Connect to it from your phone or computer.
3. A captive portal opens automatically (or navigate to `192.168.4.1`).
4. Select your home Wi-Fi network and enter the password.
5. Click **Save**. Mural will restart and connect to your Wi-Fi.

Once connected, the OLED display shows Mural's IP address and mDNS address:

```
http://192.168.x.x
      or
http://mural.local
```

Open either address in your browser to access the web interface.

> **Tip:** `http://mural.local` works on most modern devices without needing to look up the IP address.

---

## 6. Using the Web Interface — Step by Step

The web interface guides you through a series of **phases** in order. Each phase must be completed before moving to the next.

---

### Phase 1 — Set Distance Between Hangers

**What you see:** A number input field asking for the distance between the two nails/hangers.

**What to do:**
1. Measure the horizontal distance between the two nails (or mounting points) on the wall, in **millimeters**.
2. Enter the value in the input field (e.g., `1000` for 1 meter).
3. Click **Set distance**.

> This value is critical for drawing accuracy. Measure carefully.

---

### Phase 2 — Select SVG Image

**What you see:** A file picker and a preview area.

**What to do:**
1. Click the file input and select an SVG file from your computer.
2. A preview of the SVG will appear.
3. Use the **D-pad** controls to pan the image and the **zoom in/out** buttons to resize it. Use the **reset** button to undo transformations.
4. Click **Preview drawing** to process the SVG.

> Use **black and white line art SVGs** for best results. Complex gradients and raster images are not supported.

---

### Phase 3 — Choose Render Type

**What you see:** Two rendering options.

| Option | Best For |
|--------|----------|
| **Path Tracing** | Most drawings; follows SVG paths directly |
| **Vector to Raster to Vector** | Preserves stroke width and thickness |

Click the option that suits your image.

---

### Phase 4 — Drawing Preview and Settings

**What you see:** A processed preview of what will actually be drawn, along with adjustable settings.

**Settings:**

| Setting | Description |
|---------|-------------|
| **Infill Density** | Adds hatching/fill inside shapes (0 = outlines only) |
| **Despeckle** | Removes small noise artifacts (higher = removes more tiny features) |
| **Flatten Paths** | Simplifies curves into straight line segments |

**What to do:**
1. Adjust the settings until the preview looks right.
2. Click **Accept** when you are happy with the result.
3. The processed drawing will be uploaded to the ESP32.

---

### Phase 5 — Retract Belts

**What you see:** Two toggle switches (Left motor / Right motor) and a "Belts are retracted" button.

**What to do:**
1. Make sure the belt loops are placed on the homing screws of the gondola.
2. Toggle **Left motor** ON to start retracting the left belt. The motor will run until it stalls against the stop screw.
3. Toggle it **OFF** as soon as the motor starts stalling (within a second or two) — don't let it stall for long.
4. Repeat for **Right motor**.
5. Both belts should now be fully retracted (shortest possible length).
6. Click **Belts are retracted** to proceed.

> **Warning:** Do not leave motors stalling for extended periods — this generates heat and can damage drivers.

---

### Phase 6 — Extend Belts to Home Position

**What you see:** An "Extend to home position" button and a loading spinner.

**What to do:**
1. Make sure the gondola is free to move and the belts are unobstructed.
2. Click **Extend to home position**.
3. The motors will automatically extend the belts to the calibrated home position. Wait for the spinner to disappear.

> If the motors skip steps during this phase, the drawing position will be inaccurate. Re-hang Mural and ensure the belts run freely.

---

### Phase 7 — Pen Calibration

**What you see:** A slider and +/- buttons to control the servo, plus a "Pen is touching the wall" button.

**What to do:**
1. Insert the pen into the gondola so that it is **close to, but not touching** the wall. Secure it with the bolt.
2. Use the **slider** or **+/-** buttons to move the servo, pushing the pen toward the wall.
3. When the pen tip is **just touching** the wall surface, click **Pen is touching the wall**.

> This sets the "pen down" servo angle. From now on, Mural knows exactly how far to push the pen during drawing.

---

### Phase 8 — Begin Drawing

**What you see:** Two buttons — "Begin Drawing" and "Reset".

**What to do:**
- Click **Begin Drawing** to start the drawing.
  The web server will shut down. The OLED display will show drawing progress (0% to 100%).
- Click **Reset** to go back to the beginning (Phase 1) without drawing.

> Once drawing begins, the web interface will be **unavailable** until drawing is complete. The ESP32 restarts automatically when finished, and the web interface becomes accessible again.

---

## 7. Preparing an SVG for Drawing

For best results, prepare your SVG file following these guidelines:

- **Use line art only** — outlines work best, not filled shapes with gradients.
- **Black and white only** — colors are not used by the plotter.
- **Keep it simple** — fewer, cleaner paths = better results and faster drawing.
- The SVG will be **scaled to fit** the drawing area automatically:
  - Drawing area width = 60% of the pin distance (e.g., 600 mm for 1000 mm pin distance)
  - 20% margin on each side, 20% margin at the top
  - Height is scaled proportionally to the SVG's aspect ratio
- Recommended tools for creating or simplifying SVGs:
  - [Inkscape](https://inkscape.org/) (free)
  - [Adobe Illustrator](https://www.adobe.com/products/illustrator.html)
  - [SVGomg](https://jakearchibald.github.io/svgomg/) (free online SVG optimizer)

---

## 8. Drawing Process Phases Reference

| Phase Name | What Happens |
|------------|-------------|
| `SetTopDistance` | User enters the distance between the two wall hangers |
| `SvgSelect` | User uploads and configures the SVG image |
| `RetractBelts` | User manually retracts both belts to their home stops |
| `ExtendToHome` | Motors automatically extend belts to the home position |
| `PenCalibration` | User sets the servo angle for the pen-down position |
| `BeginDrawing` | User starts the drawing or resets the whole process |

The system always starts at **SetTopDistance** on power-on or after reset.

---

## 9. Tools Modal (Advanced)

Accessible via the gear icon in the top-right corner (visible during the **Set Distance** phase).

| Tool | Description |
|------|-------------|
| **Left Motor slider** | Manually retract (-1) or extend (+1) the left belt |
| **Right Motor slider** | Manually retract (-1) or extend (+1) the right belt |
| **Park Servo** | Moves the servo to the parked (pen up) position at 90 degrees |
| **Extend 1000mm** | Extends the belt by a fixed 1000 mm — used for E-steps calibration |

---

## 10. Calibration: E-Steps

E-steps calibration ensures that the motor moves the correct physical distance per step.

**When to calibrate:** If your drawings come out distorted or scaled incorrectly.

**Steps:**
1. Open the Tools Modal (gear icon).
2. Mark a reference point on the belt or measure from a fixed starting point.
3. Click **Extend 1000mm**.
4. Measure the actual distance the belt moved.
5. Calculate the correction:
   ```
   new_steps_per_rotation = current_steps_per_rotation x (1000 / measured_mm)
   ```
6. Update `stepsPerRotation` in `src/movement.h` (currently `200 * 8 = 1600`).
7. Re-flash the firmware.

**Current geometry constants** (in `src/movement.h`):

| Constant | Value | Description |
|----------|-------|-------------|
| `stepsPerRotation` | 1600 | Steps per full motor rotation (200 x 1/8 microstepping) |
| `diameter` | 12.69 mm | Effective pulley diameter |
| `d_t` | 76.027 mm | Distance between belt tangent points |
| `HOME_Y_OFFSET_MM` | 350 mm | Y coordinate of home position in image space |

---

## 11. Tips and Troubleshooting

### Mural is not reachable at http://mural.local
- Try the IP address shown on the OLED display instead.
- mDNS may not work on some Android devices — use the IP address directly.

### The drawing is skewed or inaccurate
- Re-measure the pin distance and re-enter it exactly.
- Check that neither belt skipped steps during the "Extend to home" phase.
- Run E-steps calibration (see Section 10).

### Motors stall immediately when retracting
- The belts may already be fully retracted. Toggle the motor off immediately and click "Belts are retracted".

### The OLED display shows "SSD1306 allocation failed"
- Check the I2C wiring: SDA to GPIO 21, SCL to GPIO 22.
- Verify the OLED's I2C address is 0x3C.

### The web interface does not load (blank page or 404)
- The LittleFS filesystem may not have been uploaded.
- Run: `pio run --target uploadfs`

### Drawing starts but the pen never touches the wall
- Pen calibration was set incorrectly. Reset the device and redo pen calibration (Phase 7).

### The ESP32 restarts after drawing is done
- This is normal. The ESP32 restarts automatically after drawing is complete so the web interface becomes accessible again.

### Want to connect to a different Wi-Fi network
- The Wi-Fi credentials are stored by the WiFiManager library.
- To reset them and re-enter the captive portal setup, erase the ESP32 flash and re-upload, or add a credentials-reset trigger in code.

---

*Mural firmware built with PlatformIO and the ESP32 Arduino framework.*
*For the kinematic model, see KinematicModel.md*
