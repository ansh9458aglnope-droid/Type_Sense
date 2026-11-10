#include "AdaptiveEngine.h"
#include <algorithm>
#include <QRandomGenerator>
#include <QStringList>
#include <QDebug>
#include <QHash>
#include <QSet>
#include <QVector>
#include <memory>
#include <new>

// Simple sentence bank for dynamic generation
static const QStringList sentenceBank = {
    "The quick brown fox jumps over the lazy dog.",
    "Practice makes typing faster and more accurate.",
    "Learning to type well requires consistent practice.",
    "The keyboard becomes easier to use when common words become automatic.",
    "The goal is not only to type quickly but also to make fewer mistakes.",
    "Typing improves muscle memory and focus.",
    "Consistency in practice leads to steady improvement.",
    "Good posture helps prevent strain while typing.",
    "Use all fingers to maximize speed and accuracy.",
    "Breaks during long sessions help maintain concentration.",
    "Set realistic goals for each practice session.",
    "Review your errors to avoid repeating them.",
    "Keep your hands relaxed while typing.",
    "Regularly challenge yourself with new exercises.",
    "Maintain proper lighting and monitor distance.",
    "Focus on accuracy before speed.",
    "Avoid looking at the keyboard during practice.",
    "Use online resources to find additional drills.",
    "Practice typing paragraphs to build stamina.",
    "Adjust your typing position for comfort.",
    "Track progress over time with metrics.",
    "Celebrate small milestones in your learning journey.",
    "Stay motivated by setting achievable targets.",
    "Develop a consistent routine for best results.",
    "Use typing software to simulate real-world scenarios.",
    "Practice with both left and right hand emphasis.",
    "Balance practice between speed and accuracy.",
    "Try typing without looking at the screen to improve muscle memory.",
    "Focus on key placement rather than individual letters.",
    "Use typing games for a fun learning experience.",
    "Keep your workspace organized to reduce distractions.",
    "Regularly update your practice content.",
    "Avoid fatigue by taking short breaks.",
    "Set a timer to challenge yourself.",
    "Practice with varied sentence structures.",
    "Use both formal and informal language in drills.",
    "Keep learning new words to expand vocabulary.",
    "Focus on rhythm and flow while typing.",
    "Maintain eye contact with the screen for better focus.",
    "Stay hydrated during long practice sessions.",
    "Adjust keyboard height for optimal posture.",
    "Use ergonomic accessories if needed.",
    "Practice consistently to build muscle memory.",
    "Set realistic expectations based on your skill level.",
    "Track metrics like WPM and accuracy over time.",
    "Keep a log of errors to identify patterns.",
    "Use spaced repetition for learning new words.",
    "Challenge yourself with longer passages gradually.",
    "Stay patient; progress takes time.",
    "Celebrate improvements in speed and accuracy.",
    "Adjust difficulty based on performance feedback.",
    "Use the practice to build confidence.",
    "Keep your hands positioned correctly on home keys.",
    "Avoid slouching while typing.",
    "Practice with both left and right hand dominance.",
    "Focus on smooth transitions between words.",
    "Maintain consistent rhythm during typing.",
    "Use proper finger placement for each key.",
    "Practice with varied sentence lengths.",
    "Stay focused by minimizing distractions.",
    "Take regular breaks to avoid strain.",
    "Set personal goals for each session.",
    "Track progress and adjust practice accordingly.",
    "Use online typing tests for benchmarking.",
    "Practice with both formal and informal language.",
    "Keep your workspace tidy.",
    "Adjust lighting to reduce eye strain.",
    "Stay mindful of posture during practice.",
    "Maintain a consistent typing rhythm.",
    "Use finger placement guidelines for accuracy.",
    "Practice with varied sentence structures for flexibility.",
    "Focus on speed while maintaining accuracy.",
    "Adjust keyboard height and angle for comfort.",
    "Take short breaks to rest your fingers.",
    "Set achievable targets for each practice session.",
    "Use metrics like WPM to track improvement.",
    "Keep a log of mistakes for targeted practice.",
    "Practice with varied sentence lengths and structures.",
    "Stay patient and consistent in your learning journey.",
    // Added natural sentences for variety
    "The quiet river reflected the orange light of the setting sun.",
    "She packed her camera before leaving for the mountain trail.",
    "Machine learning models improve when they receive better quality data.",
    "The library was almost empty after the evening classes ended.",
    "The old train crossed the valley just before sunset.",
    "A sudden storm turned the peaceful meadow into a wet wonderland.",
    "He wrote code that could parse natural language in real time.",
    "During the lecture, the professor illustrated complex algorithms with simple diagrams.",
    "Their conversation about art lasted for hours over coffee.",
    "The aroma of freshly baked bread filled the kitchen.",
    "She smiled when she saw her childhood photo on social media.",
    "The city skyline glittered under a blanket of stars.",
    "In the laboratory, scientists measured the reaction rate at different temperatures.",
    "He tuned his guitar until every string sang in harmony.",
    "The book's plot twist left readers stunned and eager for more.",
    "A small dog chased its tail around the garden.",
    "She painted a landscape that captured the essence of autumn.",
    "They celebrated their promotion with a surprise party.",
    "The documentary explored the history of jazz music.",
    "The wind carried the scent of pine from the distant forest.",
    "He discovered an ancient manuscript in the attic.",
    "The child giggled as she played hide-and-seek behind the curtains.",
    "They navigated through the maze of streets using only their sense of direction.",
    "Her laughter echoed across the quiet room.",
    "The spaceship glided silently past the stars.",
    "He brewed a cup of coffee that tasted like sunshine.",
    "The mountain peak offered a panoramic view of the valley below.",
    "She composed a symphony inspired by the ocean's rhythm.",
    "They debated philosophy over steaming cups of tea.",
    "A sudden flash of lightning illuminated the night sky."
};

