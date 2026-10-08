#include "AdaptiveEngine.h"
#include <algorithm>
#include <QRandomGenerator>
#include <QStringList>
#include <QDebug>

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
{
    // Build a small exercise bank
    m_bank = {
        {"Home Row Easy 1", "home-row", 1, "asdfghjkl;"},
        {"Home Row Easy 2", "home-row", 1, "qwertyuiop[]\\"},        {"Bottom Row Medium 1", "bottom-row", 2, "zxcvbnm,./"},
        {"Mixed Text Hard 1", "mixed-text", 3, "The quick brown fox jumps over the lazy dog."},
        {"Mixed Text Hard 2", "mixed-text", 3, "Pack my box with five dozen liquor jugs."}
    };

    // Ensure bank is sorted by difficulty then title for deterministic order
    std::sort(m_bank.begin(), m_bank.end(), [](const Exercise&a,const Exercise&b){
        if(a.difficulty!=b.difficulty) return a.difficulty<b.difficulty;return a.title<b.title;});
}

const Exercise* AdaptiveEngine::getNextExercise()
{
    // Find next unique exercise of current difficulty.
    size_t startIdx = m_nextIndex;
    bool wrapped = false;
    const Exercise* selected = nullptr;
    bool hasWeakCandidate = false; // whether we already found a weak‑key exercise
    for(size_t i=startIdx;; ++i){
        if(i>=m_bank.size()){
            if(wrapped) break; // all exercises examined
            i=0; wrapped=true;
        }
        const auto& ex = m_bank[i];
        if(ex.difficulty!=m_currentDifficulty) continue;
        if(generatedTargets_.contains(ex.targetText)) continue; // skip duplicates
        
        bool hasWeak=false;
        for(QChar c:ex.targetText){
            if(m_errorCount.contains(c) && m_errorCount[c]>0){hasWeak=true;break;}
        }
        if(hasWeak && !selected){
            selected = &ex;
            hasWeakCandidate = true;
        } else if(!hasWeakCandidate && !selected){
            // first non‑weak candidate
            selected = &ex;
        }
        if(selected) break; // prefer weak over non‑weak, so we can stop once a suitable one is found
    }
    if(selected){
        // Update state based on the chosen exercise
        const Exercise& ex = *selected;
        if(ex.category == "dynamic"){
            m_lastWasDynamic = true; // metadata already set by generateDynamicExercise()
            // keep existing adaptive/weak key data
        } else {
            m_lastWasDynamic = false;
            m_lastAdaptive = false;
            m_lastWeakKeysTargeted.clear();
            int matchCount=0;
            for(QChar c:ex.targetText){
                if(m_errorCount.contains(c) && m_errorCount[c]>0) matchCount++;
            }
            m_lastWeakKeyMatchesCount = matchCount;
        }
        // Mark as used and advance index
        generatedTargets_.insert(ex.targetText);
        // advance index past the chosen exercise
        size_t idxFound = 0;
        for(size_t j=0;j<m_bank.size();++j){
            if(&m_bank[j]==selected) {idxFound=j;break;}
        }
        m_nextIndex = (idxFound+1)%m_bank.size();
        return selected;
    }
    // No matching difficulty left – bump to next level
    if(m_currentDifficulty<3){
        ++m_currentDifficulty;
        m_nextIndex=0;
        return getNextExercise();
    }
    // Bank exhausted, generate a dynamic exercise
    return generateDynamicExercise();
}

const Exercise* AdaptiveEngine::generateDynamicExercise()
{
    // Gather weak keys
    QList<QChar> weakKeys;
    for(auto it=m_errorCount.constBegin(); it!=m_errorCount.constEnd(); ++it){
        if(it.value()>0)
            weakKeys.append(it.key());
    }

    QString target;
    int minSentences = 1, maxSentences = 1;
    switch(m_currentDifficulty){
        case 1: minSentences=1; maxSentences=2; break;
        case 2: minSentences=2; maxSentences=3; break;
        default: minSentences=3; maxSentences=4; break;
    }

    // Filter sentences containing weak keys
    QVector<QString> candidates;
    for(const QString &s : sentenceBank){
        bool hasWeak=false;
        for(QChar c:s){
            if(weakKeys.contains(c)){
                hasWeak=true;break;
            }
        }
        if(hasWeak || weakKeys.isEmpty()){
            candidates.append(s);
        }
    }

    if(candidates.isEmpty()){
        candidates = sentenceBank;
    }

    int count = QRandomGenerator::global()->bounded(minSentences, maxSentences+1);
    for(int i=0;i<count;++i){
        const QString &s = candidates.at(QRandomGenerator::global()->bounded(candidates.size()));
        if(!target.isEmpty()) target += " ";
        target += s;
    }

    // Avoid duplicate exercises
    if(generatedTargets_.contains(target)){
        return generateDynamicExercise();
    }

    Exercise ex;
    ex.title = QString("Dynamic %1").arg(++m_generatedCount);
    ex.category = "dynamic";
    ex.difficulty = m_currentDifficulty;
    ex.targetText = target;

        // Set debug state
        m_lastWasDynamic=true;
        m_lastAdaptive=!weakKeys.isEmpty();
        // Determine weak keys actually present in the generated target text
        QSet<QChar> used;
        for(QChar c : target){
            if(weakKeys.contains(c) && !used.contains(c)){
                used.insert(c);
            }
        }
        // Build list of weak key stats sorted by count descending
        QList<QPair<QChar,int>> wcList;
        for(auto it=m_errorCount.constBegin(); it!=m_errorCount.constEnd(); ++it){
            if(it.value()>0)
                wcList.append(qMakePair(it.key(),it.value()));
        }
        std::sort(wcList.begin(),wcList.end(),
                  [](const QPair<QChar,int>&a,const QPair<QChar,int>&b){return a.second>b.second;});
        // Determine focus keys: top N (5) weak keys that actually appear in target
        QVector<QChar> focus;
        const int maxFocus = 5;
        for(const auto &p : wcList){
            if(used.contains(p.first)){
                focus.append(p.first);
                if(focus.size() >= maxFocus) break;
            }
        }
        m_lastWeakKeysTargeted = focus;
    // Count matches in this exercise
    int matchCount=0;
    for(QChar c:target){ if(m_errorCount.contains(c) && m_errorCount[c]>0) matchCount++; }
    m_lastWeakKeyMatchesCount = matchCount;


    m_bank.append(ex);
    generatedTargets_.insert(target);
    return &m_bank.last();
}

void AdaptiveEngine::recordErrors(const QVector<QPair<QChar,QChar>>& errors)
{
    for(const auto& p:errors){
        QChar expected = p.first;
        m_errorCount[expected] += 1;
    }
}
