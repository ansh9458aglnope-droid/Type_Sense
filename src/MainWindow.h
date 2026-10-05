#pragma once
#include <QWidget>
#include <QKeyEvent>

class TypingEngine;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onEngineKeyPressed(QKeyEvent* event);

private:
    TypingEngine* engine_;
};