#include "ExamCountdown_v1.h"
#include <QLabel>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QDesktopWidget>
#include <QDateTime>
#include <QTimer>
#include <QThread>
#include <QGraphicsEffect>
#include <QSoundEffect>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

ExamCountdown_v1::ExamCountdown_v1(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this); 
    // 窗口设置
    desktop = QApplication::desktop()->screenGeometry();
    this->setGeometry(desktop.width() * 0.358625, -desktop.height() * 0.05, desktop.width() * 0.28275, desktop.height() * 0.05);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("font-family: DIN1451, Microsoft Yahei UI;");

    borderRadius = 5;
    border = 1;
    QFont font;
    font.setPixelSize(20);
    font.setWeight(QFont::Bold);

    BlockLabelShowTimes = 0;





    NumberLabelOpacityEffect = new QGraphicsOpacityEffect(StartWindowNumberLabel);
    NumberLabelOpacityEffect->setOpacity(0.05);
    StartWindowBlockLabel1OpacityEffect = new QGraphicsOpacityEffect(StartWindowBlockLabel1);
    StartWindowBlockLabel1OpacityEffect->setOpacity(0.05);
    StartWindowBlockLabel2OpacityEffect = new QGraphicsOpacityEffect(StartWindowBlockLabel2);
    StartWindowBlockLabel2OpacityEffect->setOpacity(0.05);
    StartWindowBlockLabel3OpacityEffect = new QGraphicsOpacityEffect(StartWindowBlockLabel3);
    StartWindowBlockLabel3OpacityEffect->setOpacity(0.05);
    StartWindowBlockLabel4OpacityEffect = new QGraphicsOpacityEffect(StartWindowBlockLabel4);
    StartWindowBlockLabel4OpacityEffect->setOpacity(0.05);



    SmallWindowText = QString::fromLocal8Bit("会考倒计时：");
    StartWindowText = QString::fromLocal8Bit("距会考");
    StartWindowEnglishText = "THE EXAM IN ";


    // 获取系统时间
    currentDateTime = QDateTime::currentDateTime();
    currentDateTimeString = currentDateTime.toString("yyyy-M-d h:m:ss");
    // 将当前时间与6月30日0:0:00进行比较
    targetDateTime = QDateTime::fromString("2025-6-30 0:0:00", "yyyy-M-d h:m:ss");
    timeDifference = currentDateTime.secsTo(targetDateTime);
    // 将秒数转换为格式d天h时m分s秒
    timeDifferenceString = QString::number(timeDifference / 86400) + QString::fromLocal8Bit(" 天 ") + QString::number((timeDifference % 86400) / 3600) + QString::fromLocal8Bit(" 时 ") + QString::number((timeDifference % 3600) / 60) + QString::fromLocal8Bit(" 分 ") + QString::number(timeDifference % 60) + QString::fromLocal8Bit(" 秒");




    CountdownSoundEffect = new QSoundEffect(this);
    CountdownSoundEffect->setSource(QUrl::fromLocalFile("sounds/countdown.wav"));
    HeartbeatSoundEffect = new QSoundEffect(this);
    HeartbeatSoundEffect->setSource(QUrl::fromLocalFile("sounds/heartbeat.wav"));


    
    SmallWindowPosition = 1;




    QFile file("config.ini");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        all = in.readAll();
        line = all.split("\n");
        for (int i = 0; i < line.size(); i++) {
            list = line[i].split("=");
            if (list[0] == "SmallWindowText")
                SmallWindowText = list[1];
            else if (list[0] == "StartWindowText")
                StartWindowText = list[1];
            else if (list[0] == "StartWindowEnglishText")
                StartWindowEnglishText = list[1];
            else if (list[0] == "targetDateTime")
                targetDateTime = QDateTime::fromString(list[1], "yyyy-M-d h:m:ss");
            else if (list[0] == "SmallWindowPosition")
                SmallWindowPosition = list[1].toInt();
        }
        file.close();
    }
    else {
        QMessageBox::information(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未找到配置文件，将使用默认配置。"));
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "SmallWindowText=" << SmallWindowText << "\n";
            out << "StartWindowText=" << StartWindowText << "\n";
            out << "StartWindowEnglishText=" << StartWindowEnglishText << "\n";
            out << "targetDateTime=" << targetDateTime.toString("yyyy-M-d h:m:ss") << "\n";
            out << "SmallWindowPosition=" << SmallWindowPosition << "\n";
            file.close();
        }
    }
    



    // Widgets
    StartWindow = new QWidget;
    StartWindow->setGeometry(0, 0, desktop.width(), desktop.height());
    StartWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    StartWindow->setAttribute(Qt::WA_TranslucentBackground);
    StartWindow->show();


    

    // Labels
    SmallWindowLabel = new QLabel(this);
    SmallWindowLabel->setGeometry(0, 0, this->width(), this->height());
    borderRadius = this->height() / 2;
    border = this->height() * 0.05;
    SmallWindowLabel->setStyleSheet("background-color: rgba(255, 255, 255, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgba(255, 255, 255, 0.75); color: rgb(200, 0, 0);");
    SmallWindowLabel->setAlignment(Qt::AlignCenter);
    SmallWindowLabel->setText(QString::fromUtf8(SmallWindowText.toUtf8()) + timeDifferenceString);
    font.setPixelSize(this->height() * 0.6);
    SmallWindowLabel->setFont(font);
    SmallWindowLabel->show();

    StartWindowUnderlyingLabel = new QLabel(StartWindow);
    StartWindowUnderlyingLabel->setGeometry(0, 0, desktop.width(), desktop.height());
    StartWindowUnderlyingLabel->setStyleSheet("background-color: rgba(0, 0, 0, 0.75);");
    StartWindowUnderlyingLabel->show();
    
    StartWindowTextColorLabel = new QLabel(StartWindow);
    StartWindowTextColorLabel->setGeometry(desktop.width() * 0.125 + desktop.height() * 0.2, desktop.height() * 0.45, desktop.width() * 0.01, desktop.height() * 0.25);
    StartWindowTextColorLabel->setStyleSheet("background-color: red;");
    StartWindowTextColorLabel->hide();
    StartWindowTextLabel1 = new QLabel(StartWindow);
    StartWindowTextLabel1->setGeometry(desktop.width() * 0.125, desktop.height() * 0.25, desktop.width() * 0.6, desktop.height() * 0.2);
    StartWindowTextLabel1->setStyleSheet("font-family: zihun59hao-chuangcuhei; color: white;");
    StartWindowTextLabel1->setText(StartWindowText);
    font.setWeight(QFont::Normal);
    font.setPixelSize(desktop.height() * 0.2);
    StartWindowTextLabel1->setFont(font);
    StartWindowTextLabel1->hide();
    StartWindowTextLabel2 = new QLabel(StartWindow);
    StartWindowTextLabel2->setGeometry(desktop.width() * 0.125 + desktop.height() * 0.2, desktop.height() * 0.45, desktop.height() * 0.4, desktop.height() * 0.2);
    StartWindowTextLabel2->setStyleSheet("font-family: zihun59hao-chuangcuhei; color: white;");
    StartWindowTextLabel2->setText(QString::fromLocal8Bit("还剩"));
    StartWindowTextLabel2->setFont(font);
    StartWindowTextLabel2->hide();
    StartWindowNumberLabel = new QLabel(StartWindow);
    StartWindowNumberLabel->setGeometry(desktop.width() * 0.15 + desktop.height() * 0.6, desktop.height() * 0.275, desktop.height() * 0.5, desktop.height() * 0.375);
    StartWindowNumberLabel->setStyleSheet("font-family: DIN1451; color: red;");
    StartWindowNumberLabel->setText(QString::number(timeDifference / 86400 + 1));
    font.setPixelSize(desktop.height() * 0.5);
    font.setWeight(QFont::Bold);
    StartWindowNumberLabel->setAlignment(Qt::AlignCenter);
    StartWindowNumberLabel->setFont(font);
    StartWindowNumberLabel->setGraphicsEffect(NumberLabelOpacityEffect);
    StartWindowNumberLabel->hide();
    StartWindowTextLabel3 = new QLabel(StartWindow);
    StartWindowTextLabel3->setGeometry(desktop.width() * 0.15 + desktop.height() * 1.1, desktop.height() * 0.45, desktop.height() * 0.2, desktop.height() * 0.2);
    StartWindowTextLabel3->setStyleSheet("font-family: zihun59hao-chuangcuhei; color: white;");
    StartWindowTextLabel3->setText(QString::fromLocal8Bit("天"));
    font.setWeight(QFont::Normal);
    font.setPixelSize(desktop.height() * 0.2);
    StartWindowTextLabel3->setFont(font);
    StartWindowTextLabel3->hide();
    StartWindowTextLabelEnglish = new QLabel(StartWindow);
    StartWindowTextLabelEnglish->setGeometry(desktop.width() * 0.1375 + desktop.height() * 0.2, desktop.height() * 0.65, desktop.width() * 0.6, desktop.height() * 0.05);
    StartWindowTextLabelEnglish->setStyleSheet("font-family: DIN1451; color: white;");
    StartWindowTextLabelEnglish->setText(StartWindowEnglishText + QString::number(timeDifference / 86400 + 1) + " DAYS");
    font.setPixelSize(desktop.height() * 0.05);
    StartWindowTextLabelEnglish->setFont(font);
    StartWindowTextLabelEnglish->hide();
    StartWindowColorLabel = new QLabel(StartWindow);
    StartWindowColorLabel->setGeometry(0, 0, 0, 0);
    StartWindowColorLabel->setStyleSheet("background-color: red;");
    StartWindowColorLabel->show();

    StartWindowBlockLabel1 = new QLabel(StartWindow);
    StartWindowBlockLabel1->setGeometry(desktop.height() * 0.05, desktop.height() * 0.05, desktop.height() * 0.1, desktop.height() * 0.1);
    StartWindowBlockLabel1->setStyleSheet("background-color: red;");
    StartWindowBlockLabel1->setGraphicsEffect(StartWindowBlockLabel1OpacityEffect);
    StartWindowBlockLabel1->hide();
    StartWindowBlockLabel2 = new QLabel(StartWindow);
    StartWindowBlockLabel2->setGeometry(desktop.width() - desktop.height() * 0.15, desktop.height() * 0.05, desktop.height() * 0.1, desktop.height() * 0.1);
    StartWindowBlockLabel2->setStyleSheet("background-color: red;");
    StartWindowBlockLabel2->setGraphicsEffect(StartWindowBlockLabel2OpacityEffect);
    StartWindowBlockLabel2->hide();
    StartWindowBlockLabel3 = new QLabel(StartWindow);
    StartWindowBlockLabel3->setGeometry(desktop.height() * 0.05, desktop.height() - desktop.height() * 0.15, desktop.height() * 0.1, desktop.height() * 0.1);
    StartWindowBlockLabel3->setStyleSheet("background-color: red;");
    StartWindowBlockLabel3->setGraphicsEffect(StartWindowBlockLabel3OpacityEffect);
    StartWindowBlockLabel3->hide();
    StartWindowBlockLabel4 = new QLabel(StartWindow);
    StartWindowBlockLabel4->setGeometry(desktop.width() - desktop.height() * 0.15, desktop.height() - desktop.height() * 0.15, desktop.height() * 0.1, desktop.height() * 0.1);
    StartWindowBlockLabel4->setStyleSheet("background-color: red;");
    StartWindowBlockLabel4->setGraphicsEffect(StartWindowBlockLabel4OpacityEffect);
    StartWindowBlockLabel4->hide();




    
    
    
    
    // Animations
    SmallWindowStartAnimation = new QPropertyAnimation(this, "pos");
    SmallWindowStartAnimation->setDuration(1000);
    SmallWindowStartAnimation->setEasingCurve(QEasingCurve::OutBack);
    if (SmallWindowPosition == 0) {
        SmallWindowStartAnimation->setStartValue(QPoint(-desktop.width() * 0.28275, 0));
        SmallWindowStartAnimation->setEndValue(QPoint(0, 0));
    }
    else if (SmallWindowPosition == 1) {
        SmallWindowStartAnimation->setStartValue(QPoint(desktop.width() * 0.358625, -desktop.height() * 0.05));
        SmallWindowStartAnimation->setEndValue(QPoint(desktop.width() * 0.358625, 0));
    }
    else if (SmallWindowPosition == 2) {
        SmallWindowStartAnimation->setStartValue(QPoint(desktop.width(), 0));
        SmallWindowStartAnimation->setEndValue(QPoint(desktop.width() * 0.71725, 0));
    }
    SmallWindowStartOpacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    SmallWindowStartOpacityAnimation->setDuration(1000);
    SmallWindowStartOpacityAnimation->setStartValue(0);
    SmallWindowStartOpacityAnimation->setEndValue(1);
    SmallWindowStartOpacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    SmallWindowCloseOpacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    SmallWindowCloseOpacityAnimation->setDuration(1000);
    SmallWindowCloseOpacityAnimation->setStartValue(1);
    SmallWindowCloseOpacityAnimation->setEndValue(0);
    SmallWindowCloseOpacityAnimation->setEasingCurve(QEasingCurve::OutCubic);

    StartWindowStartOpacityAnimation = new QPropertyAnimation(StartWindow, "windowOpacity");
    StartWindowStartOpacityAnimation->setDuration(1000);
    StartWindowStartOpacityAnimation->setStartValue(0);
    StartWindowStartOpacityAnimation->setEndValue(1);
    StartWindowStartOpacityAnimation->setEasingCurve(QEasingCurve::InCubic);
    StartWindowColorLabelAnimation1 = new QPropertyAnimation(StartWindowColorLabel, "geometry");
    StartWindowColorLabelAnimation1->setDuration(1000);
    StartWindowColorLabelAnimation1->setStartValue(QRect(0, 0, 0, desktop.height()));
    StartWindowColorLabelAnimation1->setEndValue(QRect(0, 0, desktop.width(), desktop.height()));
    StartWindowColorLabelAnimation1->setEasingCurve(QEasingCurve::InCubic);
    StartWindowColorLabelAnimation2 = new QPropertyAnimation(StartWindowColorLabel, "geometry");
    StartWindowColorLabelAnimation2->setDuration(1000);
    StartWindowColorLabelAnimation2->setStartValue(QRect(0, 0, desktop.width(), desktop.height()));
    StartWindowColorLabelAnimation2->setEndValue(QRect(desktop.width(), 0, 0, desktop.height()));
    StartWindowColorLabelAnimation2->setEasingCurve(QEasingCurve::OutCubic);
    StartWindowTextColorLabelAnimation = new QPropertyAnimation(StartWindowTextColorLabel, "geometry");
    StartWindowTextColorLabelAnimation->setDuration(1500);
    StartWindowTextColorLabelAnimation->setStartValue(QRect(desktop.width() * 0.1 + desktop.height() * 0.2, desktop.height() * 0.575, desktop.width() * 0.0125, 0));
    StartWindowTextColorLabelAnimation->setEndValue(QRect(desktop.width() * 0.1 + desktop.height() * 0.2, desktop.height() * 0.45, desktop.width() * 0.0125, desktop.height() * 0.25));
    StartWindowTextColorLabelAnimation->setEasingCurve(QEasingCurve::OutCubic);
    StartWindowCloseOpacityAnimation = new QPropertyAnimation(StartWindow, "windowOpacity");
    StartWindowCloseOpacityAnimation->setDuration(3000);
    StartWindowCloseOpacityAnimation->setStartValue(1);
    StartWindowCloseOpacityAnimation->setEndValue(0);
    StartWindowCloseOpacityAnimation->setEasingCurve(QEasingCurve::InCubic);
    StartWindowNumberLabelOpacityAnimation1 = new QPropertyAnimation(NumberLabelOpacityEffect, "opacity");
    StartWindowNumberLabelOpacityAnimation1->setDuration(50);
    StartWindowNumberLabelOpacityAnimation1->setStartValue(0);
    StartWindowNumberLabelOpacityAnimation1->setEndValue(0.75);
    StartWindowNumberLabelOpacityAnimation2 = new QPropertyAnimation(NumberLabelOpacityEffect, "opacity");
    StartWindowNumberLabelOpacityAnimation2->setDuration(50);
    StartWindowNumberLabelOpacityAnimation2->setStartValue(0.75);
    StartWindowNumberLabelOpacityAnimation2->setEndValue(0.075);
    StartWindowNumberLabelOpacityAnimation3 = new QPropertyAnimation(NumberLabelOpacityEffect, "opacity");
    StartWindowNumberLabelOpacityAnimation3->setDuration(50);
    StartWindowNumberLabelOpacityAnimation3->setStartValue(0.075);
    StartWindowNumberLabelOpacityAnimation3->setEndValue(0.875);
    StartWindowNumberLabelOpacityAnimation4 = new QPropertyAnimation(NumberLabelOpacityEffect, "opacity");
    StartWindowNumberLabelOpacityAnimation4->setDuration(50);
    StartWindowNumberLabelOpacityAnimation4->setStartValue(0.875);
    StartWindowNumberLabelOpacityAnimation4->setEndValue(0.125);
    StartWindowNumberLabelOpacityAnimation5 = new QPropertyAnimation(NumberLabelOpacityEffect, "opacity");
    StartWindowNumberLabelOpacityAnimation5->setDuration(500);
    StartWindowNumberLabelOpacityAnimation5->setStartValue(0.5);
    StartWindowNumberLabelOpacityAnimation5->setEndValue(1);

    StartWindowBlockLabel1OpacityAnimation1 = new QPropertyAnimation(StartWindowBlockLabel1OpacityEffect, "opacity");
    StartWindowBlockLabel1OpacityAnimation1->setDuration(450);
    StartWindowBlockLabel1OpacityAnimation1->setStartValue(0.05);
    StartWindowBlockLabel1OpacityAnimation1->setEndValue(0.05);
    StartWindowBlockLabel1OpacityAnimation2 = new QPropertyAnimation(StartWindowBlockLabel1OpacityEffect, "opacity");
    StartWindowBlockLabel1OpacityAnimation2->setDuration(50);
    StartWindowBlockLabel1OpacityAnimation2->setStartValue(0.05);
    StartWindowBlockLabel1OpacityAnimation2->setEndValue(1);
    StartWindowBlockLabel1OpacityAnimation3 = new QPropertyAnimation(StartWindowBlockLabel1OpacityEffect, "opacity");
    StartWindowBlockLabel1OpacityAnimation3->setDuration(450);
    StartWindowBlockLabel1OpacityAnimation3->setStartValue(1);
    StartWindowBlockLabel1OpacityAnimation3->setEndValue(1);
    StartWindowBlockLabel1OpacityAnimation4 = new QPropertyAnimation(StartWindowBlockLabel1OpacityEffect, "opacity");
    StartWindowBlockLabel1OpacityAnimation4->setDuration(50);
    StartWindowBlockLabel1OpacityAnimation4->setStartValue(1);
    StartWindowBlockLabel1OpacityAnimation4->setEndValue(0.05);
    StartWindowBlockLabel2OpacityAnimation1 = new QPropertyAnimation(StartWindowBlockLabel2OpacityEffect, "opacity");
    StartWindowBlockLabel2OpacityAnimation1->setDuration(450);
    StartWindowBlockLabel2OpacityAnimation1->setStartValue(0.05);
    StartWindowBlockLabel2OpacityAnimation1->setEndValue(0.05);
    StartWindowBlockLabel2OpacityAnimation2 = new QPropertyAnimation(StartWindowBlockLabel2OpacityEffect, "opacity");
    StartWindowBlockLabel2OpacityAnimation2->setDuration(50);
    StartWindowBlockLabel2OpacityAnimation2->setStartValue(0.05);
    StartWindowBlockLabel2OpacityAnimation2->setEndValue(1);
    StartWindowBlockLabel2OpacityAnimation3 = new QPropertyAnimation(StartWindowBlockLabel2OpacityEffect, "opacity");
    StartWindowBlockLabel2OpacityAnimation3->setDuration(450);
    StartWindowBlockLabel2OpacityAnimation3->setStartValue(1);
    StartWindowBlockLabel2OpacityAnimation3->setEndValue(1);
    StartWindowBlockLabel2OpacityAnimation4 = new QPropertyAnimation(StartWindowBlockLabel2OpacityEffect, "opacity");
    StartWindowBlockLabel2OpacityAnimation4->setDuration(50);
    StartWindowBlockLabel2OpacityAnimation4->setStartValue(1);
    StartWindowBlockLabel2OpacityAnimation4->setEndValue(0.05);
    StartWindowBlockLabel3OpacityAnimation1 = new QPropertyAnimation(StartWindowBlockLabel3OpacityEffect, "opacity");
    StartWindowBlockLabel3OpacityAnimation1->setDuration(450);
    StartWindowBlockLabel3OpacityAnimation1->setStartValue(0.05);
    StartWindowBlockLabel3OpacityAnimation1->setEndValue(0.05);
    StartWindowBlockLabel3OpacityAnimation2 = new QPropertyAnimation(StartWindowBlockLabel3OpacityEffect, "opacity");
    StartWindowBlockLabel3OpacityAnimation2->setDuration(50);
    StartWindowBlockLabel3OpacityAnimation2->setStartValue(0.05);
    StartWindowBlockLabel3OpacityAnimation2->setEndValue(1);
    StartWindowBlockLabel3OpacityAnimation3 = new QPropertyAnimation(StartWindowBlockLabel3OpacityEffect, "opacity");
    StartWindowBlockLabel3OpacityAnimation3->setDuration(450);
    StartWindowBlockLabel3OpacityAnimation3->setStartValue(1);
    StartWindowBlockLabel3OpacityAnimation3->setEndValue(1);
    StartWindowBlockLabel3OpacityAnimation4 = new QPropertyAnimation(StartWindowBlockLabel3OpacityEffect, "opacity");
    StartWindowBlockLabel3OpacityAnimation4->setDuration(50);
    StartWindowBlockLabel3OpacityAnimation4->setStartValue(1);
    StartWindowBlockLabel3OpacityAnimation4->setEndValue(0.05);
    StartWindowBlockLabel4OpacityAnimation1 = new QPropertyAnimation(StartWindowBlockLabel4OpacityEffect, "opacity");
    StartWindowBlockLabel4OpacityAnimation1->setDuration(450);
    StartWindowBlockLabel4OpacityAnimation1->setStartValue(0.05);
    StartWindowBlockLabel4OpacityAnimation1->setEndValue(0.05);
    StartWindowBlockLabel4OpacityAnimation2 = new QPropertyAnimation(StartWindowBlockLabel4OpacityEffect, "opacity");
    StartWindowBlockLabel4OpacityAnimation2->setDuration(50);
    StartWindowBlockLabel4OpacityAnimation2->setStartValue(0.05);
    StartWindowBlockLabel4OpacityAnimation2->setEndValue(1);
    StartWindowBlockLabel4OpacityAnimation3 = new QPropertyAnimation(StartWindowBlockLabel4OpacityEffect, "opacity");
    StartWindowBlockLabel4OpacityAnimation3->setDuration(450);
    StartWindowBlockLabel4OpacityAnimation3->setStartValue(1);
    StartWindowBlockLabel4OpacityAnimation3->setEndValue(1);
    StartWindowBlockLabel4OpacityAnimation4 = new QPropertyAnimation(StartWindowBlockLabel4OpacityEffect, "opacity");
    StartWindowBlockLabel4OpacityAnimation4->setDuration(50);
    StartWindowBlockLabel4OpacityAnimation4->setStartValue(1);
    StartWindowBlockLabel4OpacityAnimation4->setEndValue(0.05);

    SmallWindowMoveAnimation = new QPropertyAnimation(this, "pos");
    SmallWindowMoveAnimation->setDuration(500);
    SmallWindowMoveAnimation->setEasingCurve(QEasingCurve::OutBack);




    
    StartWindowAnimationGroup = new QSequentialAnimationGroup;
    StartWindowAnimationGroup->addAnimation(StartWindowStartOpacityAnimation);
    StartWindowAnimationGroup->addAnimation(StartWindowColorLabelAnimation1);
    StartWindowAnimationGroup->addAnimation(StartWindowColorLabelAnimation2);
    StartWindowAnimationGroup->addAnimation(StartWindowTextColorLabelAnimation);
    StartWindowAnimationGroup->addAnimation(StartWindowCloseOpacityAnimation);
    StartWindowNumberLabelAnimationGroup = new QSequentialAnimationGroup;
    StartWindowNumberLabelAnimationGroup->addAnimation(StartWindowNumberLabelOpacityAnimation1);
    StartWindowNumberLabelAnimationGroup->addAnimation(StartWindowNumberLabelOpacityAnimation2);
    StartWindowNumberLabelAnimationGroup->addAnimation(StartWindowNumberLabelOpacityAnimation3);
    StartWindowNumberLabelAnimationGroup->addAnimation(StartWindowNumberLabelOpacityAnimation4);
    StartWindowNumberLabelAnimationGroup->addAnimation(StartWindowNumberLabelOpacityAnimation5);

    StartWindowBlockLabel1AnimationGroup = new QSequentialAnimationGroup;
    StartWindowBlockLabel1AnimationGroup->addAnimation(StartWindowBlockLabel1OpacityAnimation1);
    StartWindowBlockLabel1AnimationGroup->addAnimation(StartWindowBlockLabel1OpacityAnimation2);
    StartWindowBlockLabel1AnimationGroup->addAnimation(StartWindowBlockLabel1OpacityAnimation3);
    StartWindowBlockLabel1AnimationGroup->addAnimation(StartWindowBlockLabel1OpacityAnimation4);
    StartWindowBlockLabel2AnimationGroup = new QSequentialAnimationGroup;
    StartWindowBlockLabel2AnimationGroup->addAnimation(StartWindowBlockLabel2OpacityAnimation1);
    StartWindowBlockLabel2AnimationGroup->addAnimation(StartWindowBlockLabel2OpacityAnimation2);
    StartWindowBlockLabel2AnimationGroup->addAnimation(StartWindowBlockLabel2OpacityAnimation3);
    StartWindowBlockLabel2AnimationGroup->addAnimation(StartWindowBlockLabel2OpacityAnimation4);
    StartWindowBlockLabel3AnimationGroup = new QSequentialAnimationGroup;
    StartWindowBlockLabel3AnimationGroup->addAnimation(StartWindowBlockLabel3OpacityAnimation1);
    StartWindowBlockLabel3AnimationGroup->addAnimation(StartWindowBlockLabel3OpacityAnimation2);
    StartWindowBlockLabel3AnimationGroup->addAnimation(StartWindowBlockLabel3OpacityAnimation3);
    StartWindowBlockLabel3AnimationGroup->addAnimation(StartWindowBlockLabel3OpacityAnimation4);
    StartWindowBlockLabel4AnimationGroup = new QSequentialAnimationGroup;
    StartWindowBlockLabel4AnimationGroup->addAnimation(StartWindowBlockLabel4OpacityAnimation1);
    StartWindowBlockLabel4AnimationGroup->addAnimation(StartWindowBlockLabel4OpacityAnimation2);
    StartWindowBlockLabel4AnimationGroup->addAnimation(StartWindowBlockLabel4OpacityAnimation3);
    StartWindowBlockLabel4AnimationGroup->addAnimation(StartWindowBlockLabel4OpacityAnimation4);





    SmallWindowStartAnimationGroup = new QParallelAnimationGroup;
    SmallWindowStartAnimationGroup->addAnimation(SmallWindowStartAnimation);
    SmallWindowStartAnimationGroup->addAnimation(SmallWindowStartOpacityAnimation);
    

    StartWindow->show();

    StartWindowAnimationGroup->start();





    // connects
    // 每隔一秒更新
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=] {
        updateLabel();
        });
    timer->start(1000);

    connect(StartWindowColorLabelAnimation1, &QPropertyAnimation::finished, [&] {
        StartWindowTextLabel1->show();
        StartWindowTextLabel2->show();
        StartWindowNumberLabel->show();
        StartWindowTextLabel3->show();
        StartWindowTextLabelEnglish->show();
        });
    connect(StartWindowColorLabelAnimation2, &QPropertyAnimation::finished, [&] {
        StartWindowTextColorLabel->show();
        StartWindowNumberLabelAnimationGroup->start();
        StartWindowBlockLabel1->show();
        StartWindowBlockLabel2->show();
        StartWindowBlockLabel3->show();
        StartWindowBlockLabel4->show();
        if ((timeDifference / 86400 + 1) <= 30) {
            StartWindowBlockLabel1AnimationGroup->start();
            StartWindowBlockLabel2AnimationGroup->start();
            StartWindowBlockLabel3AnimationGroup->start();
            StartWindowBlockLabel4AnimationGroup->start();
        }
        });
    connect(StartWindowAnimationGroup, &QSequentialAnimationGroup::finished, [&] {
        StartWindow->hide();
        StartWindowTextColorLabel->hide();
        StartWindowTextLabel1->hide();
        StartWindowTextLabel2->hide();
        StartWindowNumberLabel->hide();
        StartWindowTextLabel3->hide();
        StartWindowTextLabelEnglish->hide();
        SmallWindowLabel->show();
        SmallWindowStartAnimationGroup->start();
        NumberLabelOpacityEffect->setOpacity(0.05);
        StartWindowBlockLabel1->hide();
        StartWindowBlockLabel2->hide();
        StartWindowBlockLabel3->hide();
        StartWindowBlockLabel4->hide();
        });
    connect(StartWindowBlockLabel1AnimationGroup, &QSequentialAnimationGroup::finished, [&] {
        if (BlockLabelShowTimes <= 2) {
            BlockLabelShowTimes++;
            StartWindowBlockLabel1AnimationGroup->stop();
            StartWindowBlockLabel2AnimationGroup->stop();
            StartWindowBlockLabel3AnimationGroup->stop();
            StartWindowBlockLabel4AnimationGroup->stop();
            StartWindowBlockLabel1AnimationGroup->start();
            StartWindowBlockLabel2AnimationGroup->start();
            StartWindowBlockLabel3AnimationGroup->start();
            StartWindowBlockLabel4AnimationGroup->start();
        }
        else BlockLabelShowTimes = 0;
        });
    connect(StartWindowBlockLabel1OpacityAnimation1, &QPropertyAnimation::finished, [&] {
        CountdownSoundEffect->play();
        if (BlockLabelShowTimes % 2 == 0 && (timeDifference / 86400 + 1) <= 14) HeartbeatSoundEffect->play();
        });



}

