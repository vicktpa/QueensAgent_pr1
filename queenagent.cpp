#include "QueenAgent.h"
#include <cmath>    // для abs()

/* Конструктор агента-ферзя
 * column - столбец (0-индексация)
 * neighbor - указатель на левого соседа
 * parent - родительский Qt-объект
 * Инициализирует ферзя на первой строке */
QueenAgent::QueenAgent(int column, QueenAgent* neighbor, QObject* parent)
    : QObject(parent)
    , m_column(column)          // Сохраняем столбец
    , m_row(1)                  // Начинаем с первой строки
    , m_neighbor(neighbor)      // Сохраняем соседа
    , m_maxRows(8)              // По умолчанию доска 8x8
    , m_terminal(false)         // Ещё не в терминальном состоянии
{
}

/* Деструктор
 * Агент не владеет соседями, они удаляются отдельно*/
QueenAgent::~QueenAgent() {}

/* Проверка возможности атаки
 * testRow - строка для проверки (1-индексация)
 * testColumn - столбец для проверки (0-индексация)
 * true - может атаковать, false - не может
 *
 * Алгоритм проверки:
 * 1. Проверяем ту же строку
 * 2. Проверяем диагональ
 * 3. Если сам не атакуем - спрашиваем соседа */
bool QueenAgent::canAttack(int testRow, int testColumn) const{
    // Проверка на ту же строку
    if (m_row == testRow)
        return true;
    // Проверка диагонали: разница строк равна разнице столбцов
    int columnDifference = testColumn - m_column;
    if ((m_row + columnDifference == testRow) ||
        (m_row - columnDifference == testRow))
        return true;
    // Спросить соседа (рекурсивно)
    if (m_neighbor != nullptr)
        return m_neighbor->canAttack(testRow, testColumn);
    return false;  // Никто не атакует
}

/* Поиск допустимого решения
 * true - решение найдено, false - нет
 * Пока кто-то из соседей атакует текущую позицию,
 * двигаемся вниз. Если не можем двигаться - возвращаем false.*/
bool QueenAgent::findSolution(){
    if (m_terminal) return false;
    // Пока сосед атакует, двигаемся
    while (m_neighbor != nullptr && m_neighbor->canAttack(m_row, m_column))
        if (!advance()) return false;
    return true;  // Нашли безопасную позицию
}

/* Продвижение к следующей позиции
 * true - успешно продвинулись, false - решения закончились
 * Логика продвижения:
 * 1. Если есть место вниз - просто спускаемся
 * 2. Если достигли дна - просим соседа подвинуться
 * 3. Если сосед подвинулся - начинаем с первой строки*/
bool QueenAgent::advance(){
    // СЛУЧАЙ 1: Можем спуститься на следующую строку
    if (m_row < m_maxRows) {
        m_row++;
        emit positionChanged(m_column, m_row);
        return findSolution();  // Проверяем новую позицию
    }
    // СЛУЧАЙ 2: Достигли дна - просим соседа подвинуться
    if (m_neighbor != nullptr) {
        if (!m_neighbor->advance()) {
            // Сосед не может двигаться - решений больше нет
            m_terminal = true;
            return false;
        }
    } else {
        // Нет соседа (самый левый ферзь) - достигли конца
        m_terminal = true;
        return false;
    }
    // СЛУЧАЙ 3: Сосед подвинулся, начинаем сначала
    m_row = 1;
    emit positionChanged(m_column, m_row);
    return findSolution();
}

/*Сброс состояния агента
 *Возвращает ферзя на первую строку и сбрасывает терминальный флаг*/
void QueenAgent::reset(){
    m_row = 1;
    m_terminal = false;
    emit positionChanged(m_column, m_row);
    // Рекурсивно сбрасываем соседа
    if (m_neighbor != nullptr) m_neighbor->reset();
}