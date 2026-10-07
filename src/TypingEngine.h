#pragma once
#include <QObject>
class QKeyEvent;
#include "c_ds/correction_stack.h"
#include "database_manager.h"
#include "database_manager.h"

class TypingEngine : public QObject {
    Q_OBJECT
public:
    explicit TypingEngine(QObject *parent = nullptr);
    ~TypingEngine();

signals:
    void keyPressed(QKeyEvent* event);

public slots:
    void handleKeyPress(QKeyEvent* event);   // called from UI
private:
    CorrectionStack* corrections_;
    DatabaseManager* db_;        // manages SQLite persistence
    int currentSessionId_;       // id of the ongoing session
};
