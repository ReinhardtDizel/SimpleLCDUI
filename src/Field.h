#ifndef FIELD_H
#define FIELD_H

#include "IDisplay.h"
#include <string.h>
#include <stdio.h>

// ---------- Вспомогательные утилиты ----------
namespace SimpleLCDUI_Utils {
  template <typename T> T clamp(T val, T lo, T hi) {
    return val < lo ? lo : val > hi ? hi : val;
  }
  template <typename T> T abs(T x) { return x < 0 ? -x : x; }
  inline int32_t pow10(int e) { int32_t r=1; while(e--) r*=10; return r; }
  inline int digits(int32_t x) {
    if(!x) return 1;
    x = abs(x);
    int n = 0;
    while(x) { n++; x /= 10; }
    return n;
  }
  inline void floatToFixed3(float v, char* buf, int sz) {
    int w = (int)v;
    int f = abs((int)((v - w) * 1000));
    snprintf(buf, sz, "%d.%03d", w, f);
  }
}

/**
 * @brief Базовый класс для любого поля на экране.
 */
class Field {
protected:
  uint8_t _col, _row;
  const char* _label;
  bool _useRusPrint;
  int _fieldId;
  IDisplay& _disp;

public:
  Field(uint8_t col, uint8_t row, const char* label, bool rus, int id, IDisplay& disp)
    : _col(col), _row(row), _label(label), _useRusPrint(rus), _fieldId(id), _disp(disp) {}

  virtual ~Field() {}

  virtual void draw(bool editMode, int cursorPos, bool blinkState) = 0;
  virtual void modifyDigit(int delta, int cursorPos) {}
  virtual int getMaxDigits() { return 0; }

  int getFieldId() const { return _fieldId; }
  bool isEditable() const { return _fieldId >= 0; }
};

/**
 * @brief Метка (нередактируемая строка).
 */
class LabelField : public Field {
public:
  LabelField(uint8_t col, uint8_t row, const char* label, bool rus, IDisplay& disp)
    : Field(col, row, label, rus, -1, disp) {}

  void draw(bool editMode, int cursorPos, bool blinkState) override {
    _disp.setCursor(_col, _row);
    if (_useRusPrint) _disp.printRus(_label);
    else _disp.print(_label);
  }
};

/**
 * @brief Целое число с поразрядным редактированием и миганием.
 */
class IntField : public Field {
  int32_t* _value;
  int _maxDigits;

public:
  IntField(uint8_t col, uint8_t row, const char* label, int32_t* val, int fieldId,
           int maxDigits, IDisplay& disp)
    : Field(col, row, label, false, fieldId, disp), _value(val), _maxDigits(maxDigits) {}

  void draw(bool editMode, int cursorPos, bool blinkState) override {
    _disp.setCursor(_col, _row);
    _disp.print(_label);
    int len = SimpleLCDUI_Utils::digits(*_value);
    if (cursorPos >= len) cursorPos = len - 1;
    _disp.setCursor(_col + strlen(_label), _row);
    for (int i = 0; i < _maxDigits; i++) _disp.write(' ');
    _disp.setCursor(_col + strlen(_label), _row);
    if (editMode) _disp.printIntBlink(*_value, cursorPos, blinkState);
    else _disp.printInt(*_value);
  }

  void modifyDigit(int delta, int cursorPos) override {
    int len = SimpleLCDUI_Utils::digits(*_value);
    if (cursorPos >= len) cursorPos = len - 1;
    int pos = len - 1 - cursorPos;
    int divisor = SimpleLCDUI_Utils::pow10(pos);
    int digit = (SimpleLCDUI_Utils::abs(*_value) / divisor) % 10;
    digit = (digit + delta + 10) % 10;
    bool neg = (*_value < 0);
    int32_t av = SimpleLCDUI_Utils::abs(*_value);
    int32_t old = (av / divisor % 10) * divisor;
    av = av - old + digit * divisor;
    *_value = neg ? -av : av;
  }

  int getMaxDigits() override { return _maxDigits; }
};

/**
 * @brief Целое число с линейным изменением (±1) и миганием всего числа.
 */
class LinearIntField : public Field {
  int32_t* _value;
  int32_t _minVal, _maxVal;
  int _width;

public:
  LinearIntField(uint8_t col, uint8_t row, const char* label, int32_t* val, int fieldId,
                 int32_t minV, int32_t maxV, int width, IDisplay& disp)
    : Field(col, row, label, false, fieldId, disp), _value(val),
      _minVal(minV), _maxVal(maxV), _width(width) {}