// Simple paragraph bank (10 short paragraphs)
static const QStringList paragraphBank = {
    "The quick brown fox jumps over the lazy dog. Practice makes typing faster and more accurate. Learning to type well requires consistent practice.",
    "Typing improves muscle memory and focus. Good posture helps prevent strain while typing. Use all fingers to maximize speed and accuracy.",
    "Consistency in practice leads to steady improvement. The keyboard becomes easier to use when common words become automatic. The goal is not only to type quickly but also to make fewer mistakes.",
    "Keep your hands relaxed while typing. Regularly challenge yourself with new exercises. Maintain proper lighting and monitor distance.",
    "Focus on accuracy before speed. Avoid looking at the keyboard during practice. Use online resources to find additional drills.",
    "Practice typing paragraphs to build stamina. Adjust your typing position for comfort. Track progress over time with metrics.",
    "Celebrate small milestones in your learning journey. Stay motivated by setting achievable targets. Develop a consistent routine for best results.",
    "Use typing software to simulate real-world scenarios. Practice with both left and right hand emphasis. Balance practice between speed and accuracy.",
    "Try typing without looking at the screen to improve muscle memory. Focus on key placement rather than individual letters. Use typing games for a fun learning experience.",
    "Keep your workspace organized to reduce distractions. Regularly update your practice content. Avoid fatigue by taking short breaks.",
    "The sunrise painted the sky with hues of pink and gold as the city awoke.",
    "At the market, vendors shouted over the clatter of carts, offering fresh produce and fragrant spices.",
    "During the hike, a sudden waterfall revealed crystal clear water cascading into a mossy pool.",
    "In the quiet library, an old book's pages rustled softly as someone turned them.",
    "The concert hall vibrated with applause after the orchestra finished their final piece.",
    "On the rooftop terrace, friends shared stories while watching the sunset over skyscrapers."
};


AdaptiveEngine::AdaptiveEngine()
    : m_queue(exercise_queue_create())
{
    m_bank = {
        {"Home Row Easy 1", "home-row", 1, "asdfghjkl;"},
        {"Home Row Easy 2", "home-row", 1, "qwertyuiop[]\\"},
        {"Bottom Row Medium 1", "bottom-row", 2, "zxcvbnm,./"},
        {"Mixed Text Hard 1", "mixed-text", 3, "The quick brown fox jumps over the lazy dog."},
        {"Mixed Text Hard 2", "mixed-text", 3, "Pack my box with five dozen liquor jugs."}
    };
    std::sort(m_bank.begin(), m_bank.end(), [](const Exercise& a, const Exercise& b) {
        if (a.difficulty != b.difficulty) return a.difficulty < b.difficulty;
        return a.title < b.title;
    });
}

