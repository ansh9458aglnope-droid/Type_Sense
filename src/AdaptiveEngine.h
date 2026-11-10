#pragma once
#include <QString>
#include <QStringList>
#include <QVector>
#include <QHash>
#include <QSet>
#include <memory>
#include "c_ds/exercise_queue.h"
#include "typing_algorithms.h"

struct Exercise {
    QString title;
    QString category;
    int difficulty = 1;
    QString targetText;
};

class AdaptiveEngine {
public:
    AdaptiveEngine();
    ~AdaptiveEngine();
    AdaptiveEngine(const AdaptiveEngine&) = delete;
    AdaptiveEngine& operator=(const AdaptiveEngine&) = delete;

    const Exercise* getNextExercise();
    void recordErrors(const QVector<QPair<QChar, QChar>>& errors);
    int currentDifficulty() const { return m_currentDifficulty; }
    const QHash<QChar, int>& errorCounts() const { return m_errorCount; }
    bool wasLastExerciseDynamic() const { return m_lastWasDynamic; }
    bool wasLastExerciseAdaptive() const { return m_lastAdaptive; }
    QVector<QChar> lastWeakKeysTargeted() const { return m_lastWeakKeysTargeted; }
    int lastWeakKeyMatchesCount() const { return m_lastWeakKeyMatchesCount; }
    const Exercise* generateDynamicExercise();
    QStringList recommendedLearningPath() const;

private:
    void enqueueScheduledExercises();
    int weakKeyScore(const Exercise& exercise) const;

    QVector<Exercise> m_bank;
    QHash<QChar, int> m_errorCount; // hash table: expected key -> error frequency
    QSet<QString> generatedTargets_;
    ExerciseQueue* m_queue = nullptr;
    std::unique_ptr<Exercise> m_currentExercise;
    int m_generatedCount = 0;
    int m_currentDifficulty = 1;
    bool m_lastWasDynamic = false;
    bool m_lastAdaptive = false;
    QVector<QChar> m_lastWeakKeysTargeted;
    int m_lastWeakKeyMatchesCount = 0;
};
