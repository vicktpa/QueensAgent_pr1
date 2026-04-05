#ifndef QUEENAGENT_H
#define QUEENAGENT_H

#include <QObject>

/*
 * Класс агента-ферзя
 *
 * Каждый ферзь представлен отдельным агентом, который управляет своим
 * положением на доске. Агенты взаимодействуют друг с другом через
 * сообщения для нахождения допустимых конфигураций.
 *
 * Принцип работы:
 * - Каждый ферзь знает только своего соседа слева
 * - Ферзь проверяет, не атакует ли его сосед
 * - Если атакует - ферзь двигается вниз
 * - Если достиг дна - просит соседа подвинуться
 */

class QueenAgent : public QObject
{
    Q_OBJECT  // Макрос Qt для поддержки сигналов/слотов

public:
    QueenAgent(int column, QueenAgent* neighbor = nullptr, QObject* parent = nullptr); // Конструктор агента-ферзя
    ~QueenAgent(); // Деструктор
    int getRow() const { return m_row; } // Получить текущую строку ферзя
    int getColumn() const { return m_column; } // Получить столбец ферзя
    QueenAgent* getNeighbor() const { return m_neighbor; } // Получить указатель на соседнего ферзя
    void setNeighbor(QueenAgent* neighbor) { m_neighbor = neighbor; } // Установить соседнего ферзя
    void setMaxRows(int maxRows) { m_maxRows = maxRows; } // Установить максимальный размер доски
    bool canAttack(int testRow, int testColumn) const; // Проверить, может ли ферзь атаковать указанную позицию
    bool findSolution(); // Найти допустимое решение для текущего ферзя и всех левых соседей
    bool advance(); // Продвинуться к следующему возможному решению
    void reset(); // Сбросить состояние агента
    bool isTerminal() const { return m_terminal; } // Проверить, находится ли агент в терминальном состоянии
    void setRow(int row) { m_row = row; } // Установить строку (для ручного управления)

signals:
    void positionChanged(int column, int row); // Сигнал об изменении позиции ферзя

private:
    int m_column;           // Столбец (фиксирован, 0-индексация)
    int m_row;              // Текущая строка (1-индексация, от 1 до m_maxRows)
    QueenAgent* m_neighbor; // Указатель на соседнего ферзя (слева)
    int m_maxRows;          // Максимальное количество строк (размер доски)
    bool m_terminal;        // Флаг терминального состояния (все решения найдены)
};

#endif // QUEENAGENT_H