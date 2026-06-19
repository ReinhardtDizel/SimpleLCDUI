markdown
# SimpleLCDUI

**A lightweight UI framework for character LCDs with editable fields and multi‑window navigation.**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

## Overview

SimpleLCDUI is a template-based C++ library that provides a simple way to build configuration menus on HD44780‑compatible character LCDs (16x2, 20x4, etc.). It handles:

- **Multiple windows** – switch between different screens.
- **Editable fields** – integer (digit-by-digit or linear), floating point, toggle (`<Да>`/`<Отм>`).
- **Cursor blinking** – visual feedback during editing.
- **Navigation** – move between fields and windows using external buttons.

The library works with any display class that provides the required methods (duck typing). It does **not** require abstract interfaces or virtual functions – just pass your display object as a template parameter.

## Features

- **Multi‑window** – create as many screens as you need.
- **Field types**:
  - `LabelField` – plain text label.
  - `IntField` – integer with digit‑by‑digit editing and cursor blink.
  - `LinearIntField` – integer with simple +/- increment (e.g., tension adjustment).
  - `FloatField` – read‑only float with 3 decimal places.
  - `FreqField` – frequency display with 2 decimal places.
  - `IntDispField` – read‑only integer.
  - `ToggleField` – boolean field showing `<Да>`/`<Отм>`.
  - `FloatEditField` – editable float with 2 decimal places.
- **Editor** – handles cursor position, blinking, and field switching.
- **UIManager** – connects windows, editor, and button events.
- **Platform independent** – uses only `getMillis()` from the display, no Arduino dependencies.
- **Lightweight** – all code is inline, minimal RAM usage.

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
Or copy the library folder into the lib/ directory of your project.

Quick Start
```cpp
#include <MELT_MT24S2A.h>      // your display class (must have printRus, printIntBlink, getMillis, etc.)
#include <SimpleLCDUI.h>

MELT_MT24S2A lcd(6, 8, 9,10,11,12,13,14,15,16);

int32_t myValue = 42;
IntField<MELT_MT24S2A> f(0, 0, "Val=", &myValue, 0, 5, lcd);

Field<MELT_MT24S2A>* allFields[] = { &f };
Field<MELT_MT24S2A>* editFields[] = { &f };
Window<MELT_MT24S2A> mainWindow(allFields, 1, editFields, 1, lcd);
Window<MELT_MT24S2A>* windows[] = { &mainWindow };

UIManager<MELT_MT24S2A> ui(lcd, windows, 1);

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
The library itself has no external dependencies. It expects the display class (Display) to provide the following methods:

void begin(uint8_t cols, uint8_t rows)

void clear()

void setCursor(uint8_t col, uint8_t row)

void write(uint8_t c)

void print(const char* s)

void printRus(const char* s)

void printInt(int32_t n)

void printIntBlink(int32_t n, int cursorPos, bool show)

void clearField(int width)

unsigned long getMillis()

Your display class (e.g., MELT_MT24S2A) should implement these methods. The library will be instantiated with that class as a template parameter.

License
This library is released under the MIT License. See LICENSE for details.

Author
Reinhardt Michael

GitHub: ReinhardtDizel

Email: dizel882@gmail.com