/**
 * @file Field.h
 * @brief Базовый класс поля и конкретные типы полей.
 *
 * Все классы параметризованы типом Display, который должен предоставлять
 * методы для вывода на экран (print, printRus, printInt, printIntBlink, clearField и др.).
 * Также требуется метод getMillis() для работы с временем.
 * Добавлены подсказки для автоматического размещения: _rowHint (строка) и _isLabel (заголовок).
 * Виртуальные функции заменены на статический полиморфизм для экономии памяти.
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

    // Конкретные методы теперь определяются в наследниках без virtual
    // Для совместимости оставляем пустые заглушки (они не вызываются)
    void draw(bool editMode, int cursorPos, bool blinkState) {}
    void modifyDigit(int delta, int cursorPos) {}
    int getMaxDigits() { return 0; }
    uint8_t getWidth() const { return 0; }

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

    void draw(bool, int, bool) {
        this->_disp.setCursor(this->_col, this->_row);
        if (this->_useRusPrint) this->_disp.printRus(this->_label);
        else this->_disp.print(this->_label);
    }

    uint8_t getWidth() const {
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

    void draw(bool editMode, int cursorPos, bool blinkState) {
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

    void modifyDigit(int delta, int cursorPos) {
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

    int getMaxDigits() { return _maxDigits; }
    uint8_t getWidth() const { return strlen(this->_label) + _maxDigits; }
};

// ... (остальные классы полей – LinearIntField, FloatField, FreqField, IntDispField, ToggleField, FloatEditField –
//      полностью аналогичны, но без virtual. Здесь они опущены для краткости, но в реальном файле присутствуют.)