#pragma once
#include <QString>
#include <QVector>
#include <QMap>
#include <QSet>

struct Exercise {
    QString title;
    QString category;   // e.g., "home-row", "top-row"
    int difficulty;     // 1=easy,2=medium,3=hard
    QString targetText;
};

class AdaptiveEngine {
public:
    AdaptiveEngine();
    ~AdaptiveEngine() = default;

    // Load the next exercise based on current progress and weak keys.
    const Exercise* getNextExercise();

    // Record errors from a finished exercise to update weak key statistics.
    void recordErrors(const QVector<QPair<QChar,QChar>>& errors);

    // Current difficulty level of last served exercise.
    int currentDifficulty() const { return m_currentDifficulty; }
    const QMap<QChar,int>& errorCounts() const { return m_errorCount; }

    // Get info about the most recently returned exercise.
    bool wasLastExerciseDynamic() const { return m_lastWasDynamic; }
    bool wasLastExerciseAdaptive() const { return m_lastAdaptive; }
    QVector<QChar> lastWeakKeysTargeted() const { return m_lastWeakKeysTargeted; }
    int lastWeakKeyMatchesCount() const { return m_lastWeakKeyMatchesCount; }

    // Generate a new dynamic exercise when bank is exhausted.
    const Exercise* generateDynamicExercise();
private:
    QVector<Exercise> m_bank;
    QMap<QChar,int> m_errorCount;   // weak key stats across session
    int m_nextIndex = 0;
    int m_generatedCount = 0;        // counter for dynamic exercises
    QSet<QString> generatedTargets_; // avoid duplicates
    int m_currentDifficulty = 1;      // start with easy
    bool m_lastWasDynamic = false;
    bool m_lastAdaptive = false;
    QVector<QChar> m_lastWeakKeysTargeted;
    int m_lastWeakKeyMatchesCount = 0;
};
