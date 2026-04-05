#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QProgressBar>
#include <QThread>
#include <QTextEdit>
#include <QCloseEvent>
#include <QTimer>

class ChessBoardWidget;
class QueensSolver;

/* Главное окно приложения
 *
 * Отвечает за:
 * - Создание и расположение всех UI элементов
 * - Обработку действий пользователя (нажатие кнопок)
 * - Отображение прогресса поиска решений
 * - Управление анимацией решений
 * - Вывод информации в консоль
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override; // Обработчик закрытия окна

private slots:
    // Управление поиском
    void startSolving();        // Запуск поиска решений
    void stopSolving();         // Остановка поиска

    // Обновление UI
    void updateProgress(int current, int total);  // Обновление прогресс-бара

    // Обработка сигналов от решателя
    void onSolutionFound(const std::vector<int>& solution);           // Найдено решение
    void onSolutionPrinted(const QString& solutionText);              // Вывод в консоль
    void onSolvingFinished(int totalSolutions);                       // Поиск завершён
    void onError(const QString& errorMessage);                        // Ошибка
    void onAllSolutionsFound(const std::vector<std::vector<int>>& solutions);  // Все решения найдены

    // Работа с консолью
    void clearConsole();        // Очистка консоли
    void saveConsoleToFile();   // Сохранение консоли в файл

    // Навигация по решениям
    void nextSolution();        // Следующее решение
    void previousSolution();    // Предыдущее решение

    // Анимация решений
    void showAllSolutions();    // Показать все решения (анимация)
    void stopAnimation();       // Остановить анимацию
    void animateSolution(int index);  // Анимация одного решения

private:
    void setupUI();              // Создание UI элементов
    void connectSignals();       // Подключение сигналов к слотам
    void appendToConsole(const QString& text);  // Добавление текста в консоль

    // UI элементы левой панели
    ChessBoardWidget* m_boardWidget;        // Виджет шахматной доски
    QPushButton* m_startButton;             // Кнопка "Начать поиск"
    QPushButton* m_stopButton;              // Кнопка "Остановить поиск"
    QPushButton* m_nextButton;              // Кнопка "Следующее решение"
    QPushButton* m_prevButton;              // Кнопка "Предыдущее решение"
    QPushButton* m_showAllButton;           // Кнопка "Показать все решения"
    QPushButton* m_stopAnimationButton;     // Кнопка "Остановить показ"
    QSpinBox* m_boardSizeSpinBox;           // Выбор размера доски (N)
    QLabel* m_solutionsCountLabel;          // Количество найденных решений
    QProgressBar* m_progressBar;            // Индикатор прогресса поиска

    // UI элементы правой панели (консоль)
    QTextEdit* m_consoleTextEdit;           // Текстовое поле консоли
    QPushButton* m_clearConsoleButton;      // Кнопка очистки консоли
    QPushButton* m_saveConsoleButton;       // Кнопка сохранения консоли

    // Логика решения
    QueensSolver* m_solver;                 // Объект-решатель
    QThread* m_solverThread;                // Поток для асинхронного поиска

    // Состояние приложения
    bool m_isSolving;                       // Выполняется ли поиск
    int m_currentSolutionsCount;            // Текущее количество найденных решений
    int m_currentAnimationIndex;            // Индекс текущего решения при анимации
    bool m_animationRunning;                // Выполняется ли анимация
};

#endif // MAINWINDOW_H