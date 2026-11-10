#include "typing_algorithms.h"
#include <algorithm>
#include <limits>
#include <QSet>

void SlidingWindowWpm::reset() { timestamps_.clear(); }

void SlidingWindowWpm::record(qint64 timestampMs) {
    timestamps_.append(timestampMs);
    while (timestamps_.size() > windowSize_) timestamps_.removeFirst();
}

double SlidingWindowWpm::currentWpm() const {
    if (timestamps_.size() < 2) return 0.0;
    const qint64 elapsed = timestamps_.last() - timestamps_.first();
    if (elapsed <= 0) return 0.0;
    const int intervals = timestamps_.size() - 1;
    // Standard typing convention: 5 characters = 1 word.
    return (double(intervals) / 5.0) * 60000.0 / double(elapsed);
}

int levenshteinDistance(const QString& a, const QString& b) {
    QVector<int> previous(b.size() + 1), current(b.size() + 1);
    for (int j = 0; j <= b.size(); ++j) previous[j] = j;
    for (int i = 1; i <= a.size(); ++i) {
        current[0] = i;
        for (int j = 1; j <= b.size(); ++j) {
            const int substitution = previous[j - 1] + (a.at(i - 1) == b.at(j - 1) ? 0 : 1);
            const int insertion = current[j - 1] + 1;
            const int deletion = previous[j] + 1;
            current[j] = std::min({substitution, insertion, deletion});
        }
        previous.swap(current);
    }
    return previous[b.size()];
}

QStringList shortestLearningPath(const QString& start, const QString& goal,
                                 const QHash<QString, QVector<SkillEdge>>& graph) {
    if (start == goal) return {start};
    const int INF = std::numeric_limits<int>::max();
    QHash<QString, int> distance;
    QHash<QString, QString> previous;
    QSet<QString> visited;
    for (auto it = graph.constBegin(); it != graph.constEnd(); ++it) {
        distance.insert(it.key(), INF);
        for (const SkillEdge& edge : it.value()) distance.insert(edge.destination, INF);
    }
    if (!distance.contains(start) || !distance.contains(goal)) return {};
    distance[start] = 0;

    while (true) {
        QString nearest;
        int best = INF;
        for (auto it = distance.constBegin(); it != distance.constEnd(); ++it) {
            if (!visited.contains(it.key()) && it.value() < best) {
                best = it.value();
                nearest = it.key();
            }
        }
        if (nearest.isEmpty() || best == INF) break;
        if (nearest == goal) break;
        visited.insert(nearest);
        for (const SkillEdge& edge : graph.value(nearest)) {
            if (edge.cost < 0 || visited.contains(edge.destination)) continue;
            const int candidate = best + edge.cost;
            if (candidate < distance.value(edge.destination, INF)) {
                distance[edge.destination] = candidate;
                previous[edge.destination] = nearest;
            }
        }
    }
    if (distance.value(goal, INF) == INF) return {};
    QStringList path;
    QString node = goal;
    while (!node.isEmpty()) {
        path.prepend(node);
        if (node == start) break;
        node = previous.value(node);
    }
    return (!path.isEmpty() && path.first() == start) ? path : QStringList{};
}
