#include "MainWindow.h"
#include "TypingEngine.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <algorithm>
#include <QDebug>
#include <QSet>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      engine_(new TypingEngine(this)),
      adaptiveEngine_(new AdaptiveEngine())
{
    qDebug() << "[DEBUG] MainWindow constructor started";

    setFocusPolicy(Qt::StrongFocus);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    // Dark theme palette
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor("#121212"));
    pal.setColor(QPalette::WindowText, Qt::white);
    setPalette(pal);
    setAutoFillBackground(true);

    QLabel* headerLabel =
        new QLabel("TypeSense – Adaptive Typing Tutor", this);

    QFont headerFont = headerLabel->font();
    headerFont.setPointSize(18);
    headerFont.setBold(true);
    headerLabel->setFont(headerFont);

    layout->addWidget(headerLabel);

    // Initialize display labels
    typingAreaLabel_ = new QLabel(this);
    typingAreaLabel_->setFont(QFont("Consolas", 14, QFont::Normal));
    typingAreaLabel_->setStyleSheet(
        "background-color:#1e1e1e; "
        "padding:10px; "
        "color:white; "
        "line-height:1.4em;"
    );
    typingAreaLabel_->setWordWrap(true);
    typingAreaLabel_->setTextFormat(Qt::RichText);
    typingAreaLabel_->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
    );

    resultsLabel_ = new QLabel(this);
    statusLabel_ = new QLabel(this);
    adaptiveInfoLabel_ = new QLabel(this);

    layout->addWidget(typingAreaLabel_);
    layout->addSpacing(12);
    layout->addWidget(resultsLabel_);

    // statusLabel_ is hidden; not added to layout
    layout->addWidget(adaptiveInfoLabel_);

    nextButton_ = new QPushButton("Next Exercise", this);
    nextButton_->setEnabled(false);

    // Ensure button never receives focus or default activation
    nextButton_->setFocusPolicy(Qt::NoFocus);
    nextButton_->setAutoDefault(false);
    nextButton_->setDefault(false);

    connect(
        nextButton_,
        &QPushButton::clicked,
        this,
        &MainWindow::loadNextExercise
    );

    layout->addWidget(nextButton_);

    qDebug() << "[DEBUG] UI setup complete";
    qDebug() << "[DEBUG] AdaptiveEngine created";

    connect(
        engine_,
        &TypingEngine::keyPressed,
        this,
        &MainWindow::onEngineKeyPressed
    );

    qDebug() << "[DEBUG] About to call loadNextExercise()";

    loadNextExercise();

    qDebug() << "[DEBUG] First exercise loading";

    QTimer::singleShot(0, this, [this]() {
        this->setFocus();
    });

    qDebug() << "[DEBUG] Constructor complete";
}

MainWindow::~MainWindow()
{
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
    if (!currentExercise_) {
        return;
    }

    // Guard against processing after completion
    if (cursorIndex_ >= currentExercise_->targetText.length()) {
        return;
    }

    qint64 now = QDateTime::currentMSecsSinceEpoch();

    interKeyIntervals_.append(
        now - lastKeystrokeTimeMs_
    );

    lastKeystrokeTimeMs_ = now;

    // Handle backspace separately
    if (event->key() == Qt::Key_Backspace) {

        if (cursorIndex_ > 0) {

            // Remove the last typed character and its correctness flag
            QChar removed = typedChars_.takeLast();

            bool wasCorrect =
                correctnessFlags_.takeLast();

            Q_UNUSED(removed);

            if (wasCorrect) {
                correctCount_--;
            } else {
                incorrectCount_--;
            }

            cursorIndex_--;
        }

        updateTypingDisplay();
        return;
    }

    QString text = event->text();

    if (text.isEmpty()) {
        return; // ignore non-text keys
    }

    QChar actual = text.at(0);

    // Record typed character
    typedChars_.append(actual);

    bool isCorrect =
        (
            cursorIndex_ < currentExercise_->targetText.length() &&
            actual ==
                currentExercise_->targetText.at(cursorIndex_)
        );

    correctnessFlags_.append(isCorrect);

    if (isCorrect) {

        correctCount_++;

    } else {

        incorrectCount_++;

        errors_.append(
            qMakePair(
                currentExercise_->targetText.at(cursorIndex_),
                actual
            )
        );
    }

    // Advance regardless of correctness
    cursorIndex_++;

    statusLabel_->setText(
        QString(
            "Typed %1/%2 chars. Correct: %3  Incorrect: %4"
        )
        .arg(cursorIndex_)
        .arg(currentExercise_->targetText.length())
        .arg(correctCount_)
        .arg(incorrectCount_)
    );

    updateTypingDisplay();

    if (cursorIndex_ >= currentExercise_->targetText.length()) {

        // Exercise finished
        qint64 elapsedMs =
            now - startTimeMs_;

        double accuracy = 0.0;

        int totalTyped =
            correctCount_ + incorrectCount_;

        if (totalTyped > 0) {

            accuracy =
                (double)correctCount_ /
                totalTyped *
                100.0;
        }

        double wpm = 0.0;

        if (elapsedMs > 0) {

            wpm =
                (
                    (double)
                    currentExercise_->targetText.length()
                    / 5.0
                ) /
                (elapsedMs / 60000.0);
        }

        QString result =
            QString(
                "Accuracy: %1%%    Errors: %2    WPM: %3    Time: %4s"
            )
            .arg(
                QString::number(
                    accuracy,
                    'f',
                    2
                )
            )
            .arg(incorrectCount_)
            .arg(
                QString::number(
                    wpm,
                    'f',
                    1
                )
            )
            .arg(
                elapsedMs / 1000.0,
                'g',
                2
            );

        adaptiveEngine_->recordErrors(errors_);

        // Build weak keys string
        QMap<QChar, int> errMap =
            adaptiveEngine_->errorCounts();

        QList<QPair<QChar, int>> list;

        for (
            auto it = errMap.constBegin();
            it != errMap.constEnd();
            ++it
        ) {
            list.append(
                qMakePair(
                    it.key(),
                    it.value()
                )
            );
        }

        std::sort(
            list.begin(),
            list.end(),
            [](
                const QPair<QChar, int>& a,
                const QPair<QChar, int>& b
            ) {
                return a.second > b.second;
            }
        );

        QString weakStr = "Weak keys:";

        for (
            int i = 0;
            i < list.size() && i < 5;
            i++
        ) {
            weakStr +=
                QString(" %1(%2)")
                    .arg(list[i].first)
                    .arg(list[i].second);
        }

        resultsLabel_->setText(
            result + "\n" + weakStr
        );

        nextButton_->setEnabled(true);

        updateTypingDisplay();
    }
}

