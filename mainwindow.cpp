#include "MainWindow.h"
#include "ChessBoardWidget.h"
#include "QueensSolver.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QThread>
#include <QTextEdit>
#include <QScrollBar>
#include <QFileDialog>
#include <QTextStream>
#include <QCloseEvent>

/* Конструктор главного окна
 * parent - родительский виджет */
MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), m_isSolving(false),
    m_currentSolutionsCount(0), m_currentAnimationIndex(0), m_animationRunning(false),
    m_boardWidget(nullptr), m_startButton(nullptr), m_stopButton(nullptr),
    m_nextButton(nullptr), m_prevButton(nullptr), m_showAllButton(nullptr),
    m_stopAnimationButton(nullptr), m_boardSizeSpinBox(nullptr),
    m_solutionsCountLabel(nullptr), m_progressBar(nullptr), m_consoleTextEdit(nullptr),
    m_clearConsoleButton(nullptr), m_saveConsoleButton(nullptr), m_solver(nullptr),
    m_solverThread(nullptr)
{
    setupUI();
    connectSignals();
    setWindowTitle("Задача о ферзях. Пункт 1");
    resize(1100, 800);
}

/* Деструктор */
MainWindow::~MainWindow(){
    stopSolving();
    if (m_solverThread) {
        m_solverThread->quit();
        m_solverThread->wait();
    }
}

/* Создание всех UI элементов */
void MainWindow::setupUI(){
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // ==================== ЛЕВАЯ ПАНЕЛЬ ====================
    QWidget* leftPanel = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    // Шахматная доска
    m_boardWidget = new ChessBoardWidget(this);
    leftLayout->addWidget(m_boardWidget, 1);

    // Группа "Поиск решений"
    QGroupBox* searchGroup = new QGroupBox("Поиск решений", this);
    QVBoxLayout* searchLayout = new QVBoxLayout(searchGroup);

    // Выбор размера доски
    QHBoxLayout* settingsLayout = new QHBoxLayout();
    settingsLayout->addWidget(new QLabel("Размер доски (N):", this));
    m_boardSizeSpinBox = new QSpinBox(this);
    m_boardSizeSpinBox->setRange(4, 12);  // Ограничение для предотвращения краша
    m_boardSizeSpinBox->setValue(8);
    settingsLayout->addWidget(m_boardSizeSpinBox);
    settingsLayout->addStretch();
    searchLayout->addLayout(settingsLayout);

    // Кнопки управления поиском
    QHBoxLayout* searchButtonLayout = new QHBoxLayout();
    m_startButton = new QPushButton("Начать поиск", this);
    m_stopButton = new QPushButton("Остановить поиск", this);
    m_stopButton->setEnabled(false);
    searchButtonLayout->addWidget(m_startButton);
    searchButtonLayout->addWidget(m_stopButton);
    searchLayout->addLayout(searchButtonLayout);

    // Информация о найденных решениях
    QHBoxLayout* infoLayout = new QHBoxLayout();
    m_solutionsCountLabel = new QLabel("Найдено решений: 0", this);
    infoLayout->addWidget(m_solutionsCountLabel);
    infoLayout->addStretch();
    searchLayout->addLayout(infoLayout);

    // Прогресс-бар
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    searchLayout->addWidget(m_progressBar);

    leftLayout->addWidget(searchGroup);

    // Группа "Навигация по решениям"
    QGroupBox* navigationGroup = new QGroupBox("Навигация по решениям", this);
    QVBoxLayout* navigationLayout = new QVBoxLayout(navigationGroup);

    // Кнопки навигации
    QHBoxLayout* navButtonLayout = new QHBoxLayout();
    m_prevButton = new QPushButton("Предыдущее", this);
    m_nextButton = new QPushButton("Следующее", this);
    m_prevButton->setEnabled(false);
    m_nextButton->setEnabled(false);
    navButtonLayout->addWidget(m_prevButton);
    navButtonLayout->addWidget(m_nextButton);
    navigationLayout->addLayout(navButtonLayout);

    // Кнопки анимации
    QHBoxLayout* animationLayout = new QHBoxLayout();
    m_showAllButton = new QPushButton("Показать все", this);
    m_stopAnimationButton = new QPushButton("Остановить показ", this);
    m_showAllButton->setEnabled(false);
    m_stopAnimationButton->setEnabled(false);
    animationLayout->addWidget(m_showAllButton);
    animationLayout->addWidget(m_stopAnimationButton);
    navigationLayout->addLayout(animationLayout);

    leftLayout->addWidget(navigationGroup);

    // ==================== ПРАВАЯ ПАНЕЛЬ ====================
    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    // Группа "Консоль решений"
    QGroupBox* consoleGroup = new QGroupBox("Консоль решений", this);
    QVBoxLayout* consoleLayout = new QVBoxLayout(consoleGroup);

    // Текстовое поле для консоли
    m_consoleTextEdit = new QTextEdit(this);
    m_consoleTextEdit->setReadOnly(true);
    m_consoleTextEdit->setFont(QFont("Courier New", 10));
    m_consoleTextEdit->setMinimumWidth(400);
    consoleLayout->addWidget(m_consoleTextEdit);

    // Кнопки управления консолью
    QHBoxLayout* consoleButtonLayout = new QHBoxLayout();
    m_clearConsoleButton = new QPushButton("Очистить", this);
    m_saveConsoleButton = new QPushButton("Сохранить в файл", this);
    consoleButtonLayout->addWidget(m_clearConsoleButton);
    consoleButtonLayout->addWidget(m_saveConsoleButton);
    consoleLayout->addLayout(consoleButtonLayout);

    rightLayout->addWidget(consoleGroup);

    // Добавляем панели в основной layout
    mainLayout->addWidget(leftPanel, 2);
    mainLayout->addWidget(rightPanel, 1);
}

