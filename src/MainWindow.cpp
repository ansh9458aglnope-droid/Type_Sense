#include "MainWindow.h"
#include "TypingEngine.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <algorithm>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), engine_(new TypingEngine(this)), adaptiveEngine_(new AdaptiveEngine())
{
    qDebug() << "[DEBUG] MainWindow constructor started";
    setFocusPolicy(Qt::StrongFocus);
    auto layout = new QVBoxLayout(this);
    QLabel* headerLabel = new QLabel("TypeSense – Adaptive Typing Tutor", this);
    layout->addWidget(headerLabel);

    // Initialize display labels
    typingAreaLabel_ = new QLabel(this);
    typingAreaLabel_->setFont(QFont("monospace", 12));
    typingAreaLabel_->setWordWrap(true);
    wpmLabel_ = new QLabel(this);
    accuracyLabel_ = new QLabel(this);
    errorsLabel_ = new QLabel(this);
    adaptiveInfoLabel_ = new QLabel(this);
    // statusLabel_ and debugLabel_ removed
// Removed targetLabel widget
//    layout->addWidget(adaptiveInfoLabel_);
//    layout->addWidget(statusLabel_);
//    layout->addWidget(debugLabel_);
    nextButton_ = new QPushButton("Next Exercise", this);
    nextButton_->setEnabled(false);
    // Ensure button never receives focus or default activation
    nextButton_->setFocusPolicy(Qt::NoFocus);
    nextButton_->setAutoDefault(false);
    nextButton_->setDefault(false);
    connect(nextButton_, &QPushButton::clicked, this, &MainWindow::loadNextExercise);
    layout->addWidget(nextButton_);

    qDebug() << "[DEBUG] UI setup complete";
    qDebug() << "[DEBUG] AdaptiveEngine created";

    connect(engine_, &TypingEngine::keyPressed, this, &MainWindow::onEngineKeyPressed);

    qDebug() << "[DEBUG] About to call loadNextExercise()";
    loadNextExercise(); // start first exercise
    qDebug() << "[DEBUG] First exercise loading";
    QTimer::singleShot(0, this, [this](){ this->setFocus(); });
    qDebug() << "[DEBUG] Constructor complete";
}

MainWindow::~MainWindow(){
    delete adaptiveEngine_;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (engine_) {
        engine_->handleKeyPress(event);
    }
    qDebug() << "Key pressed:" << event->text();
}

void MainWindow::onEngineKeyPressed(QKeyEvent* event)
{
    if (!currentExercise_){return;}
    // Guard against processing after completion
    if(cursorIndex_ >= currentExercise_->targetText.length()){
        return;
    }
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    interKeyIntervals_.append(now - lastKeystrokeTimeMs_);
    lastKeystrokeTimeMs_ = now;

    // Handle backspace separately
    if (event->key() == Qt::Key_Backspace){
        if(cursorIndex_>0){
            // remove the last typed character and its correctness flag
            QChar removed = typedChars_.takeLast();
            bool wasCorrect = correctnessFlags_.takeLast();
            if(wasCorrect) correctCount_--; else incorrectCount_--;
            cursorIndex_--;
        }
        updateTypingDisplay();
        return;
    }

    QString text = event->text();
    if(text.isEmpty()){
        return; // ignore non-text keys
    }
    QChar actual = text.at(0);
    // Record typed character
    typedChars_.append(actual);
    bool isCorrect = (cursorIndex_ < currentExercise_->targetText.length() && actual == currentExercise_->targetText.at(cursorIndex_));
    correctnessFlags_.append(isCorrect);
    if(isCorrect){
        correctCount_++;
    }else{
        incorrectCount_++;
        errors_.append(qMakePair(currentExercise_->targetText.at(cursorIndex_), actual));
    }
    cursorIndex_++; // advance regardless of correctness

    statusLabel_->setText(QString("Typed %1/%2 chars. Correct: %3  Incorrect: %4").arg(cursorIndex_).arg(currentExercise_->targetText.length()).arg(correctCount_).arg(incorrectCount_));
    updateTypingDisplay();

    if(cursorIndex_>=currentExercise_->targetText.length()){
        // exercise finished
        qint64 elapsedMs = now - startTimeMs_;
        double accuracy = 0.0;
        int totalTyped = correctCount_ + incorrectCount_;
        if(totalTyped>0) accuracy = (double)correctCount_/totalTyped*100.0;
        double wpm = 0.0;
        if(elapsedMs>0){
            wpm = ((double)currentExercise_->targetText.length()/5.0)/(elapsedMs/60000.0);
        }
        QString result = QString("Accuracy: %1%%, Errors: %2, Time: %3s, WPM: %4").arg(QString::number(accuracy,'f',2)).arg(incorrectCount_).arg(elapsedMs/1000.0,'g',2).arg(QString::number(wpm,'f',1));
        adaptiveEngine_->recordErrors(errors_);
        // Build weak keys string
        QMap<QChar,int> errMap = adaptiveEngine_->errorCounts();
        QList<QPair<QChar,int>> list;
        for(auto it=errMap.constBegin();it!=errMap.constEnd();++it){list.append(qMakePair(it.key(),it.value()));}
        std::sort(list.begin(),list.end(),[](const QPair<QChar,int>&a,const QPair<QChar,int>&b){return a.second>b.second;});
        QString weakStr="Weak keys:";
        for(int i=0;i<list.size() && i<5;i++){
            weakStr += QString(" %1(%2)").arg(list[i].first).arg(list[i].second);
        }
        statusLabel_->setText(result + "\n" + weakStr);
        nextButton_->setEnabled(true);
        updateTypingDisplay();
    }
}

