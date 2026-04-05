#ifndef QUEENSSOLVER_H
#define QUEENSSOLVER_H

#include <QObject>
#include <vector>
#include <atomic>
#include <map>
#include "QueenAgent.h"

/* Класс для решения задачи о N ферзях с использованием агентов
 *
 * Отвечает за:
 * - Создание цепочки агентов-ферзей
 * - Поиск всех допустимых решений
 * - Отправку найденных решений в главное окно через сигналы
 * - Управление потоком выполнения (асинхронный поиск) */
class QueensSolver : public QObject
{
    Q_OBJECT

public:
    explicit QueensSolver(int boardSize, QObject* parent = nullptr);
    ~QueensSolver();

public slots:
    void solve();   // Запуск поиска решений
    void stop();    // Остановка поиска

signals:
    void solutionFound(const std::vector<int>& solution);                    // Найдено решение
    void progressUpdated(int current, int total);                           // Обновление прогресса
    void finished(int totalSolutions);                                      // Поиск завершён
    void error(const QString& error);                                       // Ошибка
    void solutionPrinted(const QString& solutionText);                      // Вывод в консоль
    void allSolutionsFound(const std::vector<std::vector<int>>& solutions); // Все решения найдены

private:
    int getExpectedSolutionCount(int n) const; // Получить ожидаемое количество решений для данного N
    std::vector<int> getCurrentPositions() const; // Получить текущие позиции всех ферзей
    bool isCurrentPositionValid() const; // Проверить, является ли текущая позиция допустимой
    QString formatSolution(const std::vector<int>& solution) const; // Форматирование решения в читаемый вид

    int m_boardSize;                                    // Размер доски (N)
    std::vector<QueenAgent*> m_agents;                  // Вектор агентов-ферзей
    std::vector<std::vector<int>> m_allSolutions;       // Все найденные решения
    std::atomic<bool> m_stopRequested;                  // Флаг остановки
    std::atomic<bool> m_isSolving;                      // Флаг выполнения поиска

    // Статический словарь с известными количествами решений
    static const std::map<int, int> s_expectedSolutions;
};

#endif // QUEENSSOLVER_H