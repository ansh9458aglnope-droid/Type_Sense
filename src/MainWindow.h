#pragma once
#include <QWidget>
#include <QKeyEvent>
#include "AdaptiveEngine.h"
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QPair>

class TypingEngine;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onEngineKeyPressed(QKeyEvent* event);
    void loadNextExercise();

private:
    TypingEngine* engine_;
    AdaptiveEngine* adaptiveEngine_;
    void updateTypingDisplay();
     QLabel* typingAreaLabel_;
QLabel* adaptiveInfoLabel_;

    QLabel* resultsLabel_;  // combined statistics display
    
    const Exercise* currentExercise_ = nullptr;
    int cursorIndex_ = 0;
    qint64 startTimeMs_ = 0;
    QVector<QPair<QChar,QChar>> errors_; // expected, actual (kept for compatibility)
    QVector<QChar> typedChars_; // actual characters typed up to cursorIndex_
    QVector<bool> correctnessFlags_; // true if character matched expected
    int correctCount_ = 0;
    int incorrectCount_ = 0;
    QVector<qint64> interKeyIntervals_;
    qint64 lastKeystrokeTimeMs_ = 0;

   
    QLabel* statusLabel_;
    QPushButton* nextButton_;
};