#ifndef EDITOR_H
#define EDITOR_H

#include "IDisplay.h"

class Editor {
public:
  static const unsigned long BLINK_INTERVAL = 500;

private:
  bool _active;
  int _fieldIndex;
  int _cursorPos;
  bool _blinkState;
  unsigned long _lastBlinkTime;
  IDisplay& _disp;

public:
  Editor(IDisplay& disp)
    : _active(false), _fieldIndex(0), _cursorPos(0),
      _blinkState(true), _lastBlinkTime(0), _disp(disp) {}

  void enter() {
    _active = true;
    _fieldIndex = 0;
    _cursorPos = 0;
    _blinkState = true;
    _lastBlinkTime = _disp.getMillis();
  }

  void exit() {
    _active = false;
  }

  bool updateBlink() {
    if (!_active) return false;
    unsigned long now = _disp.getMillis();
    if (now - _lastBlinkTime >= BLINK_INTERVAL) {
      _lastBlinkTime = now;
      _blinkState = !_blinkState;
      return true;
    }
    return false;
  }

  void moveCursor(int maxDigits) {
    if (maxDigits <= 0) return;
    _cursorPos++;
    if (_cursorPos >= maxDigits) _cursorPos = 0;
  }

  void nextField(int maxFields) {
    _fieldIndex++;
    if (_fieldIndex >= maxFields) _fieldIndex = 0;
    _cursorPos = 0;
  }

  bool isActive() const { return _active; }
  int getFieldIndex() const { return _fieldIndex; }
  int getCursorPos() const { return _cursorPos; }
  bool getBlinkState() const { return _blinkState; }
};

#endif