void MainWindow::loadNextExercise()
{
    currentExercise_ =
        adaptiveEngine_->getNextExercise();

    if (!currentExercise_) {

        typingAreaLabel_->setText(
            "No more exercises."
        );

        statusLabel_->setText("");

        nextButton_->setEnabled(false);

        nextButton_->setFocusPolicy(
            Qt::NoFocus
        );

        return;
    }

    cursorIndex_ = 0;
    correctCount_ = 0;
    incorrectCount_ = 0;

    errors_.clear();
    interKeyIntervals_.clear();
    typedChars_.clear();
    correctnessFlags_.clear();

    updateTypingDisplay();

    startTimeMs_ =
        QDateTime::currentMSecsSinceEpoch();

    lastKeystrokeTimeMs_ =
        startTimeMs_;

    QString adaptiveInfo;

    if (adaptiveEngine_->wasLastExerciseAdaptive()) {

        adaptiveInfo =
            "Adaptive Exercise ✓\n";

        auto keys =
            adaptiveEngine_->lastWeakKeysTargeted();

        QSet<QChar> seen;
        QString keyList;

        for (QChar c : keys) {

            if (!seen.contains(c)) {

                seen.insert(c);

                keyList += c;
                keyList += ' ';
            }
        }

        keyList =
            keyList.trimmed();

        // Calculate how many times each focus key
        // appears in the current exercise
        QString frequencyList;

        for (QChar c : keys) {

            if (!seen.contains(c)) {
                continue;
            }

            int frequency =
                currentExercise_->targetText.count(c);

            frequencyList +=
                QString("%1(%2) ")
                    .arg(c)
                    .arg(frequency);
        }

        frequencyList =
            frequencyList.trimmed();

        int matches =
            adaptiveEngine_->lastWeakKeyMatchesCount();

        Q_UNUSED(matches);

        adaptiveInfo +=
            "Focus keys: " +
            keyList +
            "\n";

        adaptiveInfo +=
            "Key frequency: " +
            frequencyList +
            "\n";

    } else {

        adaptiveInfo =
            "Standard Exercise\n";

        if (
            adaptiveEngine_->
                lastWeakKeysTargeted()
                .isEmpty()
        ) {

            adaptiveInfo +=
                "No weak-key focus yet\n";
        }
    }

    // Include difficulty info
    QString diffLabel =
        (currentExercise_->difficulty == 1)
            ? "Easy"
            : (
                (currentExercise_->difficulty == 2)
                    ? "Medium"
                    : "Hard"
            );

    adaptiveInfo +=
        "Difficulty: " +
        diffLabel;

    adaptiveInfoLabel_->setText(
        adaptiveInfo
    );
}

void MainWindow::updateTypingDisplay()
{
    if (!currentExercise_) {
        return;
    }

    QString html;

    const QString target =
        currentExercise_->targetText;

    int len =
        target.length();

    for (int i = 0; i < len; ++i) {

        QChar expected =
            target.at(i);

        if (i < cursorIndex_) {

            QChar typed =
                typedChars_.at(i);

            bool correct =
                correctnessFlags_.at(i);

            QString color =
                correct
                    ? "#4caf50"
                    : "#f44336";

            html +=
                QString(
                    "<span style='color:%1;'>%2</span>"
                )
                .arg(color)
                .arg(
                    QString(typed)
                        .toHtmlEscaped()
                );

        } else if (i == cursorIndex_) {

            // Current character:
            // yellow background + black text
            html +=
                QString(
                    "<span style='"
                    "background-color:#ffeb3b;"
                    "color:#000000;"
                    "font-weight:bold;"
                    "'>%1</span>"
                )
                .arg(
                    QString(expected)
                        .toHtmlEscaped()
                );

        } else {

            html +=
                QString(
                    "<span style='color:gray;'>%1</span>"
                )
                .arg(
                    QString(expected)
                        .toHtmlEscaped()
                );
        }
    }

    typingAreaLabel_->setText(html);
}