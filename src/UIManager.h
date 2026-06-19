#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "Window.h"
#include "Editor.h"

/**
 * @brief Главный менеджер пользовательского интерфейса.
 *
 * Управляет переключением окон, входом/выходом из режима редактирования,
 * передачей команд увеличения/уменьшения активному полю.
 * Не содержит логики кнопок – ожидает вызовы методов извне.
 * Вся работа со временем осуществляется через IDisplay::getMillis().
 */
class UIManager {
public:
  /**
   * @brief Конструктор менеджера.
   *
   * @param disp        Ссылка на объект дисплея (реализующий IDisplay).
   * @param windows     Массив указателей на окна.
   * @param windowCount Количество окон в массиве.
   */
  UIManager(IDisplay& disp, Window** windows, int windowCount)
    : _disp(disp), _windows(windows), _windowCount(windowCount),
      _currentWindow(0), _lastRedraw(0), _editor(disp) {}

  /**
   * @brief Инициализация (вызвать один раз в setup()).
   *
   * Устанавливает текущее окно на первое, фиксирует время последней
   * перерисовки и запрашивает полную перерисовку первого окна.
   */
  void begin() {
    _currentWindow = 0;
    _lastRedraw = _disp.getMillis();
    if (_windows[_currentWindow]) _windows[_currentWindow]->setNeedsFullRedraw();
  }

  /**
   * @brief Переключение на следующее окно (циклически).
   *
   * Если редактор был активен, он будет выключен перед сменой окна.
   */
  void nextWindow() {
    if (_editor.isActive()) { _editor.exit(); }
    _currentWindow = (_currentWindow + 1) % _windowCount;
    if (_windows[_currentWindow]) _windows[_currentWindow]->setNeedsFullRedraw();
  }

  /**
   * @brief Действие по короткому нажатию кнопки «Enter».
   *
   * Если редактор не активен – входит в редактор (если в окне есть
   * редактируемые поля). Если редактор активен – перемещает курсор
   * на один разряд вправо (циклически).
   */
  void editAction() {
    Window* win = _windows[_currentWindow];
    if (!win) return;
    if (!_editor.isActive()) {
      if (win->getEditFieldCount() > 0) _editor.enter();
    } else {
      Field* f = win->getEditField(_editor.getFieldIndex());
      if (f) _editor.moveCursor(f->getMaxDigits());
    }
  }

  /**
   * @brief Действие по длинному нажатию кнопки «Enter».
   *
   * Выходит из режима редактирования (если он был активен).
   */
  void exitEdit() {
    if (_editor.isActive()) _editor.exit();
  }

  /**
   * @brief Увеличить значение в активном поле (или выполнить другое действие
   *        в специальном режиме, если он реализован).
   *
   * Вызывает modifyDigit(+1) для текущего редактируемого поля.
   */
  void incrementValue() {
    if (!_editor.isActive()) return;
    Window* win = _windows[_currentWindow];
    if (!win) return;
    Field* f = win->getEditField(_editor.getFieldIndex());
    if (f) f->modifyDigit(1, _editor.getCursorPos());
  }

  /**
   * @brief Уменьшить значение в активном поле.
   *
   * Вызывает modifyDigit(-1) для текущего редактируемого поля.
   */
  void decrementValue() {
    if (!_editor.isActive()) return;
    Window* win = _windows[_currentWindow];
    if (!win) return;
    Field* f = win->getEditField(_editor.getFieldIndex());
    if (f) f->modifyDigit(-1, _editor.getCursorPos());
  }

  /**
   * @brief Переключиться на следующее редактируемое поле в окне.
   *
   * Используется, когда в окне несколько редактируемых полей.
   */
  void nextEditField() {
    if (!_editor.isActive()) return;
    Window* win = _windows[_currentWindow];
    if (!win) return;
    _editor.nextField(win->getEditFieldCount());
  }

  /**
   * @brief Главный цикл обновления (вызывать в loop()).
   *
   * Обновляет состояние мигания редактора и перерисовывает текущее окно
   * при необходимости (каждые 200 мс или при изменении мигания).
   */
  void update() {
    unsigned long now = _disp.getMillis();
    bool blinkChanged = _editor.updateBlink();
    bool needRedraw = (now - _lastRedraw >= 200) || blinkChanged;

    if (needRedraw) {
      _lastRedraw = now;
      Window* win = _windows[_currentWindow];
      if (!win) return;
      Field* editF = nullptr;
      if (_editor.isActive()) editF = win->getEditField(_editor.getFieldIndex());
      win->draw(editF, _editor.getCursorPos(), _editor.getBlinkState());
    }
  }

  /**
   * @brief Возвращает true, если редактор активен.
   */
  bool isEditing() const { return _editor.isActive(); }

  /**
   * @brief Возвращает индекс текущего окна.
   */
  int getCurrentWindow() const { return _currentWindow; }

private:
  IDisplay& _disp;          ///< Ссылка на дисплей
  Window** _windows;        ///< Массив окон
  int _windowCount;         ///< Количество окон
  int _currentWindow;       ///< Индекс текущего окна
  Editor _editor;           ///< Редактор (мигание, курсор)
  unsigned long _lastRedraw;///< Время последней перерисовки
};

#endif