#include "ChessBoardWidget.h"

#include <QFont>
#include <QPen>
#include <QBrush>
#include <cmath>

/* Конструктор виджета шахматной доски
 * parent - родительский виджет
 * Инициализирует доску размером 8x8*/
ChessBoardWidget::ChessBoardWidget(QWidget* parent): QWidget(parent),
    m_boardSize(8), m_cellSize(60), m_currentSolutionIndex(0)
{
    setMinimumSize(400, 400);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/* Установка размера доски
 * size - новый размер (количество клеток)*/
void ChessBoardWidget::setBoardSize(int size){
    m_boardSize = size;
    m_queenPositions.assign(m_boardSize, -1);
    update();  // Перерисовываем виджет
}

/* Установка позиций ферзей
 * positions - вектор строк для каждого столбца (0-индексация)*/
void ChessBoardWidget::setQueenPositions(const std::vector<int>& positions){
    if (positions.size() >= static_cast<size_t>(m_boardSize)) {
        m_queenPositions = positions;
        m_queenPositions.resize(m_boardSize, -1);
    } else {
        m_queenPositions.assign(m_boardSize, -1);
        for (size_t i = 0; i < positions.size(); ++i)
            if (i < static_cast<size_t>(m_boardSize))
                m_queenPositions[i] = positions[i];
    }
    update();  // Перерисовываем виджет
}

/* Очистка доски (удаление всех ферзей) */
void ChessBoardWidget::clearQueens() {
    m_queenPositions.assign(m_boardSize, -1);
    update();
}

/* Переход к следующему решению */
void ChessBoardWidget::nextSolution() {
    if (m_allSolutions.empty()) return;
    m_currentSolutionIndex = (m_currentSolutionIndex + 1) % m_allSolutions.size();
    setQueenPositions(m_allSolutions[m_currentSolutionIndex]);
}

/* Переход к предыдущему решению */
void ChessBoardWidget::previousSolution() {
    if (m_allSolutions.empty()) return;
    m_currentSolutionIndex = (m_currentSolutionIndex - 1 + m_allSolutions.size()) % m_allSolutions.size();
    setQueenPositions(m_allSolutions[m_currentSolutionIndex]);
}

/* Загрузка всех решений
 * solutions - вектор всех решений */
void ChessBoardWidget::setSolutions(const std::vector<std::vector<int>>& solutions) {
    m_allSolutions = solutions;
    m_currentSolutionIndex = 0;
    if (!solutions.empty()) setQueenPositions(solutions[0]);
    update();
}

/* Обработчик события отрисовки
 * event - событие отрисовки */
void ChessBoardWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // Включение сглаживания
    drawBoard(painter);
    drawQueens(painter);
}

/* Обработчик изменения размера виджета
 * event - событие изменения размера */
void ChessBoardWidget::resizeEvent(QResizeEvent* event) {
    Q_UNUSED(event)
    // Вычисляем размер клетки на основе размера виджета
    int widgetSize = qMin(width(), height());
    m_cellSize = widgetSize / m_boardSize;
    update();
}

/* Рисование шахматной доски
 * painter - объект для рисования */
void ChessBoardWidget::drawBoard(QPainter& painter) {
    for (int i = 0; i < m_boardSize; ++i) {
        for (int j = 0; j < m_boardSize; ++j) {
            QRect rect = getCellRect(i, j);
            // Чередуем цвета клеток (классическая шахматная раскраска)
            if ((i + j) % 2 == 0)
                painter.fillRect(rect, QColor(240, 217, 181));  // Светлая клетка (бежевая)
            else
                painter.fillRect(rect, QColor(181, 136, 99));   // Тёмная клетка (коричневая)
            // Рисуем границу клетки
            painter.setPen(QPen(Qt::black, 1));
            painter.drawRect(rect);
        }
    }
    /*
    // Рисуем координаты (буквы для столбцов, цифры для строк)
    painter.setFont(QFont("Arial", 10));
    painter.setPen(Qt::black);
    for (int i = 0; i < m_boardSize; ++i) {
        // Буквы для столбцов (A, B, C, ...)
        QRect colRect = getCellRect(i, 0);
        painter.drawText(colRect, Qt::AlignCenter, QString(char('A' + i)));

        // Цифры для строк (1, 2, 3, ...)
        QRect rowRect = getCellRect(0, i);
        painter.drawText(rowRect, Qt::AlignCenter, QString::number(m_boardSize - i));
    }*/
}

/* Рисование ферзей
 * painter - объект для рисования */
void ChessBoardWidget::drawQueens(QPainter& painter)
{
    for (int col = 0; col < m_boardSize; ++col) {
        if (col < static_cast<int>(m_queenPositions.size()) && m_queenPositions[col] >= 0) {
            int row = m_queenPositions[col];
            if (row < m_boardSize) drawQueenAt(painter, col, row);
        }
    }
}

/* Получение координат клетки
 * col - столбец (0-индексация)
 * row - строка (0-индексация) */
QRect ChessBoardWidget::getCellRect(int col, int row) const
{
    int x = col * m_cellSize;
    int y = row * m_cellSize;
    return QRect(x, y, m_cellSize, m_cellSize);
}

/* Рисование ферзя в клетке
 * painter - объект для рисования
 * col - столбец
 * row - строка
 * highlighted - подсвечивать ли ферзя */
void ChessBoardWidget::drawQueenAt(QPainter& painter, int col, int row){
    QRect rect = getCellRect(col, row);
    int centerX = rect.center().x();
    int centerY = rect.center().y();
    int size = m_cellSize * 0.6; // Размер ферзя относительно клетки

    painter.save();
    painter.setBrush(QBrush(Qt::black));
    painter.drawEllipse(centerX - size/2, centerY - size/2, size, size);
    painter.restore();
}