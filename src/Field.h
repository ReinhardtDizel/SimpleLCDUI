/**
 * @file Field.h
 * @brief Базовый класс поля и конкретные типы полей.
 *
 * Все классы параметризованы типом Display, который должен предоставлять
 * методы для вывода на экран (print, printRus, printInt, printIntBlink, clearField и др.).
 * Также требуется метод getMillis() для работы с временем.
 * Добавлены подсказки для автоматического размещения: _rowHint (строка) и _isLabel (заголовок).
 */
#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Forward declaration
template<typename Display> class Window;

namespace SimpleLCDUI_Utils {
    template <typename T> T clamp(T val, T lo, T hi) { return val < lo ? lo : val > hi ? hi : val; }
    template <typename T> T absVal(T x) { return x < 0 ? -x : x; }
    inline int32_t pow10(int e) { int32_t r=1; while(e--) r*=10; return r; }
    inline int digits(int32_t x) { if(!x) return 1; x = absVal(x); int n=0; while(x){ n++; x/=10; } return n; }
    inline void floatToFixed3(float v, char* buf, int sz) {
        int w = (int)v; int f = absVal((int)((v - w) * 1000));
        snprintf(buf, sz, "%d.%03d", w, f);
    }
}

template<typename Display>
class Field {
protected:
    uint8_t _col, _row;
    const char* _label;
    bool _useRusPrint;
    int _fieldId;
    Display& _disp;

    uint8_t _rowHint;    // желаемая строка (0,1,...)
    bool    _isLabel;    // true, если это метка-заголовок

public:
    Field(uint8_t col, uint8_t row, const char* label, bool rus, int id, Display& disp,
          uint8_t rowHint = 0, bool isLabel = false)
        : _col(col), _row(row), _label(label), _useRusPrint(rus), _fieldId(id), _disp(disp),
          _rowHint(rowHint), _isLabel(isLabel) {}

    virtual ~Field() {}
    virtual void draw(bool editMode, int cursorPos, bool blinkState) = 0;
    virtual void modifyDigit(int delta, int cursorPos) {}
    virtual int getMaxDigits() { return 0; }
    virtual uint8_t getWidth() const = 0;   // общая ширина поля (метка + значение)

    int getFieldId() const { return _fieldId; }
    bool isEditable() const { return _fieldId >= 0; }

    friend class Window<Display>;
};

// ---------- Конкретные поля ----------

template<typename Display>
class LabelField : public Field<Display> {
public:
    LabelField(uint8_t col, uint8_t row, const char* label, bool rus, Display& disp,
               uint8_t rowHint = 0)
        : Field<Display>(col, row, label, rus, -1, disp, rowHint, true) {}

    void draw(bool, int, bool) override {
        this->_disp.setCursor(this->_col, this->_row);
        if (this->_useRusPrint) this->_disp.printRus(this->_label);
        else this->_disp.print(this->_label);
    }

    uint8_t getWidth() const override {
        return strlen(this->_label);
    }
};

template<typename Display>
class IntField : public Field<Display> {
    int32_t* _value;
    int _maxDigits;
public:
    IntField(uint8_t col, uint8_t row, const char* label, int32_t* val, int fieldId,
             int maxDigits, Display& disp, uint8_t rowHint = 0)
        : Field<Display>(col, row, label, false, fieldId, disp, rowHint), _value(val), _maxDigits(maxDigits) {}

    void draw(bool editMode, int cursorPos, bool blinkState) override {
        this->_disp.setCursor(this->_col, this->_row);
        this->_disp.print(this->_label);
        int len = SimpleLCDUI_Utils::digits(*_value);
        if (cursorPos >= len) cursorPos = len - 1;
        this->_disp.setCursor(this->_col + strlen(this->_label), this->_row);
        for (int i = 0; i < _maxDigits; i++) this->_disp.write(' ');
        this->_disp.setCursor(this->_col + strlen(this->_label), this->_row);
        if (editMode) this->_disp.printIntBlink(*_value, cursorPos, blinkState);
        else this->_disp.printInt(*_value);
    }

    void modifyDigit(int delta, int cursorPos) override {
        int len = SimpleLCDUI_Utils::digits(*_value);
        if (cursorPos >= len) cursorPos = len - 1;
        int pos = len - 1 - cursorPos;
        int divisor = SimpleLCDUI_Utils::pow10(pos);
        int digit = (SimpleLCDUI_Utils::absVal(*_value) / divisor) % 10;
        digit = (digit + delta + 10) % 10;
        bool neg = (*_value < 0);
        int32_t av = SimpleLCDUI_Utils::absVal(*_value);
        int32_t old = (av / divisor % 10) * divisor;
        av = av - old + digit * divisor;
        *_value = neg ? -av : av;
    }

    int getMaxDigits() override { return _maxDigits; }
    uint8_t getWidth() const override { return strlen(this->_label) + _maxDigits; }
};

template<typename Display>
class LinearIntField : public Field<Display> {
    int32_t* _value;
    int32_t _minVal, _maxVal;
    int _width;
public:
    LinearIntField(uint8_t col, uint8_t row, const char* label, int32_t* val, int fieldId,
                   int32_t minV, int32_t maxV, int width, Display& disp, uint8_t rowHint = 0)
        : Field<Display>(col, row, label, false, fieldId, disp, rowHint), _value(val),
          _minVal(minV), _maxVal(maxV), _width(width) {}

