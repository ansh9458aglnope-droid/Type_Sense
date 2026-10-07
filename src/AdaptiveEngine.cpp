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
    "Stay patient and consistent in your learning journey."
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
    "Keep your workspace organized to reduce distractions. Regularly update your practice content. Avoid fatigue by taking short breaks."
};


AdaptiveEngine::AdaptiveEngine()
{
    // Build a small exercise bank
    m_bank = {
        {"Home Row Easy 1", "home-row", 1, "asdfghjkl;"},
        {"Home Row Easy 2", "home-row", 1, "qwertyuiop[]\\"},
        {"Top Row Medium 1", "top-row", 2, "QWERTYUIOP{}|"},
        {"Bottom Row Medium 1", "bottom-row", 2, "zxcvbnm,./"},
        {"Mixed Text Hard 1", "mixed-text", 3, "The quick brown fox jumps over the lazy dog."},
        {"Mixed Text Hard 2", "mixed-text", 3, "Pack my box with five dozen liquor jugs."}
    };

    // Ensure bank is sorted by difficulty then title for deterministic order
    std::sort(m_bank.begin(), m_bank.end(), [](const Exercise&a,const Exercise&b){
        if(a.difficulty!=b.difficulty) return a.difficulty<b.difficulty;return a.title<b.title;});
}

const Exercise* AdaptiveEngine::getNextExercise()
{
    // Find next exercise matching current difficulty and containing weak keys if any.
    for(size_t i=m_nextIndex;i<m_bank.size();++i){
        const auto& ex = m_bank[i];
        if(ex.difficulty!=m_currentDifficulty) continue;
        // Determine weak keys present in this exercise
        bool hasWeak=false;
        int matchCount=0;
        QVector<QChar> weakKeysInEx;
        for(QChar c:ex.targetText){
            if(m_errorCount.contains(c)){
                int cnt=m_errorCount[c];
                if(cnt>0){hasWeak=true;matchCount++;weakKeysInEx.append(c);}
            }
        }
        // If we have weak keys, pick it immediately
        bool adaptive = hasWeak;
        m_lastWasDynamic=false;
        m_lastAdaptive=adaptive;
        m_lastWeakKeysTargeted.clear();
        for(QChar ck:weakKeysInEx){ if(!m_lastWeakKeysTargeted.contains(ck)) m_lastWeakKeysTargeted.append(ck);} // unique
        m_lastWeakKeyMatchesCount = matchCount;
        
        if(hasWeak || m_nextIndex==i){
            m_nextIndex = i+1;
            return &ex;
        }
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
    m_lastWeakKeysTargeted.clear();
    for(QChar ck:weakKeys){ if(!m_lastWeakKeysTargeted.contains(ck)) m_lastWeakKeysTargeted.append(ck); }
    // Count matches in this exercise
    int matchCount=0;
    for(QChar c:target){ if(m_errorCount.contains(c) && m_errorCount[c]>0) matchCount++; }
    m_lastWeakKeyMatchesCount = matchCount;

    qDebug() << "[AdaptiveEngine]" << "Weak keys:";
    QList<QPair<QChar,int>> wcList;
    for(auto it=m_errorCount.constBegin(); it!=m_errorCount.constEnd(); ++it){ if(it.value()>0) wcList.append(qMakePair(it.key(),it.value())); }
    std::sort(wcList.begin(),wcList.end(),[](const QPair<QChar,int>&a,const QPair<QChar,int>&b){return a.second>b.second;});
    QString weakStr="";
    for(const auto &p:wcList) weakStr += QString(" %1(%2)").arg(p.first).arg(p.second);
    qDebug() << weakStr;

    qDebug() << "Selected exercise:" << target;
    QString matchedWeakStr="";
    for(QChar c:target){ if(weakKeys.contains(c)) matchedWeakStr += QString(" %1").arg(c); }
    qDebug() << "Matched weak keys:" << matchedWeakStr.trimmed();
    qDebug() << "Adaptive:" << (m_lastAdaptive?"YES":"NO");

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
