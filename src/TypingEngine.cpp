#include "TypingEngine.h"
#include <QKeyEvent>
#include <cstdint>
#include <QString>
#include <QDateTime>

// Forward declaration of DatabaseManager; header already included in TypingEngine.h

TypingEngine::TypingEngine(QObject *parent)
    : QObject(parent), corrections_(correction_stack_create()), db_(new DatabaseManager("typing.db"))
{
    db_->init();
    currentSessionId_ = db_->startSession();

}

TypingEngine::~TypingEngine()
{
    db_->endSession(currentSessionId_);
    correction_stack_destroy(corrections_);
    delete db_;

}

void TypingEngine::handleKeyPress(QKeyEvent* event)
{
    // If the user pressed Backspace, record it in the stack
    if (event->key() == Qt::Key_Backspace) {
        correction_stack_push(corrections_, (void*)(intptr_t)event->key());
    }

    // Persist key stroke
    KeyStroke ks;
    ks.sessionId = currentSessionId_;
    ks.keyText = event->text();
    ks.timestamp = QDateTime::currentMSecsSinceEpoch();
    ks.correct = true;   // placeholder – real correctness logic later
    db_->insertKeyStroke(ks);

    emit keyPressed(event);
}
