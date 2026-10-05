#pragma once
#include <QString>
#include <QVector>

struct KeyStroke {
    int sessionId;
    QString keyText;
    qint64 timestamp; // epoch ms
    bool correct;
};

class DatabaseManager {
public:
    explicit DatabaseManager(const QString& dbPath);
    void init(); // create tables if not exist

    int startSession();
    void endSession(int sessionId);

    void insertKeyStroke(const KeyStroke& ks);
    QVector<KeyStroke> getStrokesForSession(int sessionId) const;
private:
    QString dbPath_;
};