ExamCountdown_v1::~ExamCountdown_v1()
{}


void ExamCountdown_v1::updateLabel() {
    // 获取系统时间
    currentDateTime = QDateTime::currentDateTime();
    currentDateTimeString = currentDateTime.toString("yyyy-M-d h:m:ss");
    timeDifference = currentDateTime.secsTo(targetDateTime);
    // 将秒数转换为格式d天h时m分s秒
    timeDifferenceString = QString::number(timeDifference / 86400) + QString::fromLocal8Bit(" 天 ") + QString::number((timeDifference % 86400) / 3600) + QString::fromLocal8Bit(" 时 ") + QString::number((timeDifference % 3600) / 60) + QString::fromLocal8Bit(" 分 ") + QString::number(timeDifference % 60) + QString::fromLocal8Bit(" 秒");
    // 更新标签文本
    SmallWindowLabel->setText(SmallWindowText + timeDifferenceString);
    StartWindowNumberLabel->setText(QString::number(timeDifference / 86400 + 1));
    StartWindowTextLabelEnglish->setText(StartWindowEnglishText + QString::number(timeDifference / 86400 + 1) + " DAYS");
    if ((currentDateTime.toString("h") == "8" && currentDateTime.toString("m") == "13" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "9" && currentDateTime.toString("m") == "3" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "9" && currentDateTime.toString("m") == "53" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "10" && currentDateTime.toString("m") == "43" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "11" && currentDateTime.toString("m") == "38" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "14" && currentDateTime.toString("m") == "20" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "15" && currentDateTime.toString("m") == "23" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "16" && currentDateTime.toString("m") == "13" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "17" && currentDateTime.toString("m") == "23" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "18" && currentDateTime.toString("m") == "6" && currentDateTime.toString("s") == "0")) {
        StartWindow->show();
        SmallWindowCloseOpacityAnimation->start();
        StartWindowAnimationGroup->start();
    }

}




