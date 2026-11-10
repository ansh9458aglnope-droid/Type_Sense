#include "TypingEngine.h"
#include <QKeyEvent>
#include <QString>
#include <QDateTime>

// Forward declaration of DatabaseManager; header already included in TypingEngine.h

TypingEngine::TypingEngine(QObject *parent)
    : QObject(parent), db_(new DatabaseManager("typing.db"))
{
    db_->init();
    currentSessionId_ = db_->startSession();

}

TypingEngine::~TypingEngine()
{
    db_->endSession(currentSessionId_);
    delete db_;

}

void TypingEngine::handleKeyPress(QKeyEvent* event)
{
    // Persist key stroke
    KeyStroke ks;
    ks.sessionId = currentSessionId_;
    ks.keyText = event->text();
    ks.timestamp = QDateTime::currentMSecsSinceEpoch();
    ks.correct = false;  // MainWindow currently owns correctness calculation; persisted value is not yet linked
    db_->insertKeyStroke(ks);

    emit keyPressed(event);
}