/* Подключение сигналов к слотам */
void MainWindow::connectSignals() {
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::startSolving);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::stopSolving);
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::nextSolution);
    connect(m_prevButton, &QPushButton::clicked, this, &MainWindow::previousSolution);
    connect(m_showAllButton, &QPushButton::clicked, this, &MainWindow::showAllSolutions);
    connect(m_stopAnimationButton, &QPushButton::clicked, this, &MainWindow::stopAnimation);
    if (m_clearConsoleButton && m_saveConsoleButton) {
        connect(m_clearConsoleButton, &QPushButton::clicked, this, &MainWindow::clearConsole);
        connect(m_saveConsoleButton, &QPushButton::clicked, this, &MainWindow::saveConsoleToFile);
    }
}

/* Запуск поиска решений */
void MainWindow::startSolving() {
    if (m_isSolving) stopSolving();

    // Останавливаем анимацию, если она была
    stopAnimation();

    int boardSize = m_boardSizeSpinBox->value();
    m_boardWidget->setBoardSize(boardSize);
    m_boardWidget->clearQueens();

    // Сброс состояния
    m_currentSolutionsCount = 0;
    m_solutionsCountLabel->setText("Поиск решений...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0);  // Бесконечный прогресс
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    m_prevButton->setEnabled(false);
    m_nextButton->setEnabled(false);
    m_showAllButton->setEnabled(false);
    m_stopAnimationButton->setEnabled(false);
    m_boardSizeSpinBox->setEnabled(false);

    // Очищаем консоль
    clearConsole();
    appendToConsole("========================================");
    appendToConsole(" Количество ферзей: N = " + QString::number(boardSize) + ")");
    appendToConsole("========================================\n");
    appendToConsole("Начинаем поиск всех решений...\n");

    // Создаём поток для решения
    m_solverThread = new QThread(this);
    m_solver = new QueensSolver(boardSize);
    m_solver->moveToThread(m_solverThread);

    // Подключаем сигналы QueensSolver
    connect(m_solverThread, &QThread::started, m_solver, &QueensSolver::solve);
    connect(m_solver, &QueensSolver::solutionFound, this, &MainWindow::onSolutionFound);
    connect(m_solver, &QueensSolver::solutionPrinted, this, &MainWindow::onSolutionPrinted);
    connect(m_solver, &QueensSolver::progressUpdated, this, &MainWindow::updateProgress);
    connect(m_solver, &QueensSolver::finished, this, &MainWindow::onSolvingFinished);
    connect(m_solver, &QueensSolver::error, this, &MainWindow::onError);
    connect(m_solver, &QueensSolver::allSolutionsFound, this, &MainWindow::onAllSolutionsFound);
    connect(m_solverThread, &QThread::finished, m_solver, &QObject::deleteLater);

    m_isSolving = true;
    m_solverThread->start();
}

/* Остановка поиска решений */
void MainWindow::stopSolving(){
    if (m_solver) m_solver->stop();

    if (m_solverThread && m_solverThread->isRunning()) {
        m_solverThread->quit();
        m_solverThread->wait(1000);
    }

    m_isSolving = false;
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_boardSizeSpinBox->setEnabled(true);
    m_progressBar->setVisible(false);

    appendToConsole("\n[ПОИСК ОСТАНОВЛЕН ПОЛЬЗОВАТЕЛЕМ]\n");
}

/* Обновление прогресса поиска
 *  current - текущее количество найденных решений
 *  total - общее ожидаемое количество (0 - неизвестно) */
void MainWindow::updateProgress(int current, int total){
    m_currentSolutionsCount = current;
    if (total > 0) {
        // Известно общее количество решений
        m_progressBar->setRange(0, total);
        m_progressBar->setValue(current);
        m_solutionsCountLabel->setText(QString("Найдено решений: %1 / %2").arg(current).arg(total));
        if (current >= total) {
            m_solutionsCountLabel->setText(QString("Найдено решений: %1 / %2 - ВСЕ!").arg(current).arg(total));
        }
    } else {
        // Общее количество неизвестно
        m_progressBar->setRange(0, 0);
        m_solutionsCountLabel->setText(QString("Найдено решений: %1").arg(current));
    }
}

/* Обработка найденного решения
 *  solution - вектор позиций ферзей */
void MainWindow::onSolutionFound(const std::vector<int>& solution){
    m_currentSolutionsCount++;
    m_solutionsCountLabel->setText(QString("Найдено решений: %1").arg(m_currentSolutionsCount));
    // Показываем первое решение сразу
    if (m_currentSolutionsCount == 1) {
        m_boardWidget->setQueenPositions(solution);
        m_boardWidget->update();
        appendToConsole("\n[✓ Первое решение отображено на доске]\n");
    }
}

/* Вывод решения в консоль
 *  solutionText - текстовое представление решения */
void MainWindow::onSolutionPrinted(const QString& solutionText){
    appendToConsole(solutionText);
}

/* Обработка завершения поиска
 *  totalSolutions - общее количество найденных решений */
void MainWindow::onSolvingFinished(int totalSolutions){
    m_isSolving = false;
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_boardSizeSpinBox->setEnabled(true);
    m_progressBar->setVisible(false);

    m_solutionsCountLabel->setText(QString("Найдено решений: %1").arg(totalSolutions));

    if (totalSolutions == 0) {
        appendToConsole("\n[РЕШЕНИЙ НЕ НАЙДЕНО]\n");
    } else {
        appendToConsole("\n========================================");
        appendToConsole("  ПОИСК ЗАВЕРШЁН");
        appendToConsole("========================================");
        appendToConsole("Всего найдено решений: " + QString::number(totalSolutions));
        appendToConsole("\n");
    }
}

/* Обработка ошибки
 *  errorMessage - сообщение об ошибке */
void MainWindow::onError(const QString& errorMessage){
    QMessageBox::critical(this, "Ошибка", errorMessage);
    appendToConsole("\n[ОШИБКА] " + errorMessage + "\n");
    stopSolving();
}

/* Обработка получения всех решений
 *  solutions - вектор всех решений */
void MainWindow::onAllSolutionsFound(const std::vector<std::vector<int>>& solutions){
    m_boardWidget->setSolutions(solutions);

    // Включаем кнопки навигации
    m_nextButton->setEnabled(true);
    m_prevButton->setEnabled(true);
    m_showAllButton->setEnabled(true);

    appendToConsole(QString("\n[✓ Загружено %1 решений. Используйте кнопки навигации]\n")
                        .arg(solutions.size()));
}

/* Переход к следующему решению */
void MainWindow::nextSolution() {
    stopAnimation();
    m_boardWidget->nextSolution();
    appendToConsole(QString("→ Переход к следующему решению (%1 из %2)")
                        .arg(m_boardWidget->getCurrentSolutionIndex() + 1)
                        .arg(m_boardWidget->getAllSolutions().size()));
}

/* Переход к предыдущему решению */
void MainWindow::previousSolution() {
    stopAnimation();
    m_boardWidget->previousSolution();
    appendToConsole(QString("← Переход к предыдущему решению (%1 из %2)")
                        .arg(m_boardWidget->getCurrentSolutionIndex() + 1)
                        .arg(m_boardWidget->getAllSolutions().size()));
}

/* Запуск анимации всех решений */
void MainWindow::showAllSolutions(){
    if (m_animationRunning) stopAnimation();
    const auto& solutions = m_boardWidget->getAllSolutions();
    if (solutions.empty()) {
        appendToConsole("[!] Нет решений для отображения");
        return;
    }

    m_animationRunning = true;
    m_currentAnimationIndex = 0;
    m_showAllButton->setEnabled(false);
    m_stopAnimationButton->setEnabled(true);
    m_prevButton->setEnabled(false);
    m_nextButton->setEnabled(false);

    appendToConsole("\n[Анимация: последовательный показ всех решений]");
    animateSolution(m_currentAnimationIndex);
}

/* Остановка анимации */
void MainWindow::stopAnimation(){
    if (m_animationRunning) {
        m_animationRunning = false;
        m_stopAnimationButton->setEnabled(false);
        m_showAllButton->setEnabled(true);
        m_prevButton->setEnabled(true);
        m_nextButton->setEnabled(true);
        appendToConsole("\n[Анимация остановлена]");
    }
}

/* Анимация одного решения (рекурсивно через таймер)
 *  index - индекс решения в списке */
void MainWindow::animateSolution(int index){
    if (!m_animationRunning) return;
    const auto& solutions = m_boardWidget->getAllSolutions();
    if (index >= static_cast<int>(solutions.size())) {
        // Анимация завершена
        m_animationRunning = false;
        m_stopAnimationButton->setEnabled(false);
        m_showAllButton->setEnabled(true);
        m_prevButton->setEnabled(true);
        m_nextButton->setEnabled(true);
        appendToConsole(QString("\n[✓ Анимация завершена. Показано %1 решений]").arg(solutions.size()));
        return;
    }
    // Показываем текущее решение
    m_boardWidget->setQueenPositions(solutions[index]);
    m_boardWidget->update();
    appendToConsole(QString("  Решение %1 из %2").arg(index + 1).arg(solutions.size()));
    // Задержка 600 мс перед следующим решением
    QTimer::singleShot(600, this, [this, index]() {
        animateSolution(index + 1);
    });
}

/* Добавление текста в консоль с авто-прокруткой
 *  text - добавляемый текст */
void MainWindow::appendToConsole(const QString& text) {
    if (m_consoleTextEdit) {
        m_consoleTextEdit->append(text);
        QScrollBar* scrollBar = m_consoleTextEdit->verticalScrollBar();
        if (scrollBar) scrollBar->setValue(scrollBar->maximum());
    }
}

/* Очистка консоли */
void MainWindow::clearConsole(){
    if (m_consoleTextEdit) m_consoleTextEdit->clear();
}

/* Сохранение консоли в файл */
void MainWindow::saveConsoleToFile(){
    if (!m_consoleTextEdit) return;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить консоль решений",
                                                    QString("solutions_%1x%2.txt").arg(m_boardSizeSpinBox->value()).arg(m_boardSizeSpinBox->value()),
                                                    "Text files (*.txt);;All files (*)");

    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << m_consoleTextEdit->toPlainText();
        file.close();
        QMessageBox::information(this, "Сохранение",
                                 QString("Консоль сохранена в файл:\n%1").arg(fileName));
    } else {
        QMessageBox::warning(this, "Ошибка",
                             QString("Не удалось сохранить файл:\n%1").arg(fileName));
    }
}

/*Обработчик закрытия окна
 *  event - событие закрытия */
void MainWindow::closeEvent(QCloseEvent* event){
    stopAnimation();
    stopSolving();
    event->accept();
}