  void draw(bool editMode, int cursorPos, bool blinkState) override {
    _disp.setCursor(_col, _row);
    _disp.print(_label);
    _disp.setCursor(_col + strlen(_label), _row);
    for (int i = 0; i < _width; i++) _disp.write(' ');
    _disp.setCursor(_col + strlen(_label), _row);
    char buf[4];
    snprintf(buf, sizeof(buf), "%d", *_value);
    if (editMode && !blinkState) _disp.write(' ');
    else _disp.print(buf);
  }

  void modifyDigit(int delta, int cursorPos) override {
    *_value = SimpleLCDUI_Utils::clamp(*_value + delta, _minVal, _maxVal);
  }

  int getMaxDigits() override { return _width; }
};

/**
 * @brief Число с плавающей точкой (только чтение, 3 знака после запятой).
 */
class FloatField : public Field {
  float* _value;

public:
  FloatField(uint8_t col, uint8_t row, const char* label, float* val, IDisplay& disp)
    : Field(col, row, label, false, -1, disp), _value(val) {}

  void draw(bool editMode, int cursorPos, bool blinkState) override {
    _disp.setCursor(_col, _row);
    _disp.print(_label);
    char buf[10];
    SimpleLCDUI_Utils::floatToFixed3(*_value, buf, sizeof(buf));
    _disp.print(buf);
    _disp.print(" ");
  }
};

/**
 * @brief Частота (только чтение, 2 знака после запятой).
 */
class FreqField : public Field {
  float* _value;

public:
  FreqField(uint8_t col, uint8_t row, const char* label, float* val, IDisplay& disp)
    : Field(col, row, label, false, -1, disp), _value(val) {}

  void draw(bool editMode, int cursorPos, bool blinkState) override {
    _disp.setCursor(_col, _row);
    _disp.print(_label);
    int whole = (int)(*_value);
    int frac = (int)((*_value - whole) * 100 + 0.5);
    if (frac >= 100) { whole++; frac = 0; }
    char buf[10];
    snprintf(buf, sizeof(buf), "%d.%02d", whole, frac);
    _disp.print(buf);
    _disp.print(" ");
  }
};

/**
 * @brief Целое число (только чтение).
 */
class IntDispField : public Field {
  int32_t* _value;

public:
  IntDispField(uint8_t col, uint8_t row, const char* label, int32_t* val, IDisplay& disp)
    : Field(col, row, label, false, -1, disp), _value(val) {}

  void draw(bool editMode, int cursorPos, bool blinkState) override {
    _disp.setCursor(_col, _row);
    _disp.print(_label);
    _disp.printInt(*_value);
    _disp.print("  ");
  }
};

/**
 * @brief Логическое поле (<Да>/<Отм>).
 */
class ToggleField : public Field {
  bool* _value;

public:
  ToggleField(uint8_t col, uint8_t row, bool* val, int fieldId, IDisplay& disp)
    : Field(col, row, "", false, fieldId, disp), _value(val) {}

  void draw(bool editMode, int cursorPos, bool blinkState) override {
    _disp.setCursor(_col, _row);
    if (editMode && !blinkState) {
      for (int i = 0; i < 4; i++) _disp.write(' ');
    } else {
      _disp.print(*_value ? "<Да>" : "<Отм>");
    }
  }

  void modifyDigit(int delta, int cursorPos) override {
    *_value = !(*_value);
  }

  int getMaxDigits() override { return 1; }
};

/**
 * @brief Редактируемое число с плавающей точкой (2 знака после запятой).
 */
class FloatEditField : public Field {
  float* _value;
  int _width;

public:
  FloatEditField(uint8_t col, uint8_t row, const char* label, float* val, int fieldId,
                 int width, IDisplay& disp)
    : Field(col, row, label, false, fieldId, disp), _value(val), _width(width) {}

  void draw(bool editMode, int cursorPos, bool blinkState) override {
    _disp.setCursor(_col, _row);
    _disp.print(_label);
    _disp.setCursor(_col + strlen(_label), _row);
    for (int i = 0; i < _width; i++) _disp.write(' ');
    _disp.setCursor(_col + strlen(_label), _row);
    int whole = (int)(*_value);
    int frac = (int)((*_value - whole) * 100 + 0.5);
    char buf[8];
    snprintf(buf, sizeof(buf), "%d.%02d", whole, frac);
    int len = strlen(buf);
    if (cursorPos >= len) cursorPos = len - 1;
    if (editMode) {
      for (int i = 0; i < len; i++) {
        if (i == cursorPos && !blinkState) _disp.write(' ');
        else _disp.write(buf[i]);
      }
    } else {
      _disp.print(buf);
    }
  }

  void modifyDigit(int delta, int cursorPos) override {
    *_value += delta * 0.01f;
    if (*_value < 0) *_value = 0;
    if (*_value > 99.99f) *_value = 99.99f;
  }

  int getMaxDigits() override { return _width; }
};

#endif