void ExamCountdown_v1::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    SmallWindowMoveAnimation->stop();
    this->startX = event->x();
    this->startY = event->y();
}

void ExamCountdown_v1::mouseMoveEvent(QMouseEvent* event) {
    QWidget::mouseMoveEvent(event);
    float disX = event->x() - this->startX;
    float disY = event->y() - this->startY;
    this->move(this->x() + disX, this->y() + disY);
}

void ExamCountdown_v1::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
    if (this->x() <= desktop.width() * 0.23908) {
        SmallWindowMoveAnimation->setStartValue(this->pos());
        SmallWindowMoveAnimation->setEndValue(QPoint(0, 0));
        SmallWindowMoveAnimation->start();
        SmallWindowPosition = 0;
        QFile file("config.ini");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "SmallWindowText=" << SmallWindowText << "\n";
            out << "StartWindowText=" << StartWindowText << "\n";
            out << "StartWindowEnglishText=" << StartWindowEnglishText << "\n";
            out << "targetDateTime=" << targetDateTime.toString("yyyy-M-d h:m:ss") << "\n";
            out << "SmallWindowPosition=" << SmallWindowPosition << "\n";
            file.close();
        }
    }
    else if (this->x() >= desktop.width() * 0.47817) {
        SmallWindowMoveAnimation->setStartValue(this->pos());
        SmallWindowMoveAnimation->setEndValue(QPoint(desktop.width() - this->width(), 0));
        SmallWindowMoveAnimation->start();
        SmallWindowPosition = 2;
        QFile file("config.ini");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "SmallWindowText=" << SmallWindowText << "\n";
            out << "StartWindowText=" << StartWindowText << "\n";
            out << "StartWindowEnglishText=" << StartWindowEnglishText << "\n";
            out << "targetDateTime=" << targetDateTime.toString("yyyy-M-d h:m:ss") << "\n";
            out << "SmallWindowPosition=" << SmallWindowPosition << "\n";
            file.close();
        }
    }
    else {
        SmallWindowMoveAnimation->setStartValue(this->pos());
        SmallWindowMoveAnimation->setEndValue(QPoint((desktop.width() - this->width()) / 2, 0));
        SmallWindowMoveAnimation->start();
        SmallWindowPosition = 1;
        QFile file("config.ini");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "SmallWindowText=" << SmallWindowText << "\n";
            out << "StartWindowText=" << StartWindowText << "\n";
            out << "StartWindowEnglishText=" << StartWindowEnglishText << "\n";
            out << "targetDateTime=" << targetDateTime.toString("yyyy-M-d h:m:ss") << "\n";
            out << "SmallWindowPosition=" << SmallWindowPosition << "\n";
            file.close();
        }
    }
}
