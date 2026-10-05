#include "database_manager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

DatabaseManager::DatabaseManager(const QString& dbPath) : dbPath_(dbPath)
{
    // The actual database connection will be opened in init()
}

void DatabaseManager::init() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath_);
    if (!db.open()) {
        qFatal("Cannot open SQLite database: %s", qPrintable(db.lastError().text()));
    }

    QSqlQuery query;
    // sessions table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            start_ts DATETIME DEFAULT CURRENT_TIMESTAMP,
            end_ts DATETIME
        )
    )");

    // keystrokes table
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS keystrokes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            session_id INTEGER,
            key_text TEXT,
            timestamp INTEGER,
            correct BOOLEAN,
            FOREIGN KEY(session_id) REFERENCES sessions(id)
        )
    )");
}

int DatabaseManager::startSession() {
    QSqlQuery query;
    query.exec("INSERT INTO sessions DEFAULT VALUES");
    return static_cast<int>(query.lastInsertId().toLongLong());
}

void DatabaseManager::endSession(int sessionId) {
    QSqlQuery query;
    query.prepare("UPDATE sessions SET end_ts = CURRENT_TIMESTAMP WHERE id = ?");
    query.addBindValue(sessionId);
    query.exec();
}

void DatabaseManager::insertKeyStroke(const KeyStroke& ks) {
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO keystrokes (session_id, key_text, timestamp, correct)
        VALUES (?, ?, ?, ?)
    )");
    query.addBindValue(ks.sessionId);
    query.addBindValue(ks.keyText);
    query.addBindValue(ks.timestamp);
    query.addBindValue(ks.correct);
    query.exec();
}

QVector<KeyStroke> DatabaseManager::getStrokesForSession(int sessionId) const {
    QVector<KeyStroke> result;
    QSqlQuery query;
    query.prepare("SELECT key_text, timestamp, correct FROM keystrokes WHERE session_id = ?");
    query.addBindValue(sessionId);
    if (!query.exec()) return result;

    while (query.next()) {
        KeyStroke ks{sessionId,
                     query.value(0).toString(),
                     query.value(1).toLongLong(),
                     query.value(2).toBool()};
        result.append(ks);
    }
    return result;
}
