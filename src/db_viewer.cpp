#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QtSql/QSqlError>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Open the database that TypeSense writes to
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("typing.db");
    if (!db.open()) {
        qWarning() << "Failed to open typing.db:" << db.lastError().text();
        return 1;
    }

    QSqlQuery query(db);

    // Print sessions table
    qDebug() << "\n--- Sessions ---";
    if (query.exec("SELECT * FROM sessions;")) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString start = query.value(1).toString();
            QString end   = query.value(2).toString();
            qDebug() << id << start << end;
        }
    } else {
        qWarning() << "Query error:" << query.lastError().text();
    }

    // Print keystrokes table (first 10 rows)
    qDebug() << "\n--- Keystrokes (first 10) ---";
    if (query.exec("SELECT * FROM keystrokes LIMIT 10;")) {
        while (query.next()) {
            int id = query.value(0).toInt();
            int sess = query.value(1).toInt();
            QString key = query.value(2).toString();
            qint64 ts = query.value(3).toLongLong();
            bool corr = query.value(4).toBool();
            qDebug() << id << sess << key << ts << corr;
        }
    } else {
        qWarning() << "Query error:" << query.lastError().text();
    }

    return 0;
}
