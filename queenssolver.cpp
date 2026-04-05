#include "QueensSolver.h"
#include <QThread>
#include <QDebug>
#include <iostream>
#include <iomanip>
#include <set>
#include <QElapsedTimer>

// Инициализация статического словаря с известными количествами решений
const std::map<int, int> QueensSolver::s_expectedSolutions = {
    {1, 1}, {2, 0}, {3, 0}, {4, 2}, {5, 10}, {6, 4}, {7, 40},
    {8, 92}, {9, 352}, {10, 724}, {11, 2680}, {12, 14200},
    {13, 73712}, {14, 365596}, {15, 2279184}
};

/* Конструктор решателя
 * boardSize - размер доски
 * parent - родительский объект*/
QueensSolver::QueensSolver(int boardSize, QObject* parent): QObject(parent),
    m_boardSize(boardSize), m_stopRequested(false), m_isSolving(false) { }

/* Деструктор */
QueensSolver::~QueensSolver() {
    stop();
    for (QueenAgent* agent : m_agents) delete agent;
    m_agents.clear();
}

/* Получение ожидаемого количества решений
 *  n - размер доски */
int QueensSolver::getExpectedSolutionCount(int n) const{
    auto it = s_expectedSolutions.find(n);
    return (it != s_expectedSolutions.end()) ? it->second : 0; // количество решений (0 - неизвестно)
}

/* Запуск поиска всех решений */
void QueensSolver::solve(){
    if (m_isSolving) return;

    m_stopRequested = false;
    m_isSolving = true;
    m_allSolutions.clear();

    int expectedCount = getExpectedSolutionCount(m_boardSize);

    // Создаём агентов (цепочка ферзей)
    for (QueenAgent* agent : m_agents) delete agent;
    m_agents.clear();

    QueenAgent* previous = nullptr;
    for (int col = 0; col < m_boardSize; ++col) {
        QueenAgent* agent = new QueenAgent(col, previous, this);
        agent->setMaxRows(m_boardSize);
        m_agents.push_back(agent);
        previous = agent;
    }

    int solutionCount = 0;
    std::set<std::vector<int>> uniqueSolutions;  // Для предотвращения дубликатов
    QElapsedTimer timer;
    timer.start();
    const int MAX_TIME_MS = 60000;  // Таймаут 60 секунд для больших досок

    QueenAgent* lastAgent = m_agents.back();

    // Основной цикл поиска
    while (!m_stopRequested) {
        // Находим решение для текущей конфигурации
        if (!lastAgent->findSolution()) break;

        // Получаем текущие позиции всех ферзей
        std::vector<int> solution;
        for (QueenAgent* agent : m_agents)
            solution.push_back(agent->getRow() - 1);  // Преобразуем в 0-индексацию

        // Проверяем валидность решения
        bool valid = true;
        for (size_t i = 0; i < m_agents.size() && valid; ++i) {
            for (size_t j = i + 1; j < m_agents.size(); ++j) {
                int row1 = m_agents[i]->getRow();
                int row2 = m_agents[j]->getRow();
                int col1 = m_agents[i]->getColumn();
                int col2 = m_agents[j]->getColumn();
                if (row1 == row2 || abs(row1 - row2) == abs(col1 - col2)) {
                    valid = false;
                    break;
                }
            }
        }

        // Если решение валидно и уникально - сохраняем
        if (valid && uniqueSolutions.find(solution) == uniqueSolutions.end()) {
            uniqueSolutions.insert(solution);
            solutionCount++;
            m_allSolutions.push_back(solution);

            emit solutionFound(solution);
            emit progressUpdated(solutionCount, expectedCount);

            QString solutionText = formatSolution(solution);
            emit solutionPrinted(solutionText);

            // Автоостановка при достижении известного количества
            if (expectedCount > 0 && solutionCount >= expectedCount) break;
        }

        // Переход к следующему решению
        if (!lastAgent->advance())  break;
        // Таймаут для больших досок (N >= 10)
        if (m_boardSize >= 10 && timer.elapsed() > MAX_TIME_MS) break;

        QThread::msleep(1);  // Небольшая задержка для предотвращения блокировки UI
    }
    // Отправляем все решения в UI
    if (!m_allSolutions.empty()) emit allSolutionsFound(m_allSolutions);
    m_isSolving = false;
    emit finished(m_allSolutions.size());
}

/* Остановка поиска */
void QueensSolver::stop(){
    m_stopRequested = true;
    int waitCount = 0;
    while (m_isSolving && waitCount < 100) {
        QThread::msleep(10);
        waitCount++;
    }
}

/* Получение текущих позиций ферзей */
std::vector<int> QueensSolver::getCurrentPositions() const{
    std::vector<int> positions;
    positions.reserve(m_boardSize);

    for (const QueenAgent* agent : m_agents) positions.push_back(agent->getRow() - 1);
    return positions; // вектор строк
}

/* Проверка валидности текущей позиции */
bool QueensSolver::isCurrentPositionValid() const{
    for (size_t i = 0; i < m_agents.size(); ++i) {
        for (size_t j = i + 1; j < m_agents.size(); ++j) {
            int row1 = m_agents[i]->getRow();
            int row2 = m_agents[j]->getRow();
            int col1 = m_agents[i]->getColumn();
            int col2 = m_agents[j]->getColumn();

            // Проверка на атаку
            if (row1 == row2 || abs(row1 - row2) == abs(col1 - col2))
                return false;
        }
    }
    return true; // ферзи не бьют друг друга
}

QString QueensSolver::formatSolution(const std::vector<int>& solution) const
{
    QString result;
    result = QString("Решение: [");

    for (size_t i = 0; i < solution.size(); ++i) {
        result += QString::number(solution[i] + 1);
        if (i < solution.size() - 1) result += ", ";
    }
    result += "]";

    return result;
}