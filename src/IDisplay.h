#ifndef IDISPLAY_H
#define IDISPLAY_H

#include <stdint.h>

/**
 * @brief Абстрактный интерфейс для символьного LCD-дисплея.
 *
 * Любой класс, реализующий этот интерфейс, может использоваться
 * библиотекой SimpleLCDUI для отображения окон и полей.
 */
class IDisplay {
public:
  virtual ~IDisplay() {}

  /** Инициализация дисплея (задаёт размеры) */
  virtual void begin(uint8_t cols, uint8_t rows) = 0;

  /** Очистка экрана */
  virtual void clear() = 0;

  /** Установка курсора (колонка, строка) */
  virtual void setCursor(uint8_t col, uint8_t row) = 0;

  /** Вывод одиночного символа */
  virtual void write(uint8_t c) = 0;

  /** Вывод ASCII-строки */
  virtual void print(const char* s) = 0;

  /** Вывод строки с русскими символами (UTF‑8 → знакогенератор) */
  virtual void printRus(const char* s) = 0;

  /** Вывод целого числа */
  virtual void printInt(int32_t n) = 0;

  /**
   * @brief Вывод целого числа с миганием одного разряда.
   *
   * @param n         Число для отображения.
   * @param cursorPos Индекс мигающего символа (0 – левый).
   * @param show      true = показать символ, false = пробел (мигание).
   */
  virtual void printIntBlink(int32_t n, int cursorPos, bool show) = 0;

  /** Очистка заданного количества знакомест (заполнение пробелами) */
  virtual void clearField(int width) = 0;

  /** Получить текущее время в миллисекундах (например, millis()) */
  virtual unsigned long getMillis() = 0;
};

#endif