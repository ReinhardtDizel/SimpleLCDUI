/**
 * @file Window.h
 * @brief Класс окна, содержащего набор полей.
 */
#ifndef WINDOW_H
#define WINDOW_H

#include "Field.h"

/**
 * @brief Окно – группа полей, отображаемых одновременно.
 * @tparam Display Тип дисплея.
 */
template<typename Display>
class Window {
    Field<Display>** _allFields;     ///< Все поля окна
    int _allFieldCount;              ///< Общее количество полей
    Field<Display>** _editFields;    ///< Редактируемые поля (подмножество всех)
    int _editFieldCount;             ///< Количество редактируемых полей
    bool _needsFullRedraw;           ///< Флаг полной перерисовки
    Display& _disp;                  ///< Ссылка на дисплей

public:
    /**
     * @brief Конструктор.
     * @param all Массив указателей на все поля.
     * @param allCnt Размер массива all.
     * @param edit Массив указателей на редактируемые поля.
     * @param editCnt Размер массива edit.
     * @param disp Ссылка на дисплей.
     */
    Window(Field<Display>** all, int allCnt, Field<Display>** edit, int editCnt, Display& disp)
        : _allFields(all), _allFieldCount(allCnt), _editFields(edit), _editFieldCount(editCnt),
          _needsFullRedraw(true), _disp(disp) {}

    /**
     * @brief Отрисовка окна.
     * @param editField Указатель на активное поле (или nullptr).
     * @param cursorPos Позиция курсора в активном поле.
     * @param blink Состояние мигания (true – символ видим).
     */
    void draw(Field<Display>* editField = nullptr, int cursorPos = 0, bool blink = true) {
        if (_needsFullRedraw) {
            _disp.clear();
            _needsFullRedraw = false;
        }
        for (int i = 0; i < _allFieldCount; i++) {
            bool isEditing = (editField != nullptr && _allFields[i] == editField);
            _allFields[i]->draw(isEditing, cursorPos, blink);
        }
    }

    /** @return Указатель на редактируемое поле по индексу */
    Field<Display>* getEditField(int index) const {
        return (index >= 0 && index < _editFieldCount) ? _editFields[index] : nullptr;
    }

    /** @return Количество редактируемых полей */
    int getEditFieldCount() const { return _editFieldCount; }

    /** Установить флаг полной перерисовки */
    void setNeedsFullRedraw() { _needsFullRedraw = true; }
};

#endif