    void draw(bool editMode, int cursorPos, bool blinkState) override {
        this->_disp.setCursor(this->_col, this->_row);
        this->_disp.print(this->_label);
        this->_disp.setCursor(this->_col + strlen(this->_label), this->_row);
        for (int i = 0; i < _width; i++) this->_disp.write(' ');
        this->_disp.setCursor(this->_col + strlen(this->_label), this->_row);
        char buf[4];
        snprintf(buf, sizeof(buf), "%d", *_value);
        if (editMode && !blinkState) this->_disp.write(' ');
        else this->_disp.print(buf);
    }

    void modifyDigit(int delta, int cursorPos) override {
        *_value = SimpleLCDUI_Utils::clamp(*_value + delta, _minVal, _maxVal);
    }

    int getMaxDigits() override { return _width; }
    uint8_t getWidth() const override { return strlen(this->_label) + _width; }
};

template<typename Display>
class FloatField : public Field<Display> {
    float* _value;
public:
    FloatField(uint8_t col, uint8_t row, const char* label, float* val, Display& disp,
               uint8_t rowHint = 0)
        : Field<Display>(col, row, label, false, -1, disp, rowHint), _value(val) {}

    void draw(bool, int, bool) override {
        this->_disp.setCursor(this->_col, this->_row);
        this->_disp.print(this->_label);
        char buf[10];
        SimpleLCDUI_Utils::floatToFixed3(*_value, buf, sizeof(buf));
        this->_disp.print(buf);
        this->_disp.print(" ");
    }

    uint8_t getWidth() const override {
        return strlen(this->_label) + 6;   // "X.XXX " (5 + 1 пробел)
    }
};

template<typename Display>
class FreqField : public Field<Display> {
    float* _value;
public:
    FreqField(uint8_t col, uint8_t row, const char* label, float* val, Display& disp,
              uint8_t rowHint = 0)
        : Field<Display>(col, row, label, false, -1, disp, rowHint), _value(val) {}

    void draw(bool, int, bool) override {
        this->_disp.setCursor(this->_col, this->_row);
        this->_disp.print(this->_label);
        int whole = (int)(*_value);
        int frac = (int)((*_value - whole) * 100 + 0.5);
        if (frac >= 100) { whole++; frac = 0; }
        char buf[10];
        snprintf(buf, sizeof(buf), "%d.%02d", whole, frac);
        this->_disp.print(buf);
        this->_disp.print(" ");
    }

    uint8_t getWidth() const override {
        return strlen(this->_label) + 6;   // "XX.XX " (5 + 1 пробел)
    }
};

template<typename Display>
class IntDispField : public Field<Display> {
    int32_t* _value;
public:
    IntDispField(uint8_t col, uint8_t row, const char* label, int32_t* val, Display& disp,
                 uint8_t rowHint = 0)
        : Field<Display>(col, row, label, false, -1, disp, rowHint), _value(val) {}

    void draw(bool, int, bool) override {
        this->_disp.setCursor(this->_col, this->_row);
        this->_disp.print(this->_label);
        this->_disp.printInt(*_value);
        this->_disp.print("  ");
    }

    uint8_t getWidth() const override {
        return strlen(this->_label) + 7;   // "XXXXX  " (5 цифр + 2 пробела)
    }
};

template<typename Display>
class ToggleField : public Field<Display> {
    bool* _value;
public:
    ToggleField(uint8_t col, uint8_t row, bool* val, int fieldId, Display& disp,
                uint8_t rowHint = 0)
        : Field<Display>(col, row, "", false, fieldId, disp, rowHint), _value(val) {}

    void draw(bool editMode, int cursorPos, bool blinkState) override {
        this->_disp.setCursor(this->_col, this->_row);
        if (editMode && !blinkState) {
            for (int i = 0; i < 4; i++) this->_disp.write(' ');
        } else {
            this->_disp.print(*_value ? "<Да>" : "<Отм>");
        }
    }

    void modifyDigit(int delta, int cursorPos) override { *_value = !(*_value); }
    int getMaxDigits() override { return 1; }
    uint8_t getWidth() const override { return 4; }
};

template<typename Display>
class FloatEditField : public Field<Display> {
    float* _value;
    int _width;
public:
    FloatEditField(uint8_t col, uint8_t row, const char* label, float* val, int fieldId,
                   int width, Display& disp, uint8_t rowHint = 0)
        : Field<Display>(col, row, label, false, fieldId, disp, rowHint), _value(val), _width(width) {}

    void draw(bool editMode, int cursorPos, bool blinkState) override {
        this->_disp.setCursor(this->_col, this->_row);
        this->_disp.print(this->_label);
        this->_disp.setCursor(this->_col + strlen(this->_label), this->_row);
        for (int i = 0; i < _width; i++) this->_disp.write(' ');
        this->_disp.setCursor(this->_col + strlen(this->_label), this->_row);
        int whole = (int)(*_value);
        int frac = (int)((*_value - whole) * 100 + 0.5);
        char buf[8];
        snprintf(buf, sizeof(buf), "%d.%02d", whole, frac);
        int len = strlen(buf);
        if (cursorPos >= len) cursorPos = len - 1;
        if (editMode) {
            for (int i = 0; i < len; i++) {
                if (i == cursorPos && !blinkState) this->_disp.write(' ');
                else this->_disp.write(buf[i]);
            }
        } else {
            this->_disp.print(buf);
        }
    }

    void modifyDigit(int delta, int cursorPos) override {
        *_value += delta * 0.01f;
        if (*_value < 0) *_value = 0;
        if (*_value > 99.99f) *_value = 99.99f;
    }
    int getMaxDigits() override { return _width; }
    uint8_t getWidth() const override { return strlen(this->_label) + _width; }
};

#endif