AdaptiveEngine::~AdaptiveEngine()
{
    if (m_queue) {
        while (!exercise_queue_is_empty(m_queue))
            delete static_cast<Exercise*>(exercise_queue_dequeue(m_queue));
        exercise_queue_destroy(m_queue);
    }
}

int AdaptiveEngine::weakKeyScore(const Exercise& exercise) const
{
    int score = 0;
    for (QChar c : exercise.targetText) score += m_errorCount.value(c, 0);
    return score;
}

void AdaptiveEngine::enqueueScheduledExercises()
{
    if (!m_queue || !exercise_queue_is_empty(m_queue)) return;

    struct Candidate { Exercise exercise; int score; };
    QVector<Candidate> heap;
    auto higherPriority = [](const Candidate& a, const Candidate& b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.exercise.difficulty != b.exercise.difficulty)
            return a.exercise.difficulty < b.exercise.difficulty;
        return a.exercise.title < b.exercise.title;
    };
    auto heapPush = [&](const Candidate& value) {
        heap.append(value);
        int child = heap.size() - 1;
        while (child > 0) {
            int parent = (child - 1) / 2;
            if (!higherPriority(heap[child], heap[parent])) break;
            heap.swapItemsAt(child, parent);
            child = parent;
        }
    };
    auto heapPop = [&]() {
        Candidate result = heap.first();
        heap[0] = heap.last();
        heap.removeLast();
        int parent = 0;
        while (parent < heap.size()) {
            int left = parent * 2 + 1, right = left + 1, best = parent;
            if (left < heap.size() && higherPriority(heap[left], heap[best])) best = left;
            if (right < heap.size() && higherPriority(heap[right], heap[best])) best = right;
            if (best == parent) break;
            heap.swapItemsAt(parent, best);
            parent = best;
        }
        return result;
    };

    for (const Exercise& exercise : m_bank) {
        if (exercise.difficulty == m_currentDifficulty &&
            !generatedTargets_.contains(exercise.targetText)) {
            heapPush({exercise, weakKeyScore(exercise)});
        }
    }

    // Queue up a small batch. A max heap chooses the most relevant exercises;
    // the C FIFO queue then preserves that chosen order for the GUI.
    const int batchSize = 3;
    for (int i = 0; i < batchSize && !heap.isEmpty(); ++i) {
        Candidate candidate = heapPop();
        Exercise* copy = new (std::nothrow) Exercise(candidate.exercise);
        if (!copy) break;
        if (!exercise_queue_enqueue(m_queue, copy)) {
            delete copy;
            break;
        }
        generatedTargets_.insert(candidate.exercise.targetText);
    }
}

const Exercise* AdaptiveEngine::getNextExercise()
{
    if (!m_queue) return generateDynamicExercise();

    if (exercise_queue_is_empty(m_queue)) {
        enqueueScheduledExercises();
    }

    if (exercise_queue_is_empty(m_queue)) {
        if (m_currentDifficulty < 3) {
            ++m_currentDifficulty;
            enqueueScheduledExercises();
        }
    }

    if (exercise_queue_is_empty(m_queue)) {
        // No static exercises remain. Dynamic generator has bounded retries.
        return generateDynamicExercise();
    }

    m_currentExercise.reset(static_cast<Exercise*>(exercise_queue_dequeue(m_queue)));
    if (!m_currentExercise) return nullptr;

    const Exercise& ex = *m_currentExercise;
    m_lastWasDynamic = (ex.category == "dynamic");
    m_lastAdaptive = false;
    m_lastWeakKeysTargeted.clear();
    m_lastWeakKeyMatchesCount = 0;

    QList<QPair<QChar, int>> rankedKeys;
    for (auto it = m_errorCount.constBegin(); it != m_errorCount.constEnd(); ++it)
        if (it.value() > 0) rankedKeys.append(qMakePair(it.key(), it.value()));
    std::sort(rankedKeys.begin(), rankedKeys.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    QSet<QChar> present;
    for (QChar c : ex.targetText) {
        if (m_errorCount.value(c, 0) > 0) {
            present.insert(c);
            ++m_lastWeakKeyMatchesCount;
        }
    }
    for (const auto& entry : rankedKeys) {
        if (present.contains(entry.first)) {
            m_lastWeakKeysTargeted.append(entry.first);
            if (m_lastWeakKeysTargeted.size() == 5) break;
        }
    }
    m_lastAdaptive = !m_lastWeakKeysTargeted.isEmpty();
    return m_currentExercise.get();
}

const Exercise* AdaptiveEngine::generateDynamicExercise()
{
    QList<QChar> weakKeys;
    for (auto it = m_errorCount.constBegin(); it != m_errorCount.constEnd(); ++it)
        if (it.value() > 0) weakKeys.append(it.key());

    QVector<QString> candidates;
    for (const QString& sentence : sentenceBank) {
        bool matches = weakKeys.isEmpty();
        for (QChar c : sentence) {
            if (weakKeys.contains(c)) { matches = true; break; }
        }
        if (matches) candidates.append(sentence);
    }
    if (candidates.isEmpty())
        for (const QString& sentence : sentenceBank) candidates.append(sentence);
    if (candidates.isEmpty()) return nullptr;

    const int minSentences = (m_currentDifficulty == 1) ? 1 : (m_currentDifficulty == 2 ? 2 : 3);
    const int maxSentences = minSentences + 1;
    QString target;
    // Bounded attempts prevent unbounded recursion if combinations repeat.
    for (int attempt = 0; attempt < 30; ++attempt) {
        target.clear();
        const int count = QRandomGenerator::global()->bounded(minSentences, maxSentences + 1);
        for (int i = 0; i < count; ++i) {
            if (!target.isEmpty()) target += ' ';
            target += candidates.at(QRandomGenerator::global()->bounded(candidates.size()));
        }
        if (!generatedTargets_.contains(target)) break;
        target.clear();
    }
    if (target.isEmpty()) {
        // Deterministic fallback: select the first unused sentence.
        for (const QString& sentence : sentenceBank) {
            if (!generatedTargets_.contains(sentence)) { target = sentence; break; }
        }
    }
    if (target.isEmpty()) target = sentenceBank.first();

    Exercise exercise;
    exercise.title = QString("Dynamic %1").arg(++m_generatedCount);
    exercise.category = "dynamic";
    exercise.difficulty = m_currentDifficulty;
    exercise.targetText = target;
    m_bank.append(exercise);
    generatedTargets_.insert(target);

    // Dynamic exercises are returned as an owned stable copy.
    m_currentExercise.reset(new Exercise(exercise));
    m_lastWasDynamic = true;
    m_lastAdaptive = !weakKeys.isEmpty();
    m_lastWeakKeysTargeted.clear();
    for (auto it = m_errorCount.constBegin(); it != m_errorCount.constEnd(); ++it) {
        if (it.value() > 0 && target.contains(it.key()) && m_lastWeakKeysTargeted.size() < 5)
            m_lastWeakKeysTargeted.append(it.key());
    }
    m_lastWeakKeyMatchesCount = 0;
    for (QChar c : target) if (m_errorCount.value(c, 0) > 0) ++m_lastWeakKeyMatchesCount;
    return m_currentExercise.get();
}

void AdaptiveEngine::recordErrors(const QVector<QPair<QChar, QChar>>& errors)
{
    for (const auto& error : errors) ++m_errorCount[error.first];
}

QStringList AdaptiveEngine::recommendedLearningPath() const
{
    QHash<QString, QVector<SkillEdge>> graph;
    graph["Home Row"] = {{"Top Row", 2}, {"Bottom Row", 3}};
    graph["Top Row"] = {{"Mixed Text", 2}};
    graph["Bottom Row"] = {{"Mixed Text", 2}};
    graph["Mixed Text"] = {{"Accuracy Drills", 1}};
    graph["Accuracy Drills"] = {{"Timed Practice", 2}};
    return shortestLearningPath("Home Row", "Timed Practice", graph);
}