void MainWindow::loadNextExercise(){
    currentExercise_ = adaptiveEngine_->getNextExercise();
    if(!currentExercise_) {
        typingAreaLabel_->setText("No more exercises.");
        statusLabel_->setText("");
        nextButton_->setEnabled(false);
        nextButton_->setFocusPolicy(Qt::NoFocus);
        return;
    }
    cursorIndex_=0; correctCount_=0; incorrectCount_=0; errors_.clear(); interKeyIntervals_.clear(); typedChars_.clear(); correctnessFlags_.clear();
    updateTypingDisplay();
    startTimeMs_ = QDateTime::currentMSecsSinceEpoch();
    lastKeystrokeTimeMs_=startTimeMs_;
    // Adaptive UI disabled for crash isolation
    // QString adaptiveInfo;
    // if(adaptiveEngine_->wasLastExerciseAdaptive()){
    //     adaptiveInfo = "Adaptive Exercise ✓\n";
    //     auto keys = adaptiveEngine_->lastWeakKeysTargeted();
    //     QString keyList;
    //     for(QChar c:keys){ keyList += c; keyList += ' '; }
    //     keyList = keyList.trimmed();
    //     int matches = adaptiveEngine_->lastWeakKeyMatchesCount();
    //     adaptiveInfo += "Focus keys: " + keyList + "\n";
    //     adaptiveInfo += "Weak-key matches: " + QString::number(matches) + "\n";
    // }else{
    //     adaptiveInfo = "Standard Exercise\n";
    //     if(adaptiveEngine_->lastWeakKeysTargeted().isEmpty()){
    //         adaptiveInfo += "No weak-key focus yet\n";
    //     }
    // }
    // QString diffLabel = (currentExercise_->difficulty==1) ? "Easy" : ((currentExercise_->difficulty==2) ? "Medium" : "Hard");
    // adaptiveInfo += "Difficulty: " + diffLabel;
    // adaptiveInfoLabel_->setText(adaptiveInfo);

    // // --- Temporary debug label ---
    // QString debugStr = QString("Adaptive: %1 | Focus: %2 | Matches: %3")
    //     .arg(adaptiveEngine_->wasLastExerciseAdaptive()?"YES":"NO")
    //     .arg(QStringList::fromVector(QVector<QString>() << "").join(' ')) // placeholder, will adjust
    //     .arg(adaptiveEngine_->lastWeakKeyMatchesCount());
    // // Build focus list string for debug label
    // QString keyStr;
    // for(QChar c:adaptiveEngine_->lastWeakKeysTargeted()){ keyStr += c; keyStr += ' '; }
    // keyStr = keyStr.trimmed();
    // debugStr = QString("Adaptive: %1 | Focus: %2 | Matches: %3")
    //     .arg(adaptiveEngine_->wasLastExerciseAdaptive()?"YES":"NO")
    //     .arg(keyStr)
    //     .arg(adaptiveEngine_->lastWeakKeyMatchesCount());
    // debugLabel_->setText(debugStr);

    // statusLabel_->setText(QString("Type the following exercise: %1\nDifficulty: %2").arg(currentExercise_->title).arg(diffLabel));
    // nextButton_->setEnabled(false);

}

void MainWindow::updateTypingDisplay()
{
    if (!currentExercise_) return;
    QString html;
    const QString target = currentExercise_->targetText;
    int len = target.length();
    for (int i = 0; i < len; ++i) {
        QChar expected = target.at(i);
        if (i < cursorIndex_) {
            QChar typed = typedChars_.at(i);
            bool correct = correctnessFlags_.at(i);
            QString color = correct ? "green" : "red";
            html += QString("<span style='color:%1;'>%2</span>").arg(color).arg(QString(typed).toHtmlEscaped());
        } else if (i == cursorIndex_) {
            html += QString("<span style='background-color:#ffff99;font-weight:bold;'>%1</span>").arg(QString(expected).toHtmlEscaped());
        } else {
            html += QString("<span style='color:gray;'>%1</span>").arg(QString(expected).toHtmlEscaped());
        }
    }
    typingAreaLabel_->setText(html);
}
