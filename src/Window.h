#ifndef WINDOW_H
#define WINDOW_H

#include "Field.h"

/**
 * @brief Окно – набор полей, отображаемых на экране одновременно.
 *
 * Хранит указатели на все поля окна и отдельно – на редактируемые.
 * При вызове draw() может передать активному полю флаг editMode,
 * позицию курсора и состояние мигания.
 */
class Window {
private:
  Field** _allFields;     // массив указателей на все поля
  int _allFieldCount;     // количество полей всего
  Field** _editFields;    // массив указателей на редактируемые поля
  int _editFieldCount;    // количество редактируемых полей
  bool _needsFullRedraw;  // требуется ли полная очистка экрана
  IDisplay& _disp;

public:
  /**
   * @brief Конструктор окна.
   *
   * @param all       Массив указателей на все поля окна.
   * @param allCnt    Количество полей в массиве all.
   * @param edit      Массив указателей на редактируемые поля (подмножество all).
   * @param editCnt   Количество редактируемых полей.
   * @param disp      Ссылка на объект дисплея.
   */
  Window(Field** all, int allCnt, Field** edit, int editCnt, IDisplay& disp)
    : _allFields(all), _allFieldCount(allCnt),
      _editFields(edit), _editFieldCount(editCnt),
      _needsFullRedraw(true), _disp(disp) {}

  /**
   * @brief Отрисовка окна.
   *
   * Если установлен флаг полной перерисовки, сначала очищает дисплей.
   * Затем вызывает draw() для каждого поля. Полю, которое совпадает
   * с переданным editField, передаёт editMode = true и параметры курсора.
   *
   * @param editField Указатель на редактируемое поле (nullptr, если нет).
   * @param cursorPos Позиция курсора в редактируемом поле.
   * @param blink     Состояние мигания (true = символ виден, false = пробел).
   */
  void draw(Field* editField = nullptr, int cursorPos = 0, bool blink = true) {
    if (_needsFullRedraw) {
      _disp.clear();
      _needsFullRedraw = false;
    }
    for (int i = 0; i < _allFieldCount; i++) {
      bool isEditing = (editField != nullptr && _allFields[i] == editField);
      _allFields[i]->draw(isEditing, cursorPos, blink);
    }
  }

  /**
   * @brief Получить указатель на редактируемое поле по индексу.
   *
   * @param index Индекс в массиве editFields.
   * @return Указатель на поле или nullptr, если индекс некорректен.
   */
  Field* getEditField(int index) const {
    return (index >= 0 && index < _editFieldCount) ? _editFields[index] : nullptr;
  }

  /** Количество редактируемых полей */
  int getEditFieldCount() const { return _editFieldCount; }

  /** Установить флаг полной перерисовки при следующем вызове draw() */
  void setNeedsFullRedraw() { _needsFullRedraw = true; }
};

#endif