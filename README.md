# Dino Game on Arduino Nano with ST7789 TFT

A small, battery-powered handheld Dino game using:

- Arduino **Nano V3.0**
- **ST7789** 240×240 TFT display
- Two push-buttons (Jump / Duck)
- **Li-Po 3.7 V 1100 mAh** battery
- **TP4056** Li-Po charger / protection board
- **MT3608** boost converter (3.7 V → 5 V)
- **SPST slide switch** for power

## Features
- Dino jumps and ducks with two buttons  
- Random obstacles, score counter
- Game over on collision
- Battery-powered with charging over USB-C

### Wiring Table
Battery (+) → TP4056 B+
Battery (–) → TP4056 B–

TP4056 OUT+ → Switch → MT3608 IN+
TP4056 OUT– → MT3608 IN–

MT3608 OUT+ → Nano 5V
MT3608 OUT– → Nano GND

### TFT ST7789 Wiring
TFT CS → Nano D10
TFT DC → Nano D8
TFT RST → Nano D9
TFT BLK → Nano D7 (or → 5V for always-on backlight)
TFT SCK → Nano D13
TFT MOSI → Nano D11
TFT VCC → Nano 5V
TFT GND → Nano GND

### Button Wiring
Jump Button → Nano D3 → GND
Duck Button → Nano D2 → GND

Each button connects between the pin and **GND**.

### Adjust the MT3608 Boost Converter
1. Disconnect the Nano.  
2. Turn the slide switch **ON** to power the MT3608.  
3. Measure voltage at MT3608 **OUT+** to **OUT-** with a multimeter.  
4. Adjust the tiny screw on the MT3608 until the output is **5.0 V**.  
5. Turn switch **OFF** and then connect MT3608 OUT+ to Nano 5V.

## Software Setup
### 1. Install Arduino IDE

### 2. Install Required Libraries
In **Arduino IDE → Tools → Manage Libraries…**
- Install **Adafruit GFX Library**
- Install **Adafruit ST7735 and ST7789 Library**

### 3. Load the Code
1. Open the provided `dino_game.ino`
2. In **Tools → Board**, choose **Arduino Nano**.  
3. Choose the correct **COM port**.  
4. Click **Upload**.
