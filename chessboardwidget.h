#ifndef CHESSBOARDWIDGET_H
#define CHESSBOARDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <vector>

/*Виджет для отображения шахматной доски и ферзей
 * Отвечает за:
 * - Отрисовку шахматной доски с чередующимися клетками
 * - Отображение ферзей в виде стилизованных корон
 * - Навигацию по найденным решениям */
class ChessBoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChessBoardWidget(QWidget* parent = nullptr); // Конструктор
    void setBoardSize(int size); // Установить размер доски
    int getBoardSize() const { return m_boardSize; } // Получить размер доски
    void setQueenPositions(const std::vector<int>& positions); // Установить позиции ферзей
    void clearQueens(); // Очистить доску (удалить всех ферзей)
    const std::vector<std::vector<int>>& getAllSolutions() const { return m_allSolutions; } // Получить все решения
    int getCurrentSolutionIndex() const { return m_currentSolutionIndex; } // Получить индекс текущего решения

public slots:
    void nextSolution(); // Перейти к следующему решению
    void previousSolution(); // Перейти к предыдущему решению
    void setSolutions(const std::vector<std::vector<int>>& solutions); // Загрузить список решений

protected:
    void paintEvent(QPaintEvent* event) override; // Обработчик события отрисовки
    void resizeEvent(QResizeEvent* event) override; // Обработчик изменения размера виджета

private:
    void drawBoard(QPainter& painter); // Нарисовать шахматную доску
    void drawQueens(QPainter& painter); // Нарисовать ферзей
    QRect getCellRect(int col, int row) const; // Получить координаты клетки
    void drawQueenAt(QPainter& painter, int col, int row); // Нарисовать ферзя в указанной клетке

private:
    int m_boardSize;                        // Размер доски (8 для классической задачи)
    int m_cellSize;                         // Размер клетки в пикселях
    std::vector<int> m_queenPositions;      // Текущие позиции ферзей (индекс=столбец, значение=строка)
    std::vector<std::vector<int>> m_allSolutions;  // Все найденные решения
    int m_currentSolutionIndex;              // Индекс текущего отображаемого решения
};

#endif // CHESSBOARDWIDGET_H