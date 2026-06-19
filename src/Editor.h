/**
 * @file Editor.h
 * @brief Класс редактора: управление миганием курсора, позицией и переключением полей.
 */
#ifndef EDITOR_H
#define EDITOR_H

#include <stdint.h>
#include "Field.h"

/**
 * @brief Редактор, управляющий состоянием редактирования.
 * @tparam Display Тип дисплея (используется только для getMillis()).
 */
template<typename Display>
class Editor {
public:
    static const unsigned long BLINK_INTERVAL = 500; ///< Период мигания, мс

private:
    bool _active;               ///< Активен ли режим редактирования
    int _fieldIndex;            ///< Индекс редактируемого поля в окне
    int _cursorPos;             ///< Позиция курсора (индекс символа)
    bool _blinkState;           ///< Текущее состояние мигания (true = символ видим)
    unsigned long _lastBlinkTime; ///< Время последнего переключения мигания
    Display& _disp;             ///< Ссылка на дисплей для получения времени

public:
    Editor(Display& disp)
        : _active(false), _fieldIndex(0), _cursorPos(0), _blinkState(true), _lastBlinkTime(0), _disp(disp) {}

    /** Войти в режим редактирования */
    void enter() {
        _active = true;
        _fieldIndex = 0;
        _cursorPos = 0;
        _blinkState = true;
        _lastBlinkTime = _disp.getMillis();
    }

    /** Выйти из режима редактирования */
    void exit() { _active = false; }

    /**
     * @brief Обновить мигание (вызывать в цикле).
     * @return true, если состояние мигания изменилось.
     */
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

    /** Переместить курсор вправо (циклически) */
    void moveCursor(int maxDigits) {
        if (maxDigits <= 0) return;
        _cursorPos++;
        if (_cursorPos >= maxDigits) _cursorPos = 0;
    }

    /** Переключиться на следующее поле */
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