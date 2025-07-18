#include "ExamCountdown_v1.h"
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QDesktopWidget>
#include <QDesktopServices>
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
    desktopAvailable = QApplication::desktop()->availableGeometry();
    this->setGeometry(desktop.width() * 0.35, -desktop.height() * 0.05, desktop.width() * 0.3, desktop.height() * 0.05);
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

    isSetting = false;

    isCountdownAudio = true;
    isHeartbeatAudio = true;
    isShowBigWindow = true;
    SmallWindowOnTopOrBottom = true;

    readConfig();
    
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | (SmallWindowOnTopOrBottom ? Qt::WindowStaysOnTopHint : Qt::WindowStaysOnBottomHint));
    



    // Widgets
    StartWindow = new QWidget;
    StartWindow->setGeometry(0, 0, desktop.width(), desktop.height());
    StartWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    StartWindow->setAttribute(Qt::WA_TranslucentBackground);
    if (isShowBigWindow) StartWindow->show();






    LogoPixelMultiplier = 2682.0 / 2048.0;



    

    // Labels
    SmallWindowUnderlyingLabel = new QLabel(this);
    SmallWindowUnderlyingLabel->setGeometry(0, 0, this->width(),this->height());
    borderRadius = this->height() / 2;
    border = this->height() * 0.025;
    SmallWindowUnderlyingLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235); ");
    SmallWindowUnderlyingLabel->show();
    SmallWindowLabel = new QLabel(this);
    SmallWindowLabel->setGeometry(0, 0, this->width()-this->height(), this->height());
    SmallWindowLabel->setStyleSheet("color: rgb(200, 0, 0);");
    SmallWindowLabel->setAlignment(Qt::AlignCenter);
    SmallWindowLabel->setText(SmallWindowText + timeDifferenceString);
    font.setPixelSize(this->height() * 0.6);
    SmallWindowLabel->setFont(font);
    SmallWindowLabel->show();

    SmallWindowLogoLabel = new QLabel(this);
    SmallWindowLogoLabel->setGeometry(desktop.width() * 0.625, (desktopAvailable.height() - desktop.width() * 0.25 * LogoPixelMultiplier) / 2, desktop.width() * 0.25, desktop.width() * 0.25 * LogoPixelMultiplier);
    SmallWindowLogoLabel->setPixmap(QPixmap("img/logo.black.2048px.png"));
    SmallWindowLogoLabel->setScaledContents(true);
    SmallWindowLogoLabel->show();


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





    // GroupBoxes
    borderRadius = this->height() / 4;
    border = this->height() * 0.025;
    font.setPixelSize(this->height() * 0.1);
    SmallWindowSettingTextGb = new QGroupBox(this);
    SmallWindowSettingTextGb->setGeometry(this->width() * 0.1, this->height() * 2, desktop.width() * 0.3, desktop.height() * 0.2);
    SmallWindowSettingTextGb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235); ");
    SmallWindowSettingTextGb->setTitle(QString::fromLocal8Bit("文本和时间"));
    SmallWindowSettingTextGb->show();
    SmallWindowSettingBigWindowGb = new QGroupBox(this);
    SmallWindowSettingBigWindowGb->setGeometry(this->width() * 0.1, this->height() * 6.5, desktop.width() * 0.3, desktop.height() * 0.1);
    SmallWindowSettingBigWindowGb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235); ");
    SmallWindowSettingBigWindowGb->setTitle(QString::fromLocal8Bit("全屏提醒"));
    SmallWindowSettingBigWindowGb->show();
    SmallWindowSettingSmallWindowOnTopOrBottomGb = new QGroupBox(this);
    SmallWindowSettingSmallWindowOnTopOrBottomGb->setGeometry(this->width() * 0.1, this->height() * 9, desktop.width() * 0.3, desktop.height() * 0.075);
    SmallWindowSettingSmallWindowOnTopOrBottomGb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235); ");
    SmallWindowSettingSmallWindowOnTopOrBottomGb->setTitle(QString::fromLocal8Bit("小窗口层级"));
    SmallWindowSettingSmallWindowOnTopOrBottomGb->show();



    // Labels in GroupBoxes
    SettingTextSmallWindowTextLabel = new QLabel(SmallWindowSettingTextGb);
    SettingTextSmallWindowTextLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.1, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
    SettingTextSmallWindowTextLabel->setText(QString::fromLocal8Bit("小窗口文本:"));
    SettingTextSmallWindowTextLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    font.setPixelSize(SettingTextSmallWindowTextLabel->height() * 0.6);
    SettingTextSmallWindowTextLabel->setFont(font);
    SettingTextSmallWindowTextLabel->show();
    SettingTextStartWindowTextLabel = new QLabel(SmallWindowSettingTextGb);
    SettingTextStartWindowTextLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.275, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
    SettingTextStartWindowTextLabel->setText(QString::fromLocal8Bit("大窗口文本:"));
    SettingTextStartWindowTextLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    SettingTextStartWindowTextLabel->setFont(font);
    SettingTextStartWindowTextLabel->show();
    SettingTextStartWindowEnglishLabel = new QLabel(SmallWindowSettingTextGb);
    SettingTextStartWindowEnglishLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.45, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
    SettingTextStartWindowEnglishLabel->setText(QString::fromLocal8Bit("大窗口英文:"));
    SettingTextStartWindowEnglishLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    SettingTextStartWindowEnglishLabel->setFont(font);
    SettingTextStartWindowEnglishLabel->show();
    SettingTextTimeLabel = new QLabel(SmallWindowSettingTextGb);
    SettingTextTimeLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.625, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
    SettingTextTimeLabel->setText(QString::fromLocal8Bit("终点时间:"));
    SettingTextTimeLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    SettingTextTimeLabel->setFont(font);
    SettingTextTimeLabel->show();






    // PushButtons
    SmallWindowMoreInfBtn = new QPushButton(this);
    SmallWindowMoreInfBtn->setGeometry(this->width() - this->height(), 0, this->height(), this->height());
    borderRadius = this->height() / 2;
    border = this->height() * 0.025;
    SmallWindowMoreInfBtn->setStyleSheet("QPushButton{background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QPushButton:hover{background-color: rgba(255, 255, 255, 0.6);} QPushButton:pressed{background-color: rgba(100, 100, 100, 0.3);}");
    SmallWindowMoreInfBtn->setIcon(QIcon("img/moreinf.png"));
    SmallWindowMoreInfBtn->setIconSize(QSize(this->height() / 2, this->height() / 2));
    font.setPixelSize(this->height());
    SmallWindowMoreInfBtn->setFont(font);
    SmallWindowMoreInfBtn->show();

    SettingTextYesBtn = new QPushButton(SmallWindowSettingTextGb);
    SettingTextYesBtn->setGeometry(0, SmallWindowSettingTextGb->height() * 0.8, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.2);
    SettingTextYesBtn->setStyleSheet("QPushButton{background-color: rgba(235, 235, 235, 0.5); border-top-left-radius: 0; border-top-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QPushButton:hover{background-color: rgba(255, 255, 255, 0.6);} QPushButton:pressed{background-color: rgba(100, 100, 100, 0.3);}");
    SettingTextYesBtn->setText(QString::fromLocal8Bit("确定"));
    font.setPixelSize(SettingTextYesBtn->height() * 0.6);
    SettingTextYesBtn->setFont(font);
    SettingTextYesBtn->show();

    SettingBigWindowTryBtn = new QPushButton(SmallWindowSettingBigWindowGb);
    SettingBigWindowTryBtn->setGeometry(SmallWindowSettingBigWindowGb->width() * 0.8 , SmallWindowSettingBigWindowGb->height() * 0.2, SmallWindowSettingBigWindowGb->width() * 0.2, SmallWindowSettingBigWindowGb->height() * 0.8);
    SettingBigWindowTryBtn->setStyleSheet("QPushButton{background-color: rgba(235, 235, 235, 0.5); border-top-right-radius: 0; border-top-left-radius: 0; border-bottom-left-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QPushButton:hover{background-color: rgba(255, 255, 255, 0.6);} QPushButton:pressed{background-color: rgba(100, 100, 100, 0.3);} QPushButton:disabled{background-color: rgba(155, 155, 155, 0.25); border: " + QString::number(border) + "px solid rgb(155, 155, 155);}");
    SettingBigWindowTryBtn->setText(QString::fromLocal8Bit("立即\n播放\n动画"));
    SettingBigWindowTryBtn->setEnabled(isShowBigWindow);
    font.setPixelSize(SettingBigWindowTryBtn->height() * 0.2);
    SettingBigWindowTryBtn->setFont(font);
    SettingBigWindowTryBtn->show();

    BilibiliBtn = new QPushButton(this);
    BilibiliBtn->setGeometry(this->width() * 0.1, this->height() * 12, desktop.width() * 0.3, desktop.height() * 0.05);
    borderRadius = BilibiliBtn->height();
    BilibiliBtn->setStyleSheet("QPushButton{background-color: rgba(255, 102, 153, 0.5); border-top-left-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(255, 102, 153);} QPushButton:hover{background-color: rgba(255, 102, 153, 0.6);} QPushButton:pressed{background-color: rgba(255, 102, 153, 0.2);}");
    BilibiliBtn->setText(QString::fromLocal8Bit("哔哩哔哩：@龙ger_longer"));
    font.setPixelSize(BilibiliBtn->height() * 0.6);
    BilibiliBtn->setFont(font);
    BilibiliBtn->show();
    GithubBtn = new QPushButton(this);
    GithubBtn->setGeometry(this->width() * 0.1, this->height() * 13, desktop.width() * 0.3, desktop.height() * 0.05);
    GithubBtn->setStyleSheet("QPushButton{background-color: rgba(135, 205, 250, 0.5); border-bottom-left-radius: 0; border-bottom-left-radius: " + QString::number(borderRadius) + "px; border-bottom-right-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(135, 205, 250);} QPushButton:hover{background-color: rgba(135, 205, 250, 0.6);} QPushButton:pressed{background-color: rgba(135, 205, 250, 0.2);}");
    GithubBtn->setText(QString::fromLocal8Bit("Github开源仓库"));
    GithubBtn->setFont(font);
    GithubBtn->show();

    ExitBtn = new QPushButton(this);
    ExitBtn->setGeometry(this->width() * 0.1, this->height() * 15, desktop.width() * 0.3, desktop.height() * 0.05);
    borderRadius = ExitBtn->height() / 2;
    ExitBtn->setStyleSheet("QPushButton{background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QPushButton:hover{background-color: rgba(255, 255, 255, 0.6);} QPushButton:pressed{background-color: rgba(235, 235, 235, 0.3);}");
    ExitBtn->setText(QString::fromLocal8Bit("退出"));
    ExitBtn->setFont(font);
    ExitBtn->show();





    // LineEdits
    borderRadius = SettingTextSmallWindowTextLabel->height() / 2;
    SettingTextSmallWindowTextLedt = new QLineEdit(SettingTextSmallWindowTextLabel);
    SettingTextSmallWindowTextLedt->setGeometry(SettingTextSmallWindowTextLabel->width() * 0.25, 0, SettingTextSmallWindowTextLabel->width() * 0.75, SettingTextSmallWindowTextLabel->height());
    SettingTextSmallWindowTextLedt->setText(SmallWindowText);
    SettingTextSmallWindowTextLedt->setPlaceholderText(QString::fromLocal8Bit("小窗口文本"));
    SettingTextSmallWindowTextLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
    font.setPixelSize(SettingTextSmallWindowTextLedt->height() * 0.6);
    SettingTextSmallWindowTextLedt->setFont(font);
    SettingTextSmallWindowTextLedt->show();
    SettingTextStartWindowTextLedt = new QLineEdit(SettingTextStartWindowTextLabel);
    SettingTextStartWindowTextLedt->setGeometry(SettingTextStartWindowTextLabel->width() * 0.25, 0, SettingTextStartWindowTextLabel->width() * 0.75, SettingTextStartWindowTextLabel->height());
    SettingTextStartWindowTextLedt->setText(StartWindowText);
    SettingTextStartWindowTextLedt->setPlaceholderText(QString::fromLocal8Bit("大窗口文本"));
    SettingTextStartWindowTextLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
    SettingTextStartWindowTextLedt->setFont(font);
    SettingTextStartWindowTextLedt->show();
    SettingTextStartWindowEnglishLedt = new QLineEdit(SettingTextStartWindowEnglishLabel);
    SettingTextStartWindowEnglishLedt->setGeometry(SettingTextStartWindowEnglishLabel->width() * 0.25, 0, SettingTextStartWindowEnglishLabel->width() * 0.75, SettingTextStartWindowEnglishLabel->height());
    SettingTextStartWindowEnglishLedt->setText(StartWindowEnglishText);
    SettingTextStartWindowEnglishLedt->setPlaceholderText(QString::fromLocal8Bit("大窗口英文"));
    SettingTextStartWindowEnglishLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
    SettingTextStartWindowEnglishLedt->setFont(font);
    SettingTextStartWindowEnglishLedt->show();
    SettingTextTimeLedt = new QLineEdit(SettingTextTimeLabel);
    SettingTextTimeLedt->setGeometry(SettingTextTimeLabel->width() * 0.25, 0, SettingTextTimeLabel->width() * 0.75, SettingTextTimeLabel->height());
    SettingTextTimeLedt->setText(targetDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    SettingTextTimeLedt->setPlaceholderText(QString::fromLocal8Bit("终点时间"));
    SettingTextTimeLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
    SettingTextTimeLedt->setFont(font);
    SettingTextTimeLedt->show();




    // CheckBoxes
    SettingBigWindowIsShowCb = new QCheckBox(SmallWindowSettingBigWindowGb);
    SettingBigWindowIsShowCb->setGeometry(0, SmallWindowSettingBigWindowGb->height() * 0.2, SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.8 / 3);
    SettingBigWindowIsShowCb->setText(QString::fromLocal8Bit("是否显示全屏提醒"));
    SettingBigWindowIsShowCb->setChecked(isShowBigWindow);
    SettingBigWindowIsShowCb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    font.setPixelSize(SettingBigWindowIsShowCb->height() * 0.6);
    SettingBigWindowIsShowCb->setFont(font);
    SettingBigWindowIsShowCb->show();
    SettingBigWindowIsCountdownAudioCb = new QCheckBox(SmallWindowSettingBigWindowGb);
    SettingBigWindowIsCountdownAudioCb->setGeometry(0, SmallWindowSettingBigWindowGb->height() * 0.2 + SmallWindowSettingBigWindowGb->height() * 0.8 / 3, SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.8 / 3);
    SettingBigWindowIsCountdownAudioCb->setText(QString::fromLocal8Bit("剩余时间≤30天时是否播放倒计时提醒音"));
    SettingBigWindowIsCountdownAudioCb->setChecked(isCountdownAudio);
    SettingBigWindowIsCountdownAudioCb->setEnabled(isShowBigWindow);
    SettingBigWindowIsCountdownAudioCb->setStyleSheet("QCheckBox{background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QCheckBox:disabled{background-color: rgba(155, 155, 155, 0.25); border: " + QString::number(border) + "px solid rgb(155, 155, 155);}");
    SettingBigWindowIsCountdownAudioCb->setFont(font);
    SettingBigWindowIsCountdownAudioCb->show();
    SettingBigWindowIsHeartbeatAudioCb = new QCheckBox(SmallWindowSettingBigWindowGb);
    SettingBigWindowIsHeartbeatAudioCb->setGeometry(0, SmallWindowSettingBigWindowGb->height() * 0.2 + 2 * SmallWindowSettingBigWindowGb->height() * 0.8 / 3, SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.8 / 3);
    SettingBigWindowIsHeartbeatAudioCb->setText(QString::fromLocal8Bit("剩余时间≤14天时是否播放心跳提醒音"));
    SettingBigWindowIsHeartbeatAudioCb->setChecked(isHeartbeatAudio);
    SettingBigWindowIsHeartbeatAudioCb->setEnabled(isShowBigWindow);
    SettingBigWindowIsHeartbeatAudioCb->setStyleSheet("QCheckBox{background-color: rgba(235, 235, 235, 0.5); border-top-right-radius: 0; border-top-left-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QCheckBox:disabled{background-color: rgba(155, 155, 155, 0.25); border: " + QString::number(border) + "px solid rgb(155, 155, 155);}");
    SettingBigWindowIsHeartbeatAudioCb->setFont(font);
    SettingBigWindowIsHeartbeatAudioCb->show();





    // RadioButtons
    SettingSmallWindowOnTopRbtn = new QRadioButton(SmallWindowSettingSmallWindowOnTopOrBottomGb);
    SettingSmallWindowOnTopRbtn->setGeometry(0, SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.2, SmallWindowSettingSmallWindowOnTopOrBottomGb->width(), SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.4);
    SettingSmallWindowOnTopRbtn->setText(QString::fromLocal8Bit("小窗口置顶"));
    SettingSmallWindowOnTopRbtn->setChecked(SmallWindowOnTopOrBottom);
    SettingSmallWindowOnTopRbtn->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    font.setPixelSize(SettingSmallWindowOnTopRbtn->height() * 0.6);
    SettingSmallWindowOnTopRbtn->setFont(font);
    SettingSmallWindowOnTopRbtn->show();
    SettingSmallWindowOnBottomRbtn = new QRadioButton(SmallWindowSettingSmallWindowOnTopOrBottomGb);
    SettingSmallWindowOnBottomRbtn->setGeometry(0, SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.6, SmallWindowSettingSmallWindowOnTopOrBottomGb->width(), SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.4);
    SettingSmallWindowOnBottomRbtn->setText(QString::fromLocal8Bit("小窗口置底"));
    SettingSmallWindowOnBottomRbtn->setChecked(!SmallWindowOnTopOrBottom);
    SettingSmallWindowOnBottomRbtn->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-top-right-radius: 0; border-top-left-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    SettingSmallWindowOnBottomRbtn->setFont(font);
    SettingSmallWindowOnBottomRbtn->show();

    
    
    
    
    // Animations
    SmallWindowStartAnimation = new QPropertyAnimation(this, "pos");
    SmallWindowStartAnimation->setDuration(1000);
    SmallWindowStartAnimation->setEasingCurve(QEasingCurve::OutBack);
    if (SmallWindowPosition == 0) {
        SmallWindowStartAnimation->setStartValue(QPoint(-desktop.width() * 0.3, 0));
        SmallWindowStartAnimation->setEndValue(QPoint(0, 0));
    }
    else if (SmallWindowPosition == 1) {
        SmallWindowStartAnimation->setStartValue(QPoint(desktop.width() * 0.35, -desktop.height() * 0.05));
        SmallWindowStartAnimation->setEndValue(QPoint(desktop.width() * 0.35, 0));
    }
    else if (SmallWindowPosition == 2) {
        SmallWindowStartAnimation->setStartValue(QPoint(desktop.width(), 0));
        SmallWindowStartAnimation->setEndValue(QPoint(desktop.width() * 0.7, 0));
    }
    SmallWindowStartOpacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    SmallWindowStartOpacityAnimation->setDuration(1000);
    SmallWindowStartOpacityAnimation->setStartValue(0);
    SmallWindowStartOpacityAnimation->setEndValue(1);
    SmallWindowStartOpacityAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    SmallWindowCloseOpacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    SmallWindowCloseOpacityAnimation->setDuration(1000);
    SmallWindowCloseOpacityAnimation->setStartValue(1);
    SmallWindowCloseOpacityAnimation->setEndValue(0);
    SmallWindowCloseOpacityAnimation->setEasingCurve(QEasingCurve::OutCubic);

    SmallWindowStartSettingAnimation = new QPropertyAnimation(this, "geometry");
    SmallWindowStartSettingAnimation->setDuration(1000);
    if (SmallWindowPosition == 0) 
        SmallWindowStartSettingAnimation->setStartValue(QRect(0, 0, desktop.width() * 0.3, desktop.height() * 0.05));
    else if (SmallWindowPosition == 1)
        SmallWindowStartSettingAnimation->setStartValue(QRect(desktop.width() * 0.35, 0, desktop.width() * 0.3, desktop.height() * 0.05));
    else if (SmallWindowPosition == 2)
        SmallWindowStartSettingAnimation->setStartValue(QRect(desktop.width() * 0.7, 0, desktop.width() * 0.3, desktop.height() * 0.05));
    SmallWindowStartSettingAnimation->setEndValue(QRect(0, 0, desktopAvailable.width(), desktopAvailable.height()));
    SmallWindowStartSettingAnimation->setEasingCurve(QEasingCurve::OutExpo);
    SmallWindowUnderlyingLabelStartSettingAnimation = new QPropertyAnimation(SmallWindowUnderlyingLabel, "geometry");
    SmallWindowUnderlyingLabelStartSettingAnimation->setDuration(1000);
    SmallWindowUnderlyingLabelStartSettingAnimation->setStartValue(QRect(0, 0, desktop.width() * 0.3, desktop.height() * 0.05));
    SmallWindowUnderlyingLabelStartSettingAnimation->setEndValue(QRect(0, 0, desktopAvailable.width(), desktopAvailable.height()));
    SmallWindowUnderlyingLabelStartSettingAnimation->setEasingCurve(QEasingCurve::OutExpo);

    SmallWindowCloseSettingAnimation = new QPropertyAnimation(this, "geometry");
    SmallWindowCloseSettingAnimation->setDuration(1000);
    SmallWindowCloseSettingAnimation->setStartValue(QRect(0, 0, desktopAvailable.width(), desktopAvailable.height()));
    if (SmallWindowPosition == 0)
        SmallWindowCloseSettingAnimation->setEndValue(QRect(0, 0, desktop.width() * 0.3, desktop.height() * 0.05));
    else if (SmallWindowPosition == 1)
        SmallWindowCloseSettingAnimation->setEndValue(QRect(desktop.width() * 0.35, 0, desktop.width() * 0.3, desktop.height() * 0.05));
    else if (SmallWindowPosition == 2)
        SmallWindowCloseSettingAnimation->setEndValue(QRect(desktop.width() * 0.7, 0, desktop.width() * 0.3, desktop.height() * 0.05));
    SmallWindowCloseSettingAnimation->setEasingCurve(QEasingCurve::OutExpo);
    SmallWindowUnderlyingLabelCloseSettingAnimation = new QPropertyAnimation(SmallWindowUnderlyingLabel, "geometry");
    SmallWindowUnderlyingLabelCloseSettingAnimation->setDuration(1000);
    SmallWindowUnderlyingLabelCloseSettingAnimation->setStartValue(QRect(0, 0, desktopAvailable.width(), desktopAvailable.height()));
    SmallWindowUnderlyingLabelCloseSettingAnimation->setEndValue(QRect(0, 0, desktop.width() * 0.3, desktop.height() * 0.05));
    SmallWindowUnderlyingLabelCloseSettingAnimation->setEasingCurve(QEasingCurve::OutExpo);


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
    


    if (isShowBigWindow) {
        StartWindow->show();
        StartWindowAnimationGroup->start();
    }
    else SmallWindowStartAnimationGroup->start();





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
        SmallWindowUnderlyingLabel->setGeometry(0, 0, this->width(), this->height());
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
        if (isCountdownAudio) CountdownSoundEffect->play();
        if (BlockLabelShowTimes % 2 == 0 && (timeDifference / 86400 + 1) <= 14 && isHeartbeatAudio) HeartbeatSoundEffect->play();
        });

    connect(SmallWindowMoreInfBtn, &QPushButton::clicked, [&] {
        if (!isSetting) {
            isSetting = true;
            SmallWindowCloseSettingAnimation->stop();
            SmallWindowUnderlyingLabelCloseSettingAnimation->stop();
            SmallWindowStartSettingAnimation->setStartValue(QRect(this->x(), this->y(), this->width(), this->height()));
            SmallWindowUnderlyingLabelStartSettingAnimation->setStartValue(QRect(0, 0, this->width(), this->height()));
            SmallWindowStartSettingAnimation->start();
            SmallWindowUnderlyingLabelStartSettingAnimation->start();
        }
        else {
            isSetting = false;
            SmallWindowStartSettingAnimation->stop();
            SmallWindowUnderlyingLabelStartSettingAnimation->stop();
            if (SmallWindowPosition == 0)
                SmallWindowCloseSettingAnimation->setEndValue(QRect(0, 0, desktop.width() * 0.3, desktop.height() * 0.05));
            else if (SmallWindowPosition == 1)
                SmallWindowCloseSettingAnimation->setEndValue(QRect(desktop.width() * 0.35, 0, desktop.width() * 0.3, desktop.height() * 0.05));
            else if (SmallWindowPosition == 2)
                SmallWindowCloseSettingAnimation->setEndValue(QRect(desktop.width() * 0.7, 0, desktop.width() * 0.3, desktop.height() * 0.05));
            SmallWindowUnderlyingLabelCloseSettingAnimation->setStartValue(QRect(0, 0, this->width(), this->height()));
            SmallWindowCloseSettingAnimation->start();
            SmallWindowUnderlyingLabelCloseSettingAnimation->start();
        }
        });

    connect(SettingTextYesBtn, &QPushButton::clicked, [&] {
        SmallWindowText = SettingTextSmallWindowTextLedt->text();
        StartWindowText = SettingTextStartWindowTextLedt->text();
        StartWindowEnglishText = SettingTextStartWindowEnglishLedt->text();
        targetDateTime = QDateTime::fromString(SettingTextTimeLedt->text(), "yyyy-M-d h:m:ss");
        writeConfig();
        });
    connect(SettingBigWindowIsShowCb, &QCheckBox::stateChanged, [&] {
        isShowBigWindow = SettingBigWindowIsShowCb->isChecked();
        SettingBigWindowIsCountdownAudioCb->setEnabled(isShowBigWindow);
        SettingBigWindowIsHeartbeatAudioCb->setEnabled(isShowBigWindow);
        SettingBigWindowTryBtn->setEnabled(isShowBigWindow);
        writeConfig();
        });
    connect(SettingBigWindowIsCountdownAudioCb, &QCheckBox::stateChanged, [&] {
        isCountdownAudio = SettingBigWindowIsCountdownAudioCb->isChecked();
        writeConfig();
        });
    connect(SettingBigWindowIsHeartbeatAudioCb, &QCheckBox::stateChanged, [&] {
        isHeartbeatAudio = SettingBigWindowIsHeartbeatAudioCb->isChecked();
        writeConfig();
        });
    connect(SettingBigWindowTryBtn, &QPushButton::clicked, [&] {
        startShowBigWindowAnimation();
        });
    connect(SettingSmallWindowOnTopRbtn, &QRadioButton::clicked, [&] {
        SmallWindowOnTopOrBottom = true;
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
        this->show();
        writeConfig();
        });
    connect(SettingSmallWindowOnBottomRbtn, &QRadioButton::clicked, [&] {
        SmallWindowOnTopOrBottom = false;
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnBottomHint);
        this->show();
        writeConfig();
        });

    connect(BilibiliBtn, &QPushButton::clicked, [&] {
        QDesktopServices::openUrl(QUrl("https://space.bilibili.com/3493110082439389"));
        });
    connect(GithubBtn, &QPushButton::clicked, [&] {
        QDesktopServices::openUrl(QUrl("https://github.com/longlonger2022/Universal-Timer"));
        });

    connect(ExitBtn, &QPushButton::clicked, [&] {
        if (QMessageBox::question(this, QString::fromLocal8Bit("退出"), QString::fromLocal8Bit("确定要退出吗？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) qApp->quit();
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
    if (((currentDateTime.toString("h") == "8" && currentDateTime.toString("m") == "13" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "9" && currentDateTime.toString("m") == "3" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "9" && currentDateTime.toString("m") == "53" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "10" && currentDateTime.toString("m") == "43" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "11" && currentDateTime.toString("m") == "38" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "14" && currentDateTime.toString("m") == "20" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "15" && currentDateTime.toString("m") == "23" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "16" && currentDateTime.toString("m") == "13" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "17" && currentDateTime.toString("m") == "23" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "18" && currentDateTime.toString("m") == "6" && currentDateTime.toString("s") == "0")) && isShowBigWindow) {
        startShowBigWindowAnimation();
    }

}

void ExamCountdown_v1::readConfig() {
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
            else if (list[0] == "isCountdownAudio")
                isCountdownAudio = list[1].toInt();
            else if (list[0] == "isHeartbeatAudio")
                isHeartbeatAudio = list[1].toInt();
            else if (list[0] == "isShowBigWindow")
                isShowBigWindow = list[1].toInt();
            else if (list[0] == "SmallWindowOnTopOrBottom")
                SmallWindowOnTopOrBottom = list[1].toInt();
        }
        file.close();
    }
    else {
        QMessageBox::critical(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未找到配置文件，将使用默认配置。"));
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "SmallWindowText=" << SmallWindowText << "\n";
            out << "StartWindowText=" << StartWindowText << "\n";
            out << "StartWindowEnglishText=" << StartWindowEnglishText << "\n";
            out << "targetDateTime=" << targetDateTime.toString("yyyy-M-d h:m:ss") << "\n";
            out << "SmallWindowPosition=" << SmallWindowPosition << "\n";
            out << "isCountdownAudio=" << isCountdownAudio << "\n";
            out << "isHeartbeatAudio=" << isHeartbeatAudio << "\n";
            out << "isShowBigWindow=" << isShowBigWindow << "\n";
            out << "SmallWindowOnTopOrBottom=" << SmallWindowOnTopOrBottom << "\n";
            file.close();
        }
    }
}
void ExamCountdown_v1::writeConfig() {
    QFile file("config.ini");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "SmallWindowText=" << SmallWindowText << "\n";
        out << "StartWindowText=" << StartWindowText << "\n";
        out << "StartWindowEnglishText=" << StartWindowEnglishText << "\n";
        out << "targetDateTime=" << targetDateTime.toString("yyyy-M-d h:m:ss") << "\n";
        out << "SmallWindowPosition=" << SmallWindowPosition << "\n";
        out << "isCountdownAudio=" << isCountdownAudio << "\n";
        out << "isHeartbeatAudio=" << isHeartbeatAudio << "\n";
        out << "isShowBigWindow=" << isShowBigWindow << "\n";
        out << "SmallWindowOnTopOrBottom=" << SmallWindowOnTopOrBottom << "\n";
        file.close();
    }
}

void ExamCountdown_v1::startShowBigWindowAnimation() {
    StartWindow->show();
    StartWindowAnimationGroup->start();
    if (isSetting) SmallWindowCloseSettingAnimation->start();
    SmallWindowCloseOpacityAnimation->start();
    isSetting = false;
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
    if (!isSetting) this->move(this->x() + disX, this->y() + disY);
}

void ExamCountdown_v1::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
    if (!isSetting) {
        if (this->x() <= desktop.width() * 0.233333) {
            SmallWindowMoveAnimation->setStartValue(this->pos());
            SmallWindowMoveAnimation->setEndValue(QPoint(0, 0));
            SmallWindowMoveAnimation->start();
            SmallWindowPosition = 0;
            SmallWindowStartAnimation->setStartValue(QPoint(-desktop.width() * 0.3, 0));
            SmallWindowStartAnimation->setEndValue(QPoint(0, 0));
        }
        else if (this->x() >= desktop.width() * 0.466667) {
            SmallWindowMoveAnimation->setStartValue(this->pos());
            SmallWindowMoveAnimation->setEndValue(QPoint(desktop.width() - this->width(), 0));
            SmallWindowMoveAnimation->start();
            SmallWindowPosition = 2;
            SmallWindowStartAnimation->setStartValue(QPoint(desktop.width(), 0));
            SmallWindowStartAnimation->setEndValue(QPoint(desktop.width() * 0.7, 0));
        }
        else {
            SmallWindowMoveAnimation->setStartValue(this->pos());
            SmallWindowMoveAnimation->setEndValue(QPoint((desktop.width() - this->width()) / 2, 0));
            SmallWindowMoveAnimation->start();
            SmallWindowPosition = 1;
            SmallWindowStartAnimation->setStartValue(QPoint(desktop.width() * 0.35, -desktop.height() * 0.05));
            SmallWindowStartAnimation->setEndValue(QPoint(desktop.width() * 0.35, 0));
        }
        writeConfig();
    }
}

