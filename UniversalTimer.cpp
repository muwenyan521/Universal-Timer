#include "UniversalTimer.h"
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QLineEdit>
#include <QListView>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QScreen>
#include <QDesktopServices>
#include <QDateTime>
#include <QTimer>
#include <QThread>
#include <QGraphicsEffect>
#include <QSoundEffect>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>





UniversalTimer::UniversalTimer(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    // 窗口设置
    desktop = QApplication::primaryScreen()->geometry();
    desktopAvailable = QApplication::primaryScreen()->availableGeometry();
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



    SmallWindowText = QString::fromUtf8("会考倒计时：");
    StartWindowText = QString::fromUtf8("距会考");
    StartWindowEnglishText = "THE EXAM IN ";

    RightConfigVersion = "1.3.0.0";
    ConfigVersion = "none";


    // 获取系统时间
    currentDateTime = QDateTime::currentDateTime();
    currentDateTimeString = currentDateTime.toString("yyyy-M-d h:m:ss");
    // 将当前时间与6月30日0:0:00进行比较
    targetDateTime = QDateTime::fromString("2025-6-30 0:0:00", "yyyy-M-d h:m:ss");
    timeDifference = currentDateTime.secsTo(targetDateTime);
    // 将秒数转换为格式d天h时m分s秒
    timeDifferenceString = QString::number(timeDifference / 86400) + QString::fromUtf8(" 天 ") + QString::number((timeDifference % 86400) / 3600) + QString::fromUtf8(" 时 ") + QString::number((timeDifference % 3600) / 60) + QString::fromUtf8(" 分 ") + QString::number(timeDifference % 60) + QString::fromUtf8(" 秒");




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


    Language = "zh-CN";
    RightLanguageVersion = "1.3.0.0";
    LanguageVersion = "none";

    readConfig();
    if (ConfigVersion != RightConfigVersion) {
        if (QMessageBox::warning(NULL, QString::fromUtf8("万能倒计时 - 警告"), QString::fromUtf8("配置文件版本不匹配，可能导致程序运行不正常，是否继续？\n请删除版本不匹配的配置文件，重新启动程序将生成符合版本的默认配置文件。\n(点击“取消”将退出程序)"), QMessageBox::Yes | QMessageBox::Cancel) != QMessageBox::Yes) exit(0);
    }

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | (SmallWindowOnTopOrBottom ? Qt::WindowStaysOnTopHint : Qt::WindowStaysOnBottomHint));




    // Widgets
    StartWindow = new QWidget;
    StartWindow->setGeometry(0, 0, desktop.width(), desktop.height());
    StartWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    StartWindow->setAttribute(Qt::WA_TranslucentBackground);
    StartWindow->setStyleSheet("font-family: DIN1451, zihun59hao-chuangcuhei, Microsoft Yahei UI;");
    if (isShowBigWindow) StartWindow->show();



    // MessageBoxes
    AskExitMsgBox = new QMessageBox(this);
    AskExitMsgBox->setWindowTitle(QString::fromUtf8("万能倒计时 - 提示"));
    AskExitMsgBox->setText(QString::fromUtf8("确定要退出吗？"));
    AskExitMsgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    AskExitMsgBox->setDefaultButton(QMessageBox::No);
    AskExitMsgBox->setIcon(QMessageBox::Question);
    AskExitMsgBox->setWindowOpacity(0.85);




    LogoPixelMultiplier = 2682.0 / 2048.0;

    // Labels
    SmallWindowUnderlyingLabel = new QLabel(this);
    SmallWindowUnderlyingLabel->setGeometry(0, 0, this->width(), this->height());
    borderRadius = this->height() / 2;
    border = this->height() * 0.025;
    SmallWindowUnderlyingLabel->setStyleSheet("background-color: rgba(255, 255, 255, 0.5); border-radius: " + QString::number(borderRadius) + "px;");
    SmallWindowUnderlyingLabel->show();
    SmallWindowLabel = new QLabel(this);
    SmallWindowLabel->setGeometry(0, 0, this->width() - desktop.height() * 0.05, desktop.height() * 0.05);
    SmallWindowLabel->setStyleSheet("color: rgb(200, 0, 0);");
    SmallWindowLabel->setAlignment(Qt::AlignCenter);
    SmallWindowLabel->setText(SmallWindowText + timeDifferenceString);
    font.setPixelSize(this->height() * 0.6);
    SmallWindowLabel->setFont(font);
    SmallWindowLabel->show();

    SmallWindowLogoLabel = new QLabel(this);
    SmallWindowLogoLabel->setGeometry(desktop.width() * 0.625, (desktopAvailable.height() - desktop.width() * 0.25 * LogoPixelMultiplier) / 2, desktop.width() * 0.25, desktop.width() * 0.25 * LogoPixelMultiplier);
    SmallWindowLogoLabel->setPixmap(QPixmap(":/img/logo.black.2048px.png"));
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
    StartWindowTextLabel2->setText(QString::fromUtf8("还剩"));
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
    StartWindowTextLabel3->setText(QString::fromUtf8("天"));
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
    SmallWindowSettingTextGb->setGeometry(this->width() * 0.1, this->height() * 2, desktop.width() * 0.465, desktop.height() * 0.2);
    SmallWindowSettingTextGb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235); ");
    SmallWindowSettingTextGb->setTitle(QString::fromUtf8("文本和时间"));
    SmallWindowSettingTextGb->show();
    SmallWindowSettingBigWindowGb = new QGroupBox(this);
    SmallWindowSettingBigWindowGb->setGeometry(this->width() * 0.1, this->height() * 6.5, desktop.width() * 0.465, desktop.height() * 0.1);
    SmallWindowSettingBigWindowGb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235); ");
    SmallWindowSettingBigWindowGb->setTitle(QString::fromUtf8("全屏提醒"));
    SmallWindowSettingBigWindowGb->show();
    SmallWindowSettingSmallWindowOnTopOrBottomGb = new QGroupBox(this);
    SmallWindowSettingSmallWindowOnTopOrBottomGb->setGeometry(this->width() * 0.1, this->height() * 9, desktop.width() * 0.465, desktop.height() * 0.075);
    SmallWindowSettingSmallWindowOnTopOrBottomGb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235); ");
    SmallWindowSettingSmallWindowOnTopOrBottomGb->setTitle(QString::fromUtf8("小窗口层级"));
    SmallWindowSettingSmallWindowOnTopOrBottomGb->show();



    // Labels in GroupBoxes
    SettingTextSmallWindowTextLabel = new QLabel(SmallWindowSettingTextGb);
    SettingTextSmallWindowTextLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.1, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
    SettingTextSmallWindowTextLabel->setText(QString::fromUtf8("小窗口文本:"));
    SettingTextSmallWindowTextLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    font.setPixelSize(SettingTextSmallWindowTextLabel->height() * 0.5);
    SettingTextSmallWindowTextLabel->setFont(font);
    SettingTextSmallWindowTextLabel->show();
    SettingTextStartWindowTextLabel = new QLabel(SmallWindowSettingTextGb);
    SettingTextStartWindowTextLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.275, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
    SettingTextStartWindowTextLabel->setText(QString::fromUtf8("大窗口文本:"));
    SettingTextStartWindowTextLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    SettingTextStartWindowTextLabel->setFont(font);
    SettingTextStartWindowTextLabel->show();
    SettingTextStartWindowEnglishLabel = new QLabel(SmallWindowSettingTextGb);
    SettingTextStartWindowEnglishLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.45, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
    SettingTextStartWindowEnglishLabel->setText(QString::fromUtf8("大窗口英文:"));
    SettingTextStartWindowEnglishLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    SettingTextStartWindowEnglishLabel->setFont(font);
    SettingTextStartWindowEnglishLabel->show();
    SettingTextTimeLabel = new QLabel(SmallWindowSettingTextGb);
    SettingTextTimeLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.625, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
    SettingTextTimeLabel->setText(QString::fromUtf8("终点时间:"));
    SettingTextTimeLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    SettingTextTimeLabel->setFont(font);
    SettingTextTimeLabel->show();






    // PushButtons
    SmallWindowMoreInfBtn = new QPushButton(this);
    SmallWindowMoreInfBtn->setGeometry(this->width() - desktop.height() * 0.05, 0, desktop.height() * 0.05, desktop.height() * 0.05);
    borderRadius = this->height() / 2;
    border = this->height() * 0.025;
    SmallWindowMoreInfBtn->setStyleSheet("QPushButton{background-color: rgba(255, 255, 255, 0.5); border-radius: " + QString::number(borderRadius) + "px;} QPushButton:hover{background-color: rgba(255, 255, 255, 0.6);} QPushButton:pressed{background-color: rgba(100, 100, 100, 0.3);}");
    SmallWindowMoreInfBtn->setIcon(QIcon(":/img/moreinf.png"));
    SmallWindowMoreInfBtn->setIconSize(QSize(this->height() / 2, this->height() / 2));
    font.setPixelSize(this->height());
    SmallWindowMoreInfBtn->setFont(font);
    SmallWindowMoreInfBtn->show();

    SettingTextYesBtn = new QPushButton(SmallWindowSettingTextGb);
    SettingTextYesBtn->setGeometry(0, SmallWindowSettingTextGb->height() * 0.8, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.2);
    SettingTextYesBtn->setStyleSheet("QPushButton{background-color: rgba(235, 235, 235, 0.5); border-top-left-radius: 0; border-top-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QPushButton:hover{background-color: rgba(255, 255, 255, 0.6);} QPushButton:pressed{background-color: rgba(100, 100, 100, 0.3);}");
    SettingTextYesBtn->setText(QString::fromUtf8("保存"));
    font.setPixelSize(SettingTextYesBtn->height() * 0.6);
    SettingTextYesBtn->setFont(font);
    SettingTextYesBtn->show();

    SettingBigWindowTryBtn = new QPushButton(SmallWindowSettingBigWindowGb);
    SettingBigWindowTryBtn->setGeometry(SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.2, SmallWindowSettingBigWindowGb->width() * 0.2, SmallWindowSettingBigWindowGb->height() * 0.8);
    SettingBigWindowTryBtn->setStyleSheet("QPushButton{background-color: rgba(235, 235, 235, 0.5); border-top-right-radius: 0; border-top-left-radius: 0; border-bottom-left-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QPushButton:hover{background-color: rgba(255, 255, 255, 0.6);} QPushButton:pressed{background-color: rgba(100, 100, 100, 0.3);} QPushButton:disabled{background-color: rgba(155, 155, 155, 0.25); border: " + QString::number(border) + "px solid rgb(155, 155, 155);}");
    SettingBigWindowTryBtn->setText(QString::fromUtf8("立即\n播放\n动画"));
    SettingBigWindowTryBtn->setEnabled(isShowBigWindow);
    font.setPixelSize(SettingBigWindowTryBtn->height() * 0.2);
    SettingBigWindowTryBtn->setFont(font);
    SettingBigWindowTryBtn->show();

    BilibiliBtn = new QPushButton(this);
    BilibiliBtn->setGeometry(this->width() * 0.1, this->height() * 12, desktop.width() * 0.465, desktop.height() * 0.05);
    borderRadius = BilibiliBtn->height();
    BilibiliBtn->setStyleSheet("QPushButton{background-color: rgba(255, 102, 153, 0.5); border-top-left-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(255, 102, 153);} QPushButton:hover{background-color: rgba(255, 102, 153, 0.6);} QPushButton:pressed{background-color: rgba(255, 102, 153, 0.2);}");
    BilibiliBtn->setText(QString::fromUtf8("哔哩哔哩：@龙ger_longer"));
    font.setPixelSize(BilibiliBtn->height() * 0.6);
    BilibiliBtn->setFont(font);
    BilibiliBtn->show();
    GithubBtn = new QPushButton(this);
    GithubBtn->setGeometry(this->width() * 0.1, this->height() * 13, desktop.width() * 0.465, desktop.height() * 0.05);
    GithubBtn->setStyleSheet("QPushButton{background-color: rgba(135, 205, 250, 0.5); border-bottom-left-radius: 0; border-bottom-left-radius: " + QString::number(borderRadius) + "px; border-bottom-right-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(135, 205, 250);} QPushButton:hover{background-color: rgba(135, 205, 250, 0.6);} QPushButton:pressed{background-color: rgba(135, 205, 250, 0.2);}");
    GithubBtn->setText(QString::fromUtf8("Github开源仓库"));
    GithubBtn->setFont(font);
    GithubBtn->show();

    ExitBtn = new QPushButton(this);
    ExitBtn->setGeometry(this->width() * 0.1, this->height() * 15, desktop.width() * 0.465, desktop.height() * 0.05);
    borderRadius = ExitBtn->height() / 2;
    ExitBtn->setStyleSheet("QPushButton{background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QPushButton:hover{background-color: rgba(255, 255, 255, 0.6);} QPushButton:pressed{background-color: rgba(235, 235, 235, 0.3);}");
    ExitBtn->setText(QString::fromUtf8("退出"));
    ExitBtn->setFont(font);
    ExitBtn->show();





    // LineEdits
    borderRadius = SettingTextSmallWindowTextLabel->height() / 2;
    SettingTextSmallWindowTextLedt = new QLineEdit(SettingTextSmallWindowTextLabel);
    SettingTextSmallWindowTextLedt->setGeometry(SettingTextSmallWindowTextLabel->width() * 0.4, 0, SettingTextSmallWindowTextLabel->width() * 0.6, SettingTextSmallWindowTextLabel->height());
    SettingTextSmallWindowTextLedt->setText(SmallWindowText);
    SettingTextSmallWindowTextLedt->setPlaceholderText(QString::fromUtf8("小窗口文本"));
    SettingTextSmallWindowTextLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); padding-left: " + QString::number(borderRadius / 2) + "px; border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
    font.setPixelSize(SettingTextSmallWindowTextLedt->height() * 0.6);
    SettingTextSmallWindowTextLedt->setFont(font);
    SettingTextSmallWindowTextLedt->show();
    SettingTextStartWindowTextLedt = new QLineEdit(SettingTextStartWindowTextLabel);
    SettingTextStartWindowTextLedt->setGeometry(SettingTextStartWindowTextLabel->width() * 0.4, 0, SettingTextStartWindowTextLabel->width() * 0.6, SettingTextStartWindowTextLabel->height());
    SettingTextStartWindowTextLedt->setText(StartWindowText);
    SettingTextStartWindowTextLedt->setPlaceholderText(QString::fromUtf8("大窗口文本"));
    SettingTextStartWindowTextLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); padding-left: " + QString::number(borderRadius / 2) + "px; border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
    SettingTextStartWindowTextLedt->setFont(font);
    SettingTextStartWindowTextLedt->show();
    SettingTextStartWindowEnglishLedt = new QLineEdit(SettingTextStartWindowEnglishLabel);
    SettingTextStartWindowEnglishLedt->setGeometry(SettingTextStartWindowEnglishLabel->width() * 0.4, 0, SettingTextStartWindowEnglishLabel->width() * 0.6, SettingTextStartWindowEnglishLabel->height());
    SettingTextStartWindowEnglishLedt->setText(StartWindowEnglishText);
    SettingTextStartWindowEnglishLedt->setPlaceholderText(QString::fromUtf8("大窗口英文"));
    SettingTextStartWindowEnglishLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); padding-left: " + QString::number(borderRadius / 2) + "px; border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
    SettingTextStartWindowEnglishLedt->setFont(font);
    SettingTextStartWindowEnglishLedt->show();
    SettingTextTimeLedt = new QLineEdit(SettingTextTimeLabel);
    SettingTextTimeLedt->setGeometry(SettingTextTimeLabel->width() * 0.4, 0, SettingTextTimeLabel->width() * 0.6, SettingTextTimeLabel->height());
    SettingTextTimeLedt->setText(targetDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    SettingTextTimeLedt->setPlaceholderText(QString::fromUtf8("终点时间"));
    SettingTextTimeLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); padding-left: " + QString::number(borderRadius / 2) + "px; border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
    SettingTextTimeLedt->setFont(font);
    SettingTextTimeLedt->show();




    // CheckBoxes
    SettingBigWindowIsShowCb = new QCheckBox(SmallWindowSettingBigWindowGb);
    SettingBigWindowIsShowCb->setGeometry(0, SmallWindowSettingBigWindowGb->height() * 0.2, SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.8 / 3);
    SettingBigWindowIsShowCb->setText(QString::fromUtf8("是否显示全屏提醒"));
    SettingBigWindowIsShowCb->setChecked(isShowBigWindow);
    SettingBigWindowIsShowCb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    font.setPixelSize(SettingBigWindowIsShowCb->height() * 0.5);
    SettingBigWindowIsShowCb->setFont(font);
    SettingBigWindowIsShowCb->show();
    SettingBigWindowIsCountdownAudioCb = new QCheckBox(SmallWindowSettingBigWindowGb);
    SettingBigWindowIsCountdownAudioCb->setGeometry(0, SmallWindowSettingBigWindowGb->height() * 0.2 + SmallWindowSettingBigWindowGb->height() * 0.8 / 3, SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.8 / 3);
    SettingBigWindowIsCountdownAudioCb->setText(QString::fromUtf8("剩余时间≤30天时是否播放倒计时提醒音"));
    SettingBigWindowIsCountdownAudioCb->setChecked(isCountdownAudio);
    SettingBigWindowIsCountdownAudioCb->setEnabled(isShowBigWindow);
    SettingBigWindowIsCountdownAudioCb->setStyleSheet("QCheckBox{background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QCheckBox:disabled{background-color: rgba(155, 155, 155, 0.25); border: " + QString::number(border) + "px solid rgb(155, 155, 155);}");
    SettingBigWindowIsCountdownAudioCb->setFont(font);
    SettingBigWindowIsCountdownAudioCb->show();
    SettingBigWindowIsHeartbeatAudioCb = new QCheckBox(SmallWindowSettingBigWindowGb);
    SettingBigWindowIsHeartbeatAudioCb->setGeometry(0, SmallWindowSettingBigWindowGb->height() * 0.2 + 2 * SmallWindowSettingBigWindowGb->height() * 0.8 / 3, SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.8 / 3);
    SettingBigWindowIsHeartbeatAudioCb->setText(QString::fromUtf8("剩余时间≤14天时是否播放心跳提醒音"));
    SettingBigWindowIsHeartbeatAudioCb->setChecked(isHeartbeatAudio);
    SettingBigWindowIsHeartbeatAudioCb->setEnabled(isShowBigWindow);
    SettingBigWindowIsHeartbeatAudioCb->setStyleSheet("QCheckBox{background-color: rgba(235, 235, 235, 0.5); border-top-right-radius: 0; border-top-left-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QCheckBox:disabled{background-color: rgba(155, 155, 155, 0.25); border: " + QString::number(border) + "px solid rgb(155, 155, 155);}");
    SettingBigWindowIsHeartbeatAudioCb->setFont(font);
    SettingBigWindowIsHeartbeatAudioCb->show();



    // ComboBoxes
    SettingLanguageCmb = new QComboBox(this);
    SettingLanguageCmb->setGeometry(this->width() * 0.1, this->height() * 17, desktop.width() * 0.465, desktop.height() * 0.05);
    SettingLanguageCmb->setCurrentIndex(0);
    borderRadius = SettingLanguageCmb->height() / 2;
    SettingLanguageCmb->setStyleSheet("QComboBox {\
                                           background-color: rgba(235, 235, 235, 0.5);\
                                           padding-left: " + QString::number(borderRadius) + "px;\
                                           border-radius: " + QString::number(borderRadius) + "px;\
                                           border: " + QString::number(border) + "px solid rgb(235, 235, 235);\
                                       }\
                                       QComboBox:focus {\
                                           background-color: rgba(135, 205, 250, 0.6);\
                                           border: " + QString::number(border) + "px solid rgb(0, 190, 255);\
                                       }\
                                       QComboBox::drop-down {\
                                           subcontrol-origin: padding;\
                                           subcontrol-position: top right;\
                                           width: " + QString::number(SettingLanguageCmb->height()) + "px;\
                                           background-color: none;\
                                           border: none;\
                                       }\
                                       QComboBox::down-arrow {\
                                           image: url(:/img/up-arrow.png);\
                                           width: " + QString::number(borderRadius) + "px;\
                                           height: " + QString::number(borderRadius) + "px;\
                                       }\
                                       QComboBox::down-arrow:on {\
                                           image: url(:/img/down-arrow.png);\
                                           width: " + QString::number(borderRadius) + "px;\
                                           height: " + QString::number(borderRadius) + "px;\
                                       }\
                                       QComboBox QAbstractItemView {\
                                           background-color: rgba(235, 235, 235, 0.5);\
                                           border: " + QString::number(border) + "px solid rgb(235, 235, 235);\
                                           outline: 0px;\
                                       }\
                                       QComboBox QAbstractItemView::item {\
                                           height: " + QString::number(SettingLanguageCmb->height()) + "px;\
                                           padding-left: " + QString::number(borderRadius) + "px;\
                                           background-color: rgba(235, 235, 235, 0.5);\
                                           border: " + QString::number(border) + "px solid rgb(235, 235, 235);\
                                       }\
                                       QComboBox QAbstractItemView::item:hover {\
                                           height: " + QString::number(SettingLanguageCmb->height()) + "px;\
                                           padding-left: " + QString::number(borderRadius) + "px;\
                                           background-color: rgba(135, 205, 250, 0.6);\
                                           border: " + QString::number(border) + "px solid rgb(0, 190, 255);\
                                       }\
                                       QComboBox QAbstractItemView::item:selected {\
                                           height: " + QString::number(SettingLanguageCmb->height()) + "px;\
                                           padding-left: " + QString::number(borderRadius) + "px;\
                                           background-color: rgba(135, 205, 250, 0.6);\
                                           border: " + QString::number(border) + "px solid rgb(0, 190, 255);\
                                       }"
    );
    font.setPixelSize(SettingLanguageCmb->height() * 0.6);
    SettingLanguageCmb->setFont(font);
    SettingLanguageCmb->setView(new QListView());
    SettingLanguageCmb->view()->setFont(font);
    SettingLanguageCmb->view()->setAttribute(Qt::WA_TranslucentBackground);
    SettingLanguageCmb->show();
    scanLanguage();




    // RadioButtons
    SettingSmallWindowOnTopRbtn = new QRadioButton(SmallWindowSettingSmallWindowOnTopOrBottomGb);
    SettingSmallWindowOnTopRbtn->setGeometry(0, SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.2, SmallWindowSettingSmallWindowOnTopOrBottomGb->width(), SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.4);
    SettingSmallWindowOnTopRbtn->setText(QString::fromUtf8("小窗口置顶"));
    SettingSmallWindowOnTopRbtn->setChecked(SmallWindowOnTopOrBottom);
    SettingSmallWindowOnTopRbtn->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
    font.setPixelSize(SettingSmallWindowOnTopRbtn->height() * 0.6);
    SettingSmallWindowOnTopRbtn->setFont(font);
    SettingSmallWindowOnTopRbtn->show();
    SettingSmallWindowOnBottomRbtn = new QRadioButton(SmallWindowSettingSmallWindowOnTopOrBottomGb);
    SettingSmallWindowOnBottomRbtn->setGeometry(0, SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.6, SmallWindowSettingSmallWindowOnTopOrBottomGb->width(), SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.4);
    SettingSmallWindowOnBottomRbtn->setText(QString::fromUtf8("小窗口置底"));
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





    selectLanguage();
    readLanguage();



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
        if (isCountdownAudio) CountdownSoundEffect->play();
        if (BlockLabelShowTimes % 2 == 0 && (timeDifference / 86400 + 1) <= 14 && isHeartbeatAudio) HeartbeatSoundEffect->play();
        });

    connect(SmallWindowMoreInfBtn, &QPushButton::clicked, [&] {
        if (!isSetting) {
            isSetting = true;
            SmallWindowCloseSettingAnimation->stop();
            SmallWindowStartSettingAnimation->setStartValue(QRect(this->x(), this->y(), this->width(), this->height()));
            SmallWindowStartSettingAnimation->start();
        }
        else {
            isSetting = false;
            SmallWindowStartSettingAnimation->stop();
            if (SmallWindowPosition == 0)
                SmallWindowCloseSettingAnimation->setEndValue(QRect(0, 0, desktop.width() * 0.3, desktop.height() * 0.05));
            else if (SmallWindowPosition == 1)
                SmallWindowCloseSettingAnimation->setEndValue(QRect(desktop.width() * 0.35, 0, desktop.width() * 0.3, desktop.height() * 0.05));
            else if (SmallWindowPosition == 2)
                SmallWindowCloseSettingAnimation->setEndValue(QRect(desktop.width() * 0.7, 0, desktop.width() * 0.3, desktop.height() * 0.05));
            SmallWindowCloseSettingAnimation->start();
        }
        });

    connect(SettingTextYesBtn, &QPushButton::clicked, [&] {
        SmallWindowText = SettingTextSmallWindowTextLedt->text();
        StartWindowText = SettingTextStartWindowTextLedt->text();
        StartWindowEnglishText = SettingTextStartWindowEnglishLedt->text();
        targetDateTime = QDateTime::fromString(SettingTextTimeLedt->text(), "yyyy-M-d h:m:ss");
        writeConfig();
        });
    connect(SettingBigWindowIsShowCb, &QCheckBox::checkStateChanged, [&] {
        isShowBigWindow = SettingBigWindowIsShowCb->isChecked();
        SettingBigWindowIsCountdownAudioCb->setEnabled(isShowBigWindow);
        SettingBigWindowIsHeartbeatAudioCb->setEnabled(isShowBigWindow);
        SettingBigWindowTryBtn->setEnabled(isShowBigWindow);
        writeConfig();
        });
    connect(SettingBigWindowIsCountdownAudioCb, &QCheckBox::checkStateChanged, [&] {
        isCountdownAudio = SettingBigWindowIsCountdownAudioCb->isChecked();
        writeConfig();
        });
    connect(SettingBigWindowIsHeartbeatAudioCb, &QCheckBox::checkStateChanged, [&] {
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
        if (AskExitMsgBox->exec() == QMessageBox::Yes) qApp->quit();
        });

    connect(SettingLanguageCmb, &QComboBox::currentTextChanged, [&] {
        changeLanguage();
        });
}

UniversalTimer::~UniversalTimer()
{
}


void UniversalTimer::updateLabel() {
    // 获取系统时间
    currentDateTime = QDateTime::currentDateTime();
    currentDateTimeString = currentDateTime.toString("yyyy-M-d h:m:ss");
    timeDifference = currentDateTime.secsTo(targetDateTime);
    // 将秒数转换为格式d天h时m分s秒
    timeDifferenceString = QString::number(timeDifference / 86400) + QString::fromUtf8(" 天 ") + QString::number((timeDifference % 86400) / 3600) + QString::fromUtf8(" 时 ") + QString::number((timeDifference % 3600) / 60) + QString::fromUtf8(" 分 ") + QString::number(timeDifference % 60) + QString::fromUtf8(" 秒");
    // 更新标签文本
    SmallWindowLabel->setText(SmallWindowText + timeDifferenceString);
    StartWindowTextLabel1->setText(StartWindowText);
    StartWindowNumberLabel->setText(QString::number(timeDifference / 86400 + 1));
    StartWindowTextLabelEnglish->setText(StartWindowEnglishText + QString::number(timeDifference / 86400 + 1) + " DAYS");
    
    // 自适应
    SmallWindowLabel->adjustSize();
    if (!isSetting) {
        SmallWindowLabel->resize(SmallWindowLabel->width() + 20, this->height());
        this->resize(SmallWindowLabel->width() + SmallWindowMoreInfBtn->width(), this->height());
        if (SmallWindowPosition == 0) {
            this->move(0, 0);
        }
        else if (SmallWindowPosition == 1) {
            this->move((desktop.width() - this->width()) / 2, 0);
        }
        else {
            this->move(desktop.width() - this->width(), 0);
        }
    }
    StartWindowNumberLabel->adjustSize();
    StartWindowTextLabel1->adjustSize();
    StartWindowTextLabel2->adjustSize();
    StartWindowTextLabel3->adjustSize();
    StartWindowTextLabelEnglish->adjustSize();
    StartWindowTextLabel1->move((desktop.width() - (StartWindowTextLabel1->width() + StartWindowNumberLabel->width() + StartWindowTextLabel3->width())) / 2, (desktop.height() - (StartWindowTextLabel1->height() + StartWindowTextLabel2->height() + StartWindowTextLabelEnglish->height())) / 2);
    StartWindowTextLabel2->move(StartWindowTextLabel1->x() + StartWindowTextLabel1->width() - StartWindowTextLabel1->height() * 2, StartWindowTextLabel1->y() + StartWindowTextLabel1->height());
    StartWindowNumberLabel->move(StartWindowTextLabel2->x() + StartWindowTextLabel2->width(), StartWindowTextLabel2->y() + StartWindowTextLabel2->height() * 1.5 - StartWindowNumberLabel->height());
    StartWindowTextLabel3->move(StartWindowNumberLabel->x() + StartWindowNumberLabel->width(), StartWindowTextLabel2->y());
    StartWindowTextLabelEnglish->move(StartWindowTextLabel2->x(), StartWindowTextLabel2->y() + StartWindowTextLabel2->height());

    if (((currentDateTime.toString("h") == "8" && currentDateTime.toString("m") == "13" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "9" && currentDateTime.toString("m") == "3" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "9" && currentDateTime.toString("m") == "53" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "10" && currentDateTime.toString("m") == "43" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "11" && currentDateTime.toString("m") == "38" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "14" && currentDateTime.toString("m") == "20" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "15" && currentDateTime.toString("m") == "23" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "16" && currentDateTime.toString("m") == "13" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "17" && currentDateTime.toString("m") == "23" && currentDateTime.toString("s") == "0") || (currentDateTime.toString("h") == "18" && currentDateTime.toString("m") == "6" && currentDateTime.toString("s") == "0")) && isShowBigWindow) {
        startShowBigWindowAnimation();
    }
}

void UniversalTimer::readConfig() {
    QFile file("config.ini");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        all = in.readAll();
        line = all.split("\n");
        for (int i = 0; i < line.size(); i++) {
            list = line[i].split("=");
            if (list[0] == "ConfigVersion")
                ConfigVersion = list[1];
            else if (list[0] == "Language")
                Language = list[1];
            else if (list[0] == "SmallWindowText")
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
        QMessageBox::critical(NULL, QString::fromUtf8("错误 Error"), QString::fromUtf8("未找到配置文件，将使用默认配置。\nNo config file is found, the default config will be used."));
        ConfigVersion = RightConfigVersion;
        writeConfig();
    }
}
void UniversalTimer::writeConfig() {
    QFile file("config.ini");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "ConfigVersion=" << RightConfigVersion << "\n";
        out << "Language=" << Language << "\n";
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


void UniversalTimer::scanLanguage() {
    // 检测存在的语言
    if (QFile::exists("lang/ar/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("العربية"));
    }
    if (QFile::exists("lang/az/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Azərbaycan"));
    }
    if (QFile::exists("lang/be/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Беларуская"));
    }
    if (QFile::exists("lang/bg/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Български"));
    }
    if (QFile::exists("lang/ca/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Català"));
    }
    if (QFile::exists("lang/cs/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Čeština"));
    }
    if (QFile::exists("lang/cy/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Cymraeg"));
    }
    if (QFile::exists("lang/da/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Dansk"));
    }
    if (QFile::exists("lang/de/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Deutsch"));
    }
    if (QFile::exists("lang/el/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Ελληνικά"));
    }
    if (QFile::exists("lang/en/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("English"));
    }
    if (QFile::exists("lang/es/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Español"));
    }
    if (QFile::exists("lang/et/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Eesti"));
    }
    if (QFile::exists("lang/eu/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Euskara"));
    }
    if (QFile::exists("lang/fa/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("فارسی"));
    }
    if (QFile::exists("lang/fi/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Suomi"));
    }
    if (QFile::exists("lang/fo/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Føroyskt"));
    }
    if (QFile::exists("lang/fr/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Français"));
    }
    if (QFile::exists("lang/gl/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Galego"));
    }
    if (QFile::exists("lang/he/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("עברית"));
    }
    if (QFile::exists("lang/hi/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("हिन्दी"));
    }
    if (QFile::exists("lang/hr/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Hrvatski"));
    }
    if (QFile::exists("lang/hu/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Magyar"));
    }
    if (QFile::exists("lang/hy/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Հայերեն"));
    }
    if (QFile::exists("lang/id/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Bahasa Indonesia"));
    }
    if (QFile::exists("lang/is/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Íslenska"));
    }
    if (QFile::exists("lang/it/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Italiano"));
    }
    if (QFile::exists("lang/ja/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("日本語"));
    }
    if (QFile::exists("lang/ka/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("ქართული"));
    }
    if (QFile::exists("lang/kk/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Қазақша"));
    }
    if (QFile::exists("lang/kn/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("ಕನ್ನಡ"));
    }
    if (QFile::exists("lang/ko/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("한국어"));
    }
    if (QFile::exists("lang/lt/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Lietuvių"));
    }
    if (QFile::exists("lang/lv/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Latviešu"));
    }
    if (QFile::exists("lang/mi/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Māori"));
    }
    if (QFile::exists("lang/mk/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Македонски"));
    }
    if (QFile::exists("lang/mn/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Монгол"));
    }
    if (QFile::exists("lang/mr/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("मराठी"));
    }
    if (QFile::exists("lang/ms/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Bahasa Melayu"));
    }
    if (QFile::exists("lang/mt/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Malti"));
    }
    if (QFile::exists("lang/nb/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Norsk (bokmål)"));
    }
    if (QFile::exists("lang/nl/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Nederlands"));
    }
    if (QFile::exists("lang/nn/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Norsk (nynorsk)"));
    }
    if (QFile::exists("lang/pa/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("ਪੰਜਾਬੀ"));
    }
    if (QFile::exists("lang/pl/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Polski"));
    }
    if (QFile::exists("lang/pt/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Português"));
    }
    if (QFile::exists("lang/qu/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Runa Simi"));
    }
    if (QFile::exists("lang/ro/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Română"));
    }
    if (QFile::exists("lang/ru/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Русский"));
    }
    if (QFile::exists("lang/sa/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("संस्कृतम्"));
    }
    if (QFile::exists("lang/se/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Sámegiella"));
    }
    if (QFile::exists("lang/sk/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Slovenčina"));
    }
    if (QFile::exists("lang/sl/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Slovenščina"));
    }
    if (QFile::exists("lang/sq/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Shqip"));
    }
    if (QFile::exists("lang/sr/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Српски"));
    }
    if (QFile::exists("lang/sv/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Svenska"));
    }
    if (QFile::exists("lang/sw/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Kiswahili"));
    }
    if (QFile::exists("lang/syr/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("ܣܘܪܝܝܐ"));
    }
    if (QFile::exists("lang/ta/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("தமிழ்"));
    }
    if (QFile::exists("lang/te/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("తెలుగు"));
    }
    if (QFile::exists("lang/th/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("ไทย"));
    }
    if (QFile::exists("lang/tl/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Tagalog"));
    }
    if (QFile::exists("lang/tn/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Setswana"));
    }
    if (QFile::exists("lang/tr/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Türkçe"));
    }
    if (QFile::exists("lang/ts/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Xitsonga"));
    }
    if (QFile::exists("lang/tt/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Татарча"));
    }
    if (QFile::exists("lang/uk/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Українська"));
    }
    if (QFile::exists("lang/ur/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("اردو"));
    }
    if (QFile::exists("lang/uz/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Oʻzbekcha"));
    }
    if (QFile::exists("lang/vi/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("Tiếng Việt"));
    }
    if (QFile::exists("lang/xh/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("isiXhosa"));
    }
    if (QFile::exists("lang/zh-CN/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("简体中文"));
    }
    if (QFile::exists("lang/zh-TW/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("繁體中文"));
    }
    if (QFile::exists("lang/zh-Literary-CN/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("文言(华夏)(简体)"));
    }
    if (QFile::exists("lang/zh-Literary-TW/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("文言(華夏)(繁體)"));
    }
    if (QFile::exists("lang/zu/main.lang")) {
        SettingLanguageCmb->addItem(QString::fromUtf8("isiZulu"));
    }
}
void UniversalTimer::selectLanguage() {
    if (Language == "ar") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("العربية"));
    }
    else if (Language == "az") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Azərbaycan"));
    }
    else if (Language == "be") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Беларуская"));
    }
    else if (Language == "bg") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Български"));
    }
    else if (Language == "ca") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Català"));
    }
    else if (Language == "cs") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Čeština"));
    }
    else if (Language == "cy") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Cymraeg"));
    }
    else if (Language == "da") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Dansk"));
    }
    else if (Language == "de") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Deutsch"));
    }
    else if (Language == "el") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Ελληνικά"));
    }
    else if (Language == "en") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("English"));
    }
    else if (Language == "es") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Español"));
    }
    else if (Language == "et") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Eesti"));
    }
    else if (Language == "eu") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Euskara"));
    }
    else if (Language == "fa") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("فارسی"));
    }
    else if (Language == "fi") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Suomi"));
    }
    else if (Language == "fo") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Føroyskt"));
    }
    else if (Language == "fr") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Français"));
    }
    else if (Language == "gl") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Galego"));
    }
    else if (Language == "he") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("עברית"));
    }
    else if (Language == "hi") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("हिन्दी"));
    }
    else if (Language == "hr") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Hrvatski"));
    }
    else if (Language == "hu") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Magyar"));
    }
    else if (Language == "hy") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Հայերեն"));
    }
    else if (Language == "id") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Bahasa Indonesia"));
    }
    else if (Language == "is") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Íslenska"));
    }
    else if (Language == "it") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Italiano"));
    }
    else if (Language == "ja") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("日本語"));
    }
    else if (Language == "ka") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("ქართული"));
    }
    else if (Language == "kk") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Қазақша"));
    }
    else if (Language == "kn") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("ಕನ್ನಡ"));
    }
    else if (Language == "ko") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("한국어"));
    }
    else if (Language == "lt") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Lietuvių"));
    }
    else if (Language == "lv") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Latviešu"));
    }
    else if (Language == "mi") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Māori"));
    }
    else if (Language == "mk") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Македонски"));
    }
    else if (Language == "mn") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Монгол"));
    }
    else if (Language == "mr") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("मराठी"));
    }
    else if (Language == "ms") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Bahasa Melayu"));
    }
    else if (Language == "mt") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Malti"));
    }
    else if (Language == "nb") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Norsk (bokmål)"));
    }
    else if (Language == "nl") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Nederlands"));
    }
    else if (Language == "nn") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Norsk (nynorsk)"));
    }
    else if (Language == "pa") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("ਪੰਜਾਬੀ"));
    }
    else if (Language == "pl") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Polski"));
    }
    else if (Language == "pt") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Português"));
    }
    else if (Language == "qu") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Runa Simi"));
    }
    else if (Language == "ro") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Română"));
    }
    else if (Language == "ru") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Русский"));
    }
    else if (Language == "sa") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("संस्कृतम्"));
    }
    else if (Language == "se") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Sámegiella"));
    }
    else if (Language == "sk") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Slovenčina"));
    }
    else if (Language == "sl") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Slovenščina"));
    }
    else if (Language == "sq") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Shqip"));
    }
    else if (Language == "sr") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Српски"));
    }
    else if (Language == "sv") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Svenska"));
    }
    else if (Language == "sw") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Kiswahili"));
    }
    else if (Language == "syr") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("ܣܘܪܝܝܐ"));
    }
    else if (Language == "ta") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("தமிழ்"));
    }
    else if (Language == "te") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("తెలుగు"));
    }
    else if (Language == "th") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("ไทย"));
    }
    else if (Language == "tl") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Tagalog"));
    }
    else if (Language == "tn") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Setswana"));
    }
    else if (Language == "tr") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Türkçe"));
    }
    else if (Language == "ts") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Xitsonga"));
    }
    else if (Language == "tt") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Татарча"));
    }
    else if (Language == "uk") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Українська"));
    }
    else if (Language == "ur") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("اردو"));
    }
    else if (Language == "uz") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Oʻzbekcha"));
    }
    else if (Language == "vi") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("Tiếng Việt"));
    }
    else if (Language == "xh") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("isiXhosa"));
    }
    else if (Language == "zh-CN") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("简体中文"));
    }
    else if (Language == "zh-TW") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("繁體中文"));
    }
    else if (Language == "zh-Literary-CN") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("文言(华夏)(简体)"));
    }
    else if (Language == "zh-Literary-TW") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("文言(華夏)(繁體)"));
    }
    else if (Language == "zu") {
        SettingLanguageCmb->setCurrentText(QString::fromUtf8("isiZulu"));
    }
}
void UniversalTimer::changeLanguage() {
    // 判断选择了哪个语言
    if (SettingLanguageCmb->currentText() == QString::fromUtf8("العربية")) {
        Language = "ar";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Azərbaycan")) {
        Language = "az";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Беларуская")) {
        Language = "be";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Български")) {
        Language = "bg";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Català")) {
        Language = "ca";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Čeština")) {
        Language = "cs";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Cymraeg")) {
        Language = "cy";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Dansk")) {
        Language = "da";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Deutsch")) {
        Language = "de";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Ελληνικά")) {
        Language = "el";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("English")) {
        Language = "en";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Español")) {
        Language = "es";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Eesti")) {
        Language = "et";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Euskara")) {
        Language = "eu";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("فارسی")) {
        Language = "fa";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Suomi")) {
        Language = "fi";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Føroyskt")) {
        Language = "fo";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Français")) {
        Language = "fr";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Galego")) {
        Language = "gl";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("עברית")) {
        Language = "he";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("हिन्दी")) {
        Language = "hi";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Hrvatski")) {
        Language = "hr";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Magyar")) {
        Language = "hu";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Հայերեն")) {
        Language = "hy";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Bahasa Indonesia")) {
        Language = "id";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Íslenska")) {
        Language = "is";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Italiano")) {
        Language = "it";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("日本語")) {
        Language = "ja";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("ქართული")) {
        Language = "ka";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Қазақша")) {
        Language = "kk";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("ಕನ್ನಡ")) {
        Language = "kn";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("한국어")) {
        Language = "ko";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Lietuvių")) {
        Language = "lt";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Latviešu")) {
        Language = "lv";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Māori")) {
        Language = "mi";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Македонски")) {
        Language = "mk";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Монгол")) {
        Language = "mn";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("मराठी")) {
        Language = "mr";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Bahasa Melayu")) {
        Language = "ms";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Malti")) {
        Language = "mt";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Norsk (bokmål)")) {
        Language = "nb";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Nederlands")) {
        Language = "nl";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Norsk (nynorsk)")) {
        Language = "nn";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("ਪੰਜਾਬੀ")) {
        Language = "pa";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Polski")) {
        Language = "pl";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Português")) {
        Language = "pt";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Runa Simi")) {
        Language = "qu";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Română")) {
        Language = "ro";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Русский")) {
        Language = "ru";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("संस्कृतम्")) {
        Language = "sa";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Sámegiella")) {
        Language = "se";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Slovenčina")) {
        Language = "sk";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Slovenščina")) {
        Language = "sl";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Shqip")) {
        Language = "sq";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Српски")) {
        Language = "sr";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Svenska")) {
        Language = "sv";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Kiswahili")) {
        Language = "sw";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("ܣܘܪܝܝܐ")) {
        Language = "syr";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("தமிழ்")) {
        Language = "ta";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("తెలుగు")) {
        Language = "te";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("ไทย")) {
        Language = "th";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Tagalog")) {
        Language = "tl";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Setswana")) {
        Language = "tn";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Türkçe")) {
        Language = "tr";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Xitsonga")) {
        Language = "ts";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Татарча")) {
        Language = "tt";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Українська")) {
        Language = "uk";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("اردو")) {
        Language = "ur";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Oʻzbekcha")) {
        Language = "uz";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("Tiếng Việt")) {
        Language = "vi";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("isiXhosa")) {
        Language = "xh";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("简体中文")) {
        Language = "zh-CN";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("繁體中文")) {
        Language = "zh-TW";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("文言(华夏)(简体)")) {
        Language = "zh-Literary-CN";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("文言(華夏)(繁體)")) {
        Language = "zh-Literary-TW";
    }
    else if (SettingLanguageCmb->currentText() == QString::fromUtf8("isiZulu")) {
        Language = "zu";
    }
    readLanguage();
    writeConfig();
}

void UniversalTimer::readLanguage() {
    QFile file("lang/" + Language + "/main.lang");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        all = in.readAll();
        line = all.split("\n");
        for (int i = 0; i < line.size(); i++) {
            list = line[i].split("=");
            if (list[0] == "LanguageVersion")
                LanguageVersion = list[1];
            else if (list[0] == "SmallWindowSettingTextGb_text")
                SmallWindowSettingTextGb->setTitle(list[1]);
            else if (list[0] == "SmallWindowSettingBigWindowGb_text")
                SmallWindowSettingBigWindowGb->setTitle(list[1]);
            else if (list[0] == "SmallWindowSettingSmallWindowOnTopOrBottomGb_text")
                SmallWindowSettingSmallWindowOnTopOrBottomGb->setTitle(list[1]);
            else if (list[0] == "SettingTextSmallWindowTextLabel_text")
                SettingTextSmallWindowTextLabel->setText(list[1]);
            else if (list[0] == "SettingTextStartWindowTextLabel_text")
                SettingTextStartWindowTextLabel->setText(list[1]);
            else if (list[0] == "SettingTextStartWindowEnglishLabel_text")
                SettingTextStartWindowEnglishLabel->setText(list[1]);
            else if (list[0] == "SettingTextTimeLabel_text")
                SettingTextTimeLabel->setText(list[1]);
            else if (list[0] == "SettingTextYesBtn_text")
                SettingTextYesBtn->setText(list[1]);
            else if (list[0] == "SettingBigWindowIsShowCb_text")
                SettingBigWindowIsShowCb->setText(list[1]);
            else if (list[0] == "SettingBigWindowIsCountdownAudioCb_text")
                SettingBigWindowIsCountdownAudioCb->setText(list[1]);
            else if (list[0] == "SettingBigWindowIsHeartbeatAudioCb_text")
                SettingBigWindowIsHeartbeatAudioCb->setText(list[1]);
            else if (list[0] == "SettingBigWindowTryBtn_text")
                SettingBigWindowTryBtn->setText(list[1]);
            else if (list[0] == "SettingSmallWindowOnTopRbtn_text")
                SettingSmallWindowOnTopRbtn->setText(list[1]);
            else if (list[0] == "SettingSmallWindowOnBottomRbtn_text")
                SettingSmallWindowOnBottomRbtn->setText(list[1]);
            else if (list[0] == "BilibiliBtn_text")
                BilibiliBtn->setText(list[1] + QString::fromUtf8(": @龙ger_longer"));
            else if (list[0] == "GithubBtn_text")
                GithubBtn->setText(list[1]);
            else if (list[0] == "ExitBtn_text")
                ExitBtn->setText(list[1]);
            else if (list[0] == "AskExitMsgBox_title")
                AskExitMsgBox->setWindowTitle(list[1]);
            else if (list[0] == "AskExitMsgBox_text")
                AskExitMsgBox->setText(list[1]);
        }
        file.close();
    }
    /*if (LanguageVersion != RightLanguageVersion) {
        QMessageBox::warning(this, "警告 Warning", "选择的语言版本不正确，可能导致程序异常。若要获取正确的版本，请前往官网或者GitHub获取正确版本。\nThe selected language version is incorrect, which may cause the program to malfunction. If you want to get the correct version, please go to the official website or GitHub to get the correct version." + LanguageVersion + RightLanguageVersion);
    }*/
}



void UniversalTimer::startShowBigWindowAnimation() {
    StartWindow->show();
    StartWindowAnimationGroup->start();
    if (isSetting) SmallWindowCloseSettingAnimation->start();
    SmallWindowCloseOpacityAnimation->start();
    isSetting = false;
}




void UniversalTimer::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    SmallWindowMoveAnimation->stop();
    this->startX = event->pos().x();
    this->startY = event->pos().y();
}

void UniversalTimer::mouseMoveEvent(QMouseEvent* event) {
    QWidget::mouseMoveEvent(event);
    float disX = event->pos().x() - this->startX;
    float disY = event->pos().y() - this->startY;
    if (!isSetting) this->move(this->x() + disX, this->y() + disY);
}

void UniversalTimer::mouseReleaseEvent(QMouseEvent* event) {
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

void UniversalTimer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    SmallWindowUnderlyingLabel->setGeometry(0, 0, this->width(), this->height());
    SmallWindowMoreInfBtn->setGeometry(this->width() - desktop.height() * 0.05, 0, desktop.height() * 0.05, desktop.height() * 0.05);
}