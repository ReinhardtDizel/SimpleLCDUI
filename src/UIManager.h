/**
 * @file UIManager.h
 * @brief Менеджер пользовательского интерфейса – связывает окна, редактор и кнопки.
 */
#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <stdint.h>
#include "Window.h"
#include "Editor.h"

/**
 * @brief Менеджер UI: переключение окон, вход/выход из редактирования, передача команд.
 * @tparam Display Тип дисплея.
 */
template<typename Display>
class UIManager {
public:
    /**
     * @brief Конструктор.
     * @param disp Ссылка на дисплей.
     * @param windows Массив указателей на окна.
     * @param windowCount Количество окон.
     */
    UIManager(Display& disp, Window<Display>** windows, int windowCount)
        : _disp(disp), _windows(windows), _windowCount(windowCount),
          _currentWindow(0), _lastRedraw(0), _editor(disp) {}

    void begin() {
        _currentWindow = 0;
        _lastRedraw = _disp.getMillis();
        if (_windows[_currentWindow]) _windows[_currentWindow]->setNeedsFullRedraw();
    }

    /** Переключиться на следующее окно */
    void nextWindow() {
        if (_editor.isActive()) _editor.exit();
        _currentWindow = (_currentWindow + 1) % _windowCount;
        if (_windows[_currentWindow]) _windows[_currentWindow]->setNeedsFullRedraw();
    }

    /** Короткое нажатие Enter: сдвиг курсора или вход в редактор */
    void editAction() {
        Window<Display>* win = _windows[_currentWindow];
        if (!win) return;
        if (!_editor.isActive()) {
            if (win->getEditFieldCount() > 0) _editor.enter();
        } else {
            Field<Display>* f = win->getEditField(_editor.getFieldIndex());
            if (f) _editor.moveCursor(f->getMaxDigits());
        }
    }

    /** Длинное нажатие Enter: выход из редактора */
    void exitEdit() { if (_editor.isActive()) _editor.exit(); }

    /** Увеличить значение в активном поле */
    void incrementValue() {
        if (!_editor.isActive()) return;
        Window<Display>* win = _windows[_currentWindow];
        if (!win) return;
        Field<Display>* f = win->getEditField(_editor.getFieldIndex());
        if (f) f->modifyDigit(1, _editor.getCursorPos());
    }

    /** Уменьшить значение в активном поле */
    void decrementValue() {
        if (!_editor.isActive()) return;
        Window<Display>* win = _windows[_currentWindow];
        if (!win) return;
        Field<Display>* f = win->getEditField(_editor.getFieldIndex());
        if (f) f->modifyDigit(-1, _editor.getCursorPos());
    }

    /** Переключиться на следующее редактируемое поле */
    void nextEditField() {
        if (!_editor.isActive()) return;
        Window<Display>* win = _windows[_currentWindow];
        if (!win) return;
        _editor.nextField(win->getEditFieldCount());
    }

    /** Главный цикл обновления (вызывать в loop) */
    void update() {
        unsigned long now = _disp.getMillis();
        bool blinkChanged = _editor.updateBlink();
        bool needRedraw = (now - _lastRedraw >= 200) || blinkChanged;

        if (needRedraw) {
            _lastRedraw = now;
            Window<Display>* win = _windows[_currentWindow];
            if (!win) return;
            Field<Display>* editF = nullptr;
            if (_editor.isActive()) editF = win->getEditField(_editor.getFieldIndex());
            win->draw(editF, _editor.getCursorPos(), _editor.getBlinkState());
        }
    }

    bool isEditing() const { return _editor.isActive(); }
    int getCurrentWindow() const { return _currentWindow; }

private:
    Display& _disp;
    Window<Display>** _windows;
    int _windowCount;
    int _currentWindow;
    Editor<Display> _editor;
    unsigned long _lastRedraw;
};

#endif