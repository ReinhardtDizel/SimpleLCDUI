/**
 * @file Window.h
 * @brief Класс окна, содержащего набор полей.
 * Добавлен метод arrangeLayout() для автоматического размещения полей по строкам.
 */
#ifndef WINDOW_H
#define WINDOW_H

#include "Field.h"

template<typename Display>
class Window {
    Field<Display>** _allFields;
    int _allFieldCount;
    Field<Display>** _editFields;
    int _editFieldCount;
    bool _needsFullRedraw;
    Display& _disp;

public:
    Window(Field<Display>** all, int allCnt, Field<Display>** edit, int editCnt, Display& disp)
        : _allFields(all), _allFieldCount(allCnt), _editFields(edit), _editFieldCount(editCnt),
          _needsFullRedraw(true), _disp(disp) {}

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

    Field<Display>* getEditField(int index) const {
        return (index >= 0 && index < _editFieldCount) ? _editFields[index] : nullptr;
    }
    int getEditFieldCount() const { return _editFieldCount; }
    void setNeedsFullRedraw() { _needsFullRedraw = true; }

    /**
     * @brief Автоматически разместить поля по строкам.
     *
     * Использует подсказки _rowHint и _isLabel для каждого поля.
     * Левое поле прижимается к колонке 0, правое — к правому краю (dispColumns-1).
     * Метки-заголовки занимают целую строку.
     *
     * @param dispColumns Количество колонок дисплея.
     * @return true, если размещение удалось, false — если поля не вмещаются.
     */
    bool arrangeLayout(uint8_t dispColumns) {
        const int MAX_ROWS = 4;
        Field<Display>* rowFields[MAX_ROWS][2] = {{nullptr}};
        uint8_t rowCount[MAX_ROWS] = {0};
        Field<Display>* labels[MAX_ROWS] = {nullptr};

        for (int i = 0; i < _allFieldCount; i++) {
            Field<Display>* f = _allFields[i];
            if (f->_isLabel) {
                uint8_t r = f->_rowHint;
                if (r >= MAX_ROWS) return false;
                labels[r] = f;
            } else {
                uint8_t r = f->_rowHint;
                if (r >= MAX_ROWS) return false;
                if (rowCount[r] >= 2) return false; // максимум 2 обычных поля в строке
                rowFields[r][rowCount[r]++] = f;
            }
        }

        // Размещаем метки
        for (int r = 0; r < MAX_ROWS; r++) {
            if (labels[r]) {
                labels[r]->_col = 0;
                labels[r]->_row = r;
                if (rowCount[r] > 0) return false; // метка и обычные поля в одной строке — конфликт
            }
        }

        // Размещаем обычные поля
        for (int r = 0; r < MAX_ROWS; r++) {
            if (rowCount[r] == 0) continue;
            if (rowCount[r] == 1) {
                rowFields[r][0]->_col = 0;
                rowFields[r][0]->_row = r;
            } else if (rowCount[r] == 2) {
                uint8_t leftW  = rowFields[r][0]->getWidth();
                uint8_t rightW = rowFields[r][1]->getWidth();
                if (leftW + rightW + 1 > dispColumns) return false; // не влезают
                rowFields[r][0]->_col = 0;
                rowFields[r][0]->_row = r;
                rowFields[r][1]->_col = dispColumns - rightW;
                rowFields[r][1]->_row = r;
            }
        }
        return true;
    }
};

#endif