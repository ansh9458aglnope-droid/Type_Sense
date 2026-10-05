#pragma once
#include <QObject>
class QKeyEvent;

class TypingEngine : public QObject {
    Q_OBJECT
public:
    explicit TypingEngine(QObject *parent = nullptr);
signals:
    void keyPressed(QKeyEvent* event);

public slots:
    void handleKeyPress(QKeyEvent* event);   // called from UI
};