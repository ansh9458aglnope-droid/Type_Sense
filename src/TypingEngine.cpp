#include "TypingEngine.h"
#include <QKeyEvent>

TypingEngine::TypingEngine(QObject *parent)
    : QObject(parent) {}

void TypingEngine::handleKeyPress(QKeyEvent* event)
{
    emit keyPressed(event);
}
