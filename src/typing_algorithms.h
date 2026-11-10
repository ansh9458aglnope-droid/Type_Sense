#pragma once
#include <QString>
#include <QStringList>
#include <QVector>
#include <QHash>

// Tracks typing speed over a rolling window of recent keystrokes.
class SlidingWindowWpm {
public:
    explicit SlidingWindowWpm(int windowSize = 10) : windowSize_(windowSize > 1 ? windowSize : 2) {}
    void reset();
    void record(qint64 timestampMs);
    double currentWpm() const;
private:
    int windowSize_;
    QVector<qint64> timestamps_;
};

// Minimum insertions, deletions and substitutions needed to transform a into b.
int levenshteinDistance(const QString& a, const QString& b);

struct SkillEdge { QString destination; int cost; };
// Dijkstra shortest path over a directed skill graph. Empty result means unreachable.
QStringList shortestLearningPath(const QString& start, const QString& goal,
                                 const QHash<QString, QVector<SkillEdge>>& graph);
