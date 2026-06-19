# SimpleLCDUI

**A lightweight UI framework for character LCDs with editable fields and multi‑window navigation.**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Ready-orange)](https://platformio.org)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-green)](https://www.arduino.cc)

## Overview

SimpleLCDUI provides a simple way to build configuration menus on HD44780‑compatible character LCDs (16x2, 20x4, etc.). It handles:

- **Multiple windows** – switch between different screens.
- **Editable fields** – integer, floating point, toggle (`<Да>`/`<Отм>`).
- **Cursor blinking** – visual feedback during editing.
- **Navigation** – move between fields and windows using external buttons.

The library works with any display that implements the `IDisplay` interface – see [MELT_MT24S2A](https://github.com/ReinhardtDizel/MELT_MT24S2A) for an example that supports the Russian МЭЛТ MT-24S2A display.

## Features

- **Multi‑window** – create as many screens as you need.
- **Field types**:
  - `IntField` – integer with digit‑by‑digit editing.
  - `LinearIntField` – integer with simple +/- increment.
  - `FloatField` – read‑only float with 3 decimal places.
  - `FreqField` – frequency display with 2 decimal places.
  - `ToggleField` – boolean `Yes`/`Cancel` field.
  - `FloatEditField` – editable float.
- **Editor** – handles cursor position, blinking, and field switching.
- **Platform independent** – uses abstract `IDisplay`, no hardware dependencies.
- **Lightweight** – minimal RAM usage, suited for ATmega328.

## Installation

### Arduino IDE
1. Download the repository as a ZIP file.
2. In Arduino IDE, go to **Sketch → Include Library → Add .ZIP Library** and select the downloaded file.
3. Restart the IDE.

### PlatformIO
Add the following to your `platformio.ini`:
```ini 
lib_deps =
    https://github.com/ReinhardtDizel/SimpleLCDUI.git

```

Or place the library folder manually in the lib/ directory of your project.

Quick Start
cpp
#include <MELT_MT24S2A.h>      // your display (implements IDisplay)
#include <SimpleLCDUI.h>

MELT_MT24S2A lcd(6, 8, 9,10,11,12,13,14,15,16);
IDisplay &display = lcd;       // reference to the interface

int32_t myValue = 42;
IntField f(0, 0, "Val=", &myValue, 0, 5);   // col, row, label, value, id, width

Field* allFields[] = { &f };
Field* editFields[] = { &f };
Window mainWindow(allFields, 1, editFields, 1);
Window* windows[] = { &mainWindow };
const int windowCount = 1;

UIManager ui(display, windows, windowCount);

void setup() {
  lcd.begin(24, 2);
  ui.begin();
}

void loop() {
  // Obtain events from your button library (e.g., OneButton)
  if (btnUp.pressed()) ui.incrementValue();
  if (btnDown.pressed()) ui.decrementValue();
  if (btnEnter.shortPress()) ui.editAction();
  if (btnEnter.longPress()) ui.exitEdit();
  // ...
  ui.update();
}
```
See the examples folder for a complete working example.

Dependencies
IDisplay – an abstract interface that your display class must implement.
The library includes IDisplay.h; you only need to provide the concrete implementation (e.g., MELT_MT24S2A).

No other external libraries are required. Button handling is left to the user (recommended: OneButton).

License
This library is released under the MIT License. See LICENSE for details.

Author
Reinhardt Michael

GitHub: ReinhardtDizel

Email: dizel882@gmail.com