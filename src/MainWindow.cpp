#include "MainWindow.h"
#include "TypingEngine.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), engine_(new TypingEngine(this))
{
    setFocusPolicy(Qt::StrongFocus);
    auto layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("TypeSense – Adaptive Typing Tutor", this);
    layout->addWidget(label);

    connect(engine_, &TypingEngine::keyPressed, this, &MainWindow::onEngineKeyPressed);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (engine_) {
        engine_->handleKeyPress(event);
    }
    qDebug() << "Key pressed:" << event->text();
}

void MainWindow::onEngineKeyPressed(QKeyEvent* event)
{
    qDebug() << "engine reported key:" << event->text();
}
