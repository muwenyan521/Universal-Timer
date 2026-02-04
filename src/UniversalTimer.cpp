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
#include <QDebug>
#include <QApplication>



UniversalTimer::UniversalTimer(QWidget* parent)
	: QWidget(parent)
	, langSettings(nullptr)
	, currentLangCode("zh_cn")
	, CountdownSoundEffect(nullptr)
	, HeartbeatSoundEffect(nullptr)
	, NumberLabelOpacityEffect(nullptr)
	, StartWindowBlockLabel1OpacityEffect(nullptr)
	, StartWindowBlockLabel2OpacityEffect(nullptr)
	, StartWindowBlockLabel3OpacityEffect(nullptr)
	, StartWindowBlockLabel4OpacityEffect(nullptr)
	, SmallWindowStartAnimation(nullptr)
	, SmallWindowStartOpacityAnimation(nullptr)
	, SmallWindowCloseOpacityAnimation(nullptr)
	, StartWindowColorLabelAnimation1(nullptr)
	, StartWindowColorLabelAnimation2(nullptr)
	, StartWindowTextColorLabelAnimation(nullptr)
	, StartWindowNumberLabelOpacityAnimation1(nullptr)
	, StartWindowNumberLabelOpacityAnimation2(nullptr)
	, StartWindowNumberLabelOpacityAnimation3(nullptr)
	, StartWindowNumberLabelOpacityAnimation4(nullptr)
	, StartWindowNumberLabelOpacityAnimation5(nullptr)
	, StartWindowBigWindowCloseOpacityAnimation(nullptr)
	, StartWindowBlockLabel1OpacityAnimation1(nullptr)
	, StartWindowBlockLabel1OpacityAnimation2(nullptr)
	, StartWindowBlockLabel1OpacityAnimation3(nullptr)
	, StartWindowBlockLabel1OpacityAnimation4(nullptr)
	, StartWindowBlockLabel2OpacityAnimation1(nullptr)
	, StartWindowBlockLabel2OpacityAnimation2(nullptr)
	, StartWindowBlockLabel2OpacityAnimation3(nullptr)
	, StartWindowBlockLabel2OpacityAnimation4(nullptr)
	, StartWindowBlockLabel3OpacityAnimation1(nullptr)
	, StartWindowBlockLabel3OpacityAnimation2(nullptr)
	, StartWindowBlockLabel3OpacityAnimation3(nullptr)
	, StartWindowBlockLabel3OpacityAnimation4(nullptr)
	, StartWindowBlockLabel4OpacityAnimation1(nullptr)
	, StartWindowBlockLabel4OpacityAnimation2(nullptr)
	, StartWindowBlockLabel4OpacityAnimation3(nullptr)
	, StartWindowBlockLabel4OpacityAnimation4(nullptr)
	, SmallWindowMoveAnimation(nullptr)
	, SmallWindowStartSettingAnimation(nullptr)
	, SmallWindowUnderlyingLabelStartSettingAnimation(nullptr)
	, SmallWindowCloseSettingAnimation(nullptr)
	, SmallWindowUnderlyingLabelCloseSettingAnimation(nullptr)
	, StartWindowAnimationGroup(nullptr)
	, StartWindowNumberLabelAnimationGroup(nullptr)
	, StartWindowBlockLabel1AnimationGroup(nullptr)
	, StartWindowBlockLabel2AnimationGroup(nullptr)
	, StartWindowBlockLabel3AnimationGroup(nullptr)
	, StartWindowBlockLabel4AnimationGroup(nullptr)
	, SmallWindowStartAnimationGroup(nullptr)
	, StartWindow(nullptr)
	, AskExitMsgBox(nullptr)
	, SmallWindowSettingTextGb(nullptr)
	, SmallWindowSettingBigWindowGb(nullptr)
	, SmallWindowSettingSmallWindowOnTopOrBottomGb(nullptr)
	, SmallWindowMoreInfBtn(nullptr)
	, SettingTextYesBtn(nullptr)
	, SettingBigWindowTryBtn(nullptr)
	, BilibiliBtn(nullptr)
	, GithubBtn(nullptr)
	, ExitBtn(nullptr)
	, SettingBigWindowIsShowCb(nullptr)
	, SettingBigWindowIsCountdownAudioCb(nullptr)
	, SettingBigWindowIsHeartbeatAudioCb(nullptr)
	, SettingSmallWindowOnTopRbtn(nullptr)
	, SettingSmallWindowOnBottomRbtn(nullptr)
	, SettingLanguageCmb(nullptr)
	, SmallWindowUnderlyingLabel(nullptr)
	, SmallWindowLabel(nullptr)
	, SmallWindowLogoLabel(nullptr)
	, StartWindowUnderlyingLabel(nullptr)
	, StartWindowColorLabel(nullptr)
	, StartWindowTextColorLabel(nullptr)
	, StartWindowTextLabel1(nullptr)
	, StartWindowTextLabel2(nullptr)
	, StartWindowTextLabel3(nullptr)
	, StartWindowNumberLabel(nullptr)
	, StartWindowTextLabelEnglish(nullptr)
	, StartWindowBlockLabel1(nullptr)
	, StartWindowBlockLabel2(nullptr)
	, StartWindowBlockLabel3(nullptr)
	, StartWindowBlockLabel4(nullptr)
	, SettingTextSmallWindowTextLabel(nullptr)
	, SettingTextStartWindowTextLabel(nullptr)
	, SettingTextStartWindowEnglishLabel(nullptr)
	, SettingTextTimeLabel(nullptr)
	, SettingTextSmallWindowTextLedt(nullptr)
	, SettingTextStartWindowTextLedt(nullptr)
	, SettingTextStartWindowEnglishLedt(nullptr)
	, SettingTextTimeLedt(nullptr)
	, startX(0)
	, startY(0)
{
	// ============== 第一步：基本设置 ==============
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

	SmallWindowText = QString::fromUtf8("会考倒计时：");
	StartWindowText = QString::fromUtf8("距会考");
	StartWindowEnglishText = "THE EXAM IN ";

	RightConfigVersion = "1.3.0.0";
	ConfigVersion = "none";

	// 获取系统时间
	currentDateTime = QDateTime::currentDateTime();
	currentDateTimeString = currentDateTime.toString("yyyy-M-d h:m:ss");
	targetDateTime = QDateTime::fromString("2025-6-30 0:0:00", "yyyy-M-d h:m:ss");
	timeDifference = currentDateTime.secsTo(targetDateTime);
	timeDifferenceString = QString::number(timeDifference / 86400) + QString::fromUtf8(" 天 ") + QString::number((timeDifference % 86400) / 3600) + QString::fromUtf8(" 时 ") + QString::number((timeDifference % 3600) / 60) + QString::fromUtf8(" 分 ") + QString::number(timeDifference % 60) + QString::fromUtf8(" 秒");

	// TimeList初始化
	TimeList = { QTime::fromString("8:13:00", "h:mm:ss"), QTime::fromString("9:03:00", "h:mm:ss"), QTime::fromString("9:53:00", "h:mm:ss"), QTime::fromString("10:43:00", "h:mm:ss"), QTime::fromString("11:38:00", "h:mm:ss"), QTime::fromString("14:20:00", "h:mm:ss"), QTime::fromString("15:23:00", "h:mm:ss"), QTime::fromString("16:13:00", "h:mm:ss"), QTime::fromString("17:03:00", "h:mm:ss"), QTime::fromString("18:06:00", "h:mm:ss") };

	// ============== 第二步：读取配置 ==============
	readConfig();

	if (ConfigVersion != RightConfigVersion) {
		if (QMessageBox::warning(NULL, QString::fromUtf8("万能倒计时 - 警告"), QString::fromUtf8("配置文件版本不匹配，可能导致程序运行不正常，是否继续？\n请删除版本不匹配的配置文件，重新启动程序将生成符合版本的默认配置文件。\n(点击'取消'将退出程序)"), QMessageBox::Yes | QMessageBox::Cancel) != QMessageBox::Yes) exit(0);
	}

	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | (SmallWindowOnTopOrBottom ? Qt::WindowStaysOnTopHint : Qt::WindowStaysOnBottomHint));

	SmallWindowPosition = 1;
	isSetting = false;
	isCountdownAudio = true;
	isHeartbeatAudio = true;
	isShowBigWindow = true;
	SmallWindowOnTopOrBottom = true;
	LogoPixelMultiplier = 2682.0 / 2048.0;

	// ============== 第三步：创建声音效果 ==============
	CountdownSoundEffect = new QSoundEffect(this);
	CountdownSoundEffect->setSource(QUrl::fromLocalFile("sounds/countdown.wav"));
	HeartbeatSoundEffect = new QSoundEffect(this);
	HeartbeatSoundEffect->setSource(QUrl::fromLocalFile("sounds/heartbeat.wav"));

	// ============== 第四步：初始化透明度效果 ==============
	NumberLabelOpacityEffect = new QGraphicsOpacityEffect();
	StartWindowBlockLabel1OpacityEffect = new QGraphicsOpacityEffect();
	StartWindowBlockLabel2OpacityEffect = new QGraphicsOpacityEffect();
	StartWindowBlockLabel3OpacityEffect = new QGraphicsOpacityEffect();
	StartWindowBlockLabel4OpacityEffect = new QGraphicsOpacityEffect();

	// ============== 第五步：创建所有控件 ==============
	// 注意：这里只创建控件，不设置语言相关文本

	// Widgets
	StartWindow = new QWidget;
	StartWindow->setGeometry(0, 0, desktop.width(), desktop.height());
	StartWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	StartWindow->setAttribute(Qt::WA_TranslucentBackground);
	StartWindow->setStyleSheet("font-family: DIN1451, zihun59hao-chuangcuhei, Microsoft Yahei UI;");
	if (isShowBigWindow) StartWindow->show();

	// MessageBoxes
	AskExitMsgBox = new QMessageBox(this);
	AskExitMsgBox->setWindowTitle("Universal Timer - Prompt");  // 英文默认
	AskExitMsgBox->setText("Are you sure you want to quit?");
	AskExitMsgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	AskExitMsgBox->setDefaultButton(QMessageBox::No);
	AskExitMsgBox->setIcon(QMessageBox::Question);
	AskExitMsgBox->setWindowOpacity(0.85);

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

	// StartWindow 相关 Labels
	StartWindowUnderlyingLabel = new QLabel(StartWindow);
	StartWindowUnderlyingLabel->setGeometry(0, 0, desktop.width(), desktop.height());
	StartWindowUnderlyingLabel->setStyleSheet("background-color: rgba(0, 0, 0, 0.75);");
	StartWindowUnderlyingLabel->hide();

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

	// 设置透明度效果
	NumberLabelOpacityEffect->setOpacity(0.05);
	StartWindowBlockLabel1OpacityEffect->setOpacity(0.05);
	StartWindowBlockLabel2OpacityEffect->setOpacity(0.05);
	StartWindowBlockLabel3OpacityEffect->setOpacity(0.05);
	StartWindowBlockLabel4OpacityEffect->setOpacity(0.05);

	// GroupBoxes
	borderRadius = this->height() / 4;
	border = this->height() * 0.025;
	font.setPixelSize(this->height() * 0.1);

	SmallWindowSettingTextGb = new QGroupBox(this);
	SmallWindowSettingTextGb->setGeometry(this->width() * 0.1, this->height() * 2, desktop.width() * 0.465, desktop.height() * 0.2);
	SmallWindowSettingTextGb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235); ");
	SmallWindowSettingTextGb->setTitle("Text and Time");  // 英文默认
	SmallWindowSettingTextGb->show();

	SmallWindowSettingBigWindowGb = new QGroupBox(this);
	SmallWindowSettingBigWindowGb->setGeometry(this->width() * 0.1, this->height() * 6.5, desktop.width() * 0.465, desktop.height() * 0.1);
	SmallWindowSettingBigWindowGb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235); ");
	SmallWindowSettingBigWindowGb->setTitle("Full Screen Reminder");  // 英文默认
	SmallWindowSettingBigWindowGb->show();

	SmallWindowSettingSmallWindowOnTopOrBottomGb = new QGroupBox(this);
	SmallWindowSettingSmallWindowOnTopOrBottomGb->setGeometry(this->width() * 0.1, this->height() * 9, desktop.width() * 0.465, desktop.height() * 0.075);
	SmallWindowSettingSmallWindowOnTopOrBottomGb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235); ");
	SmallWindowSettingSmallWindowOnTopOrBottomGb->setTitle("Small Window Level");  // 英文默认
	SmallWindowSettingSmallWindowOnTopOrBottomGb->show();

	// Labels in GroupBoxes
	SettingTextSmallWindowTextLabel = new QLabel(SmallWindowSettingTextGb);
	SettingTextSmallWindowTextLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.1, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
	SettingTextSmallWindowTextLabel->setText("Small window text:");  // 英文默认
	SettingTextSmallWindowTextLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
	font.setPixelSize(SettingTextSmallWindowTextLabel->height() * 0.5);
	SettingTextSmallWindowTextLabel->setFont(font);
	SettingTextSmallWindowTextLabel->show();

	SettingTextStartWindowTextLabel = new QLabel(SmallWindowSettingTextGb);
	SettingTextStartWindowTextLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.275, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
	SettingTextStartWindowTextLabel->setText("Large window text:");  // 英文默认
	SettingTextStartWindowTextLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
	SettingTextStartWindowTextLabel->setFont(font);
	SettingTextStartWindowTextLabel->show();

	SettingTextStartWindowEnglishLabel = new QLabel(SmallWindowSettingTextGb);
	SettingTextStartWindowEnglishLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.45, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
	SettingTextStartWindowEnglishLabel->setText("Large window English:");  // 英文默认
	SettingTextStartWindowEnglishLabel->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
	SettingTextStartWindowEnglishLabel->setFont(font);
	SettingTextStartWindowEnglishLabel->show();

	SettingTextTimeLabel = new QLabel(SmallWindowSettingTextGb);
	SettingTextTimeLabel->setGeometry(0, SmallWindowSettingTextGb->height() * 0.625, SmallWindowSettingTextGb->width(), SmallWindowSettingTextGb->height() * 0.175);
	SettingTextTimeLabel->setText("Target time:");  // 英文默认
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
	SettingTextYesBtn->setText("Save");  // 英文默认
	font.setPixelSize(SettingTextYesBtn->height() * 0.6);
	SettingTextYesBtn->setFont(font);
	SettingTextYesBtn->show();

	SettingBigWindowTryBtn = new QPushButton(SmallWindowSettingBigWindowGb);
	SettingBigWindowTryBtn->setGeometry(SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.2, SmallWindowSettingBigWindowGb->width() * 0.2, SmallWindowSettingBigWindowGb->height() * 0.8);
	SettingBigWindowTryBtn->setStyleSheet("QPushButton{background-color: rgba(235, 235, 235, 0.5); border-top-right-radius: 0; border-top-left-radius: 0; border-bottom-left-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QPushButton:hover{background-color: rgba(255, 255, 255, 0.6);} QPushButton:pressed{background-color: rgba(100, 100, 100, 0.3);} QPushButton:disabled{background-color: rgba(155, 155, 155, 0.25); border: " + QString::number(border) + "px solid rgb(155, 155, 155);}");
	SettingBigWindowTryBtn->setText("Play\nAnimation\nNow");  // 英文默认
	SettingBigWindowTryBtn->setEnabled(isShowBigWindow);
	font.setPixelSize(SettingBigWindowTryBtn->height() * 0.2);
	SettingBigWindowTryBtn->setFont(font);
	SettingBigWindowTryBtn->show();

	BilibiliBtn = new QPushButton(this);
	BilibiliBtn->setGeometry(this->width() * 0.1, this->height() * 12, desktop.width() * 0.465, desktop.height() * 0.05);
	borderRadius = BilibiliBtn->height();
	BilibiliBtn->setStyleSheet("QPushButton{background-color: rgba(255, 102, 153, 0.5); border-top-left-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(255, 102, 153);} QPushButton:hover{background-color: rgba(255, 102, 153, 0.6);} QPushButton:pressed{background-color: rgba(255, 102, 153, 0.2);}");
	BilibiliBtn->setText("Bilibili: @龙ger_longer");
	font.setPixelSize(BilibiliBtn->height() * 0.6);
	BilibiliBtn->setFont(font);
	BilibiliBtn->show();

	GithubBtn = new QPushButton(this);
	GithubBtn->setGeometry(this->width() * 0.1, this->height() * 13, desktop.width() * 0.465, desktop.height() * 0.05);
	GithubBtn->setStyleSheet("QPushButton{background-color: rgba(135, 205, 250, 0.5); border-bottom-left-radius: 0; border-bottom-left-radius: " + QString::number(borderRadius) + "px; border-bottom-right-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(135, 205, 250);} QPushButton:hover{background-color: rgba(135, 205, 250, 0.6);} QPushButton:pressed{background-color: rgba(135, 205, 250, 0.2);}");
	GithubBtn->setText("GitHub Open Source Repository");  // 英文默认
	GithubBtn->setFont(font);
	GithubBtn->show();

	ExitBtn = new QPushButton(this);
	ExitBtn->setGeometry(this->width() * 0.1, this->height() * 15, desktop.width() * 0.465, desktop.height() * 0.05);
	borderRadius = ExitBtn->height() / 2;
	ExitBtn->setStyleSheet("QPushButton{background-color: rgba(235, 235, 235, 0.5); border-radius: " + QString::number(borderRadius) + "px; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QPushButton:hover{background-color: rgba(255, 255, 255, 0.6);} QPushButton:pressed{background-color: rgba(235, 235, 235, 0.3);}");
	ExitBtn->setText("Exit");  // 英文默认
	ExitBtn->setFont(font);
	ExitBtn->show();

	// LineEdits
	borderRadius = SettingTextSmallWindowTextLabel->height() / 2;
	SettingTextSmallWindowTextLedt = new QLineEdit(SettingTextSmallWindowTextLabel);
	SettingTextSmallWindowTextLedt->setGeometry(SettingTextSmallWindowTextLabel->width() * 0.4, 0, SettingTextSmallWindowTextLabel->width() * 0.6, SettingTextSmallWindowTextLabel->height());
	SettingTextSmallWindowTextLedt->setText(SmallWindowText);
	SettingTextSmallWindowTextLedt->setPlaceholderText("Small window text");  // 英文默认
	SettingTextSmallWindowTextLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); padding-left: " + QString::number(borderRadius / 2) + "px; border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
	font.setPixelSize(SettingTextSmallWindowTextLedt->height() * 0.6);
	SettingTextSmallWindowTextLedt->setFont(font);
	SettingTextSmallWindowTextLedt->show();

	SettingTextStartWindowTextLedt = new QLineEdit(SettingTextStartWindowTextLabel);
	SettingTextStartWindowTextLedt->setGeometry(SettingTextStartWindowTextLabel->width() * 0.4, 0, SettingTextStartWindowTextLabel->width() * 0.6, SettingTextStartWindowTextLabel->height());
	SettingTextStartWindowTextLedt->setText(StartWindowText);
	SettingTextStartWindowTextLedt->setPlaceholderText("Large window text");  // 英文默认
	SettingTextStartWindowTextLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); padding-left: " + QString::number(borderRadius / 2) + "px; border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
	SettingTextStartWindowTextLedt->setFont(font);
	SettingTextStartWindowTextLedt->show();

	SettingTextStartWindowEnglishLedt = new QLineEdit(SettingTextStartWindowEnglishLabel);
	SettingTextStartWindowEnglishLedt->setGeometry(SettingTextStartWindowEnglishLabel->width() * 0.4, 0, SettingTextStartWindowEnglishLabel->width() * 0.6, SettingTextStartWindowEnglishLabel->height());
	SettingTextStartWindowEnglishLedt->setText(StartWindowEnglishText);
	SettingTextStartWindowEnglishLedt->setPlaceholderText("Large window English");  // 英文默认
	SettingTextStartWindowEnglishLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); padding-left: " + QString::number(borderRadius / 2) + "px; border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
	SettingTextStartWindowEnglishLedt->setFont(font);
	SettingTextStartWindowEnglishLedt->show();

	SettingTextTimeLedt = new QLineEdit(SettingTextTimeLabel);
	SettingTextTimeLedt->setGeometry(SettingTextTimeLabel->width() * 0.4, 0, SettingTextTimeLabel->width() * 0.6, SettingTextTimeLabel->height());
	SettingTextTimeLedt->setText(targetDateTime.toString("yyyy-MM-dd hh:mm:ss"));
	SettingTextTimeLedt->setPlaceholderText("Target time");  // 英文默认
	SettingTextTimeLedt->setStyleSheet("QLineEdit{background-color: rgba(235, 235, 235, 0.5); padding-left: " + QString::number(borderRadius / 2) + "px; border-radius: " + QString::number(borderRadius) + "px; border-top-right-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235); } QLineEdit:focus{background-color: rgba(135, 205, 250, 0.6); border: " + QString::number(border) + "px solid rgb(0, 190, 255); }");
	SettingTextTimeLedt->setFont(font);
	SettingTextTimeLedt->show();

	// CheckBoxes
	SettingBigWindowIsShowCb = new QCheckBox(SmallWindowSettingBigWindowGb);
	SettingBigWindowIsShowCb->setGeometry(0, SmallWindowSettingBigWindowGb->height() * 0.2, SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.8 / 3);
	SettingBigWindowIsShowCb->setText("Show full screen reminder");  // 英文默认
	SettingBigWindowIsShowCb->setChecked(isShowBigWindow);
	SettingBigWindowIsShowCb->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
	font.setPixelSize(SettingBigWindowIsShowCb->height() * 0.5);
	SettingBigWindowIsShowCb->setFont(font);
	SettingBigWindowIsShowCb->show();

	SettingBigWindowIsCountdownAudioCb = new QCheckBox(SmallWindowSettingBigWindowGb);
	SettingBigWindowIsCountdownAudioCb->setGeometry(0, SmallWindowSettingBigWindowGb->height() * 0.2 + SmallWindowSettingBigWindowGb->height() * 0.8 / 3, SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.8 / 3);
	SettingBigWindowIsCountdownAudioCb->setText("Play countdown sound when ≤30 days");  // 英文默认
	SettingBigWindowIsCountdownAudioCb->setChecked(isCountdownAudio);
	SettingBigWindowIsCountdownAudioCb->setEnabled(isShowBigWindow);
	SettingBigWindowIsCountdownAudioCb->setStyleSheet("QCheckBox{background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QCheckBox:disabled{background-color: rgba(155, 155, 155, 0.25); border: " + QString::number(border) + "px solid rgb(155, 155, 155);}");
	SettingBigWindowIsCountdownAudioCb->setFont(font);
	SettingBigWindowIsCountdownAudioCb->show();

	SettingBigWindowIsHeartbeatAudioCb = new QCheckBox(SmallWindowSettingBigWindowGb);
	SettingBigWindowIsHeartbeatAudioCb->setGeometry(0, SmallWindowSettingBigWindowGb->height() * 0.2 + 2 * SmallWindowSettingBigWindowGb->height() * 0.8 / 3, SmallWindowSettingBigWindowGb->width() * 0.8, SmallWindowSettingBigWindowGb->height() * 0.8 / 3);
	SettingBigWindowIsHeartbeatAudioCb->setText("Play heartbeat sound when ≤14 days");  // 英文默认
	SettingBigWindowIsHeartbeatAudioCb->setChecked(isHeartbeatAudio);
	SettingBigWindowIsHeartbeatAudioCb->setEnabled(isShowBigWindow);
	SettingBigWindowIsHeartbeatAudioCb->setStyleSheet("QCheckBox{background-color: rgba(235, 235, 235, 0.5); border-top-right-radius: 0; border-top-left-radius: 0; border-bottom-right-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);} QCheckBox:disabled{background-color: rgba(155, 155, 155, 0.25); border: " + QString::number(border) + "px solid rgb(155, 155, 155);}");
	SettingBigWindowIsHeartbeatAudioCb->setFont(font);
	SettingBigWindowIsHeartbeatAudioCb->show();

	// ComboBoxes
	SettingLanguageCmb = new QComboBox(this);
	SettingLanguageCmb->setGeometry(this->width() * 0.1, this->height() * 17, desktop.width() * 0.465, desktop.height() * 0.05);
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

	// RadioButtons
	SettingSmallWindowOnTopRbtn = new QRadioButton(SmallWindowSettingSmallWindowOnTopOrBottomGb);
	SettingSmallWindowOnTopRbtn->setGeometry(0, SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.2, SmallWindowSettingSmallWindowOnTopOrBottomGb->width(), SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.4);
	SettingSmallWindowOnTopRbtn->setText("Small window on top");  // 英文默认
	SettingSmallWindowOnTopRbtn->setChecked(SmallWindowOnTopOrBottom);
	SettingSmallWindowOnTopRbtn->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
	font.setPixelSize(SettingSmallWindowOnTopRbtn->height() * 0.6);
	SettingSmallWindowOnTopRbtn->setFont(font);
	SettingSmallWindowOnTopRbtn->show();

	SettingSmallWindowOnBottomRbtn = new QRadioButton(SmallWindowSettingSmallWindowOnTopOrBottomGb);
	SettingSmallWindowOnBottomRbtn->setGeometry(0, SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.6, SmallWindowSettingSmallWindowOnTopOrBottomGb->width(), SmallWindowSettingSmallWindowOnTopOrBottomGb->height() * 0.4);
	SettingSmallWindowOnBottomRbtn->setText("Small window on bottom");  // 英文默认
	SettingSmallWindowOnBottomRbtn->setChecked(!SmallWindowOnTopOrBottom);
	SettingSmallWindowOnBottomRbtn->setStyleSheet("background-color: rgba(235, 235, 235, 0.5); border-top-right-radius: 0; border-top-left-radius: 0; border: " + QString::number(border) + "px solid rgb(235, 235, 235);");
	SettingSmallWindowOnBottomRbtn->setFont(font);
	SettingSmallWindowOnBottomRbtn->show();

	// Animations - 初始化所有动画
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

	StartWindowColorLabelAnimation1 = new QPropertyAnimation(StartWindowColorLabel, "geometry");
	StartWindowColorLabelAnimation1->setDuration(250);
	StartWindowColorLabelAnimation1->setStartValue(QRect(-desktop.width(), 0, desktop.width(), desktop.height()));
	StartWindowColorLabelAnimation1->setEndValue(QRect(0, 0, desktop.width(), desktop.height()));

	StartWindowColorLabelAnimation2 = new QPropertyAnimation(StartWindowColorLabel, "geometry");
	StartWindowColorLabelAnimation2->setDuration(1000);
	StartWindowColorLabelAnimation2->setStartValue(QRect(0, 0, desktop.width(), desktop.height()));
	StartWindowColorLabelAnimation2->setEndValue(QRect(desktop.width(), 0, desktop.width(), desktop.height()));
	StartWindowColorLabelAnimation2->setEasingCurve(QEasingCurve::OutCubic);

	StartWindowTextColorLabelAnimation = new QPropertyAnimation(StartWindowTextColorLabel, "geometry");
	StartWindowTextColorLabelAnimation->setDuration(1500);
	StartWindowTextColorLabelAnimation->setStartValue(QRect(desktop.width() * 0.1 + desktop.height() * 0.2, desktop.height() * 0.575, desktop.width() * 0.0125, 0));
	StartWindowTextColorLabelAnimation->setEndValue(QRect(desktop.width() * 0.1 + desktop.height() * 0.2, desktop.height() * 0.45, desktop.width() * 0.0125, desktop.height() * 0.25));
	StartWindowTextColorLabelAnimation->setEasingCurve(QEasingCurve::OutCubic);

	// 这里注意：不能用重复的名字，将原来的 StartWindowCloseOpacityAnimation 改为 StartWindowBigWindowCloseOpacityAnimation
	QPropertyAnimation* StartWindowBigWindowCloseOpacityAnimation = new QPropertyAnimation(StartWindow, "windowOpacity");
	StartWindowBigWindowCloseOpacityAnimation->setDuration(3000);
	StartWindowBigWindowCloseOpacityAnimation->setStartValue(1);
	StartWindowBigWindowCloseOpacityAnimation->setEndValue(0);
	StartWindowBigWindowCloseOpacityAnimation->setEasingCurve(QEasingCurve::InCubic);

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
	StartWindowAnimationGroup->addAnimation(StartWindowColorLabelAnimation1);
	StartWindowAnimationGroup->addAnimation(StartWindowColorLabelAnimation2);
	StartWindowAnimationGroup->addAnimation(StartWindowTextColorLabelAnimation);
	StartWindowAnimationGroup->addAnimation(StartWindowBigWindowCloseOpacityAnimation);  // 使用新的变量名

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
		startShowBigWindowAnimation();
	}
	else SmallWindowStartAnimationGroup->start();

	selectLanguage();

	// connects
	// 每隔1秒更新
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, [=] {
		updateLabel();
		});
	timer->start(1000);

	connect(StartWindowColorLabelAnimation1, &QPropertyAnimation::finished, [&] {
		StartWindowUnderlyingLabel->show();
		StartWindowTextLabel1->show();
		StartWindowTextLabel2->show();
		StartWindowNumberLabel->show();
		StartWindowTextLabel3->show();
		StartWindowTextLabelEnglish->show();
		updateLabel();
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
		StartWindow->setWindowOpacity(1);
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

	// 创建所有控件后，加载语言
	loadLanguage(currentLangCode);

	// 扫描可用的语言
	scanLanguage();
	selectLanguage();

	// 应用当前语言
	applyLanguage();
}

UniversalTimer::~UniversalTimer() {
	// 清理语言设置
	if (langSettings) {
		delete langSettings;
	}
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
	StartWindowTextLabelEnglish->move(StartWindowTextLabel2->x() + desktop.height() * 0.025, StartWindowTextLabel2->y() + StartWindowTextLabel2->height());
	StartWindowTextColorLabelAnimation->setStartValue(QRect(StartWindowTextLabel2->x() - desktop.width() * 0.025, StartWindowTextLabel2->y() + (StartWindowTextLabel2->height() + StartWindowTextLabelEnglish->height()) / 2, desktop.width() * 0.01, 0));
	StartWindowTextColorLabelAnimation->setEndValue(QRect(StartWindowTextLabel2->x() - desktop.width() * 0.025, StartWindowTextLabel2->y(), desktop.width() * 0.01, StartWindowTextLabel2->height() + StartWindowTextLabelEnglish->height()));


	// 判断是否到达TimeList中的任意时间点
	QTime currentTime = QTime::fromString(currentDateTime.toString("h:mm:ss"), "h:mm:ss");
	if (TimeList.contains(currentTime)) {
		//SmallWindowLabel->setText(currentDateTime.toString("h:mm:ss") + TimeList[i]);
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
			else if (list[0] == "Language") {
				// 使用新的语言代码格式
				currentLangCode = list[1];
			}
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
		readTimeConfig();
	}
	else {
		QMessageBox::information(NULL, QString::fromUtf8("Welcome"), QString::fromUtf8("欢迎使用万能倒计时！\n\n您可以通过本软件设置倒计时，然后清晰明了地查看距离倒计时还剩多少时间！\n\n开始后，点击倒计时条的三条杠，设置完毕并开始使用吧！"));
		ConfigVersion = RightConfigVersion;
		currentLangCode = "zh_cn";  // 默认简体中文
		writeConfig();
		writeTimeConfig();
	}
}

void UniversalTimer::writeConfig() {
	QFile file("config.ini");
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		out << "ConfigVersion=" << RightConfigVersion << "\n";
		out << "Language=" << currentLangCode << "\n";  // 保存新的语言代码
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

void UniversalTimer::readTimeConfig() {
	QFile file("timeconfig.ini");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&file);
		TimeList.clear();
		while (!in.atEnd()) {
			TimeList.append(QTime::fromString(in.readLine(), "h:mm:ss"));
		}
		file.close();
	}
}

void UniversalTimer::writeTimeConfig() {
	QFile file("timeconfig.ini");
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		for (int i = 0; i < TimeList.size(); i++) {
			out << TimeList[i].toString("h:mm:ss") << "\n";
		}
		file.close();
	}
}

void UniversalTimer::startShowBigWindowAnimation() {
	StartWindow->show();
	StartWindowColorLabel->show();
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

void UniversalTimer::loadLanguage(const QString& langCode) {
	// 构建语言文件路径
	QString langFilePath = QString("./lang/%1.lang").arg(langCode);

	// 删除旧的 langSettings
	if (langSettings) {
		delete langSettings;
		langSettings = nullptr;
	}

	// 如果语言文件存在，加载它
	if (QFile::exists(langFilePath)) {
		langSettings = new QSettings(langFilePath, QSettings::IniFormat);
		currentLangCode = langCode;
	}
	else {
		// 如果文件不存在，使用默认语言
		qDebug() << "Language file not found:" << langFilePath;
		if (QFile::exists("lang/zh_cn.lang")) {
			langSettings = new QSettings("lang/zh_cn.lang", QSettings::IniFormat);
			currentLangCode = "zh_cn";
		}
		else {
			// 如果没有语言文件，创建空的 QSettings
			langSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "UniversalTimer", "Language");
		}
	}

	// 应用语言
	applyLanguage();
}

void UniversalTimer::applyLanguage() {
	if (!langSettings) return;

	langSettings->beginGroup("UI");

	// 应用所有UI文本
	SmallWindowSettingTextGb->setTitle(langSettings->value("SmallWindowSettingTextGb_text", QString::fromUtf8("文本和时间")).toString());
	SmallWindowSettingBigWindowGb->setTitle(langSettings->value("SmallWindowSettingBigWindowGb_text", QString::fromUtf8("全屏提醒")).toString());
	SmallWindowSettingSmallWindowOnTopOrBottomGb->setTitle(langSettings->value("SmallWindowSettingSmallWindowOnTopOrBottomGb_text", QString::fromUtf8("小窗口层级")).toString());

	SettingTextSmallWindowTextLabel->setText(langSettings->value("SettingTextSmallWindowTextLabel_text", QString::fromUtf8("小窗口文本:")).toString());
	SettingTextStartWindowTextLabel->setText(langSettings->value("SettingTextStartWindowTextLabel_text", QString::fromUtf8("大窗口文本:")).toString());
	SettingTextStartWindowEnglishLabel->setText(langSettings->value("SettingTextStartWindowEnglishLabel_text", QString::fromUtf8("大窗口英文:")).toString());
	SettingTextTimeLabel->setText(langSettings->value("SettingTextTimeLabel_text", QString::fromUtf8("终点时间:")).toString());

	SettingTextYesBtn->setText(langSettings->value("SettingTextYesBtn_text", QString::fromUtf8("保存")).toString());

	SettingBigWindowIsShowCb->setText(langSettings->value("SettingBigWindowIsShowCb_text", QString::fromUtf8("是否显示全屏提醒")).toString());
	SettingBigWindowIsCountdownAudioCb->setText(langSettings->value("SettingBigWindowIsCountdownAudioCb_text", QString::fromUtf8("剩余时间≤30天时是否播放倒计时提醒音")).toString());
	SettingBigWindowIsHeartbeatAudioCb->setText(langSettings->value("SettingBigWindowIsHeartbeatAudioCb_text", QString::fromUtf8("剩余时间≤14天时是否播放心跳提醒音")).toString());

	SettingBigWindowTryBtn->setText(langSettings->value("SettingBigWindowTryBtn_text", QString::fromUtf8("立即\n播放\n动画")).toString());

	SettingSmallWindowOnTopRbtn->setText(langSettings->value("SettingSmallWindowOnTopRbtn_text", QString::fromUtf8("小窗口置顶")).toString());
	SettingSmallWindowOnBottomRbtn->setText(langSettings->value("SettingSmallWindowOnBottomRbtn_text", QString::fromUtf8("小窗口置底")).toString());

	BilibiliBtn->setText(langSettings->value("BilibiliBtn_text", QString::fromUtf8("哔哩哔哩")).toString() + QString::fromUtf8(": @龙ger_longer"));
	GithubBtn->setText(langSettings->value("GithubBtn_text", QString::fromUtf8("Github开源仓库")).toString());
	ExitBtn->setText(langSettings->value("ExitBtn_text", QString::fromUtf8("退出")).toString());

	AskExitMsgBox->setWindowTitle(langSettings->value("AskExitMsgBox_title", QString::fromUtf8("万能倒计时 - 提示")).toString());
	AskExitMsgBox->setText(langSettings->value("AskExitMsgBox_text", QString::fromUtf8("确定要退出吗？")).toString());

	langSettings->endGroup();
}

void UniversalTimer::scanLanguage() {
	if (!SettingLanguageCmb) return;

	SettingLanguageCmb->clear();

	// 扫描 lang 目录下的 .lang 文件
	QDir langDir("lang");
	QStringList filters;
	filters << "*.lang";
	langDir.setNameFilters(filters);

	QStringList langFiles = langDir.entryList();

	// 定义语言名称映射
	QMap<QString, QString> langNameMap;
	langNameMap["en_us"] = "English";
	langNameMap["zh_cn"] = QString::fromUtf8("简体中文");
	langNameMap["zh_tw"] = QString::fromUtf8("繁體中文");

	foreach(QString langFile, langFiles) {
		QString langCode = langFile.left(langFile.length() - 5); // 移除 .lang 后缀

		// 尝试从文件读取语言名称
		QSettings langSetting(langDir.filePath(langFile), QSettings::IniFormat);
		QString langName = langSetting.value("Language/Name", "").toString();

		// 如果没有定义名称，使用映射或文件名
		if (langName.isEmpty()) {
			if (langNameMap.contains(langCode)) {
				langName = langNameMap[langCode];
			}
			else {
				langName = langCode;
			}
		}

		SettingLanguageCmb->addItem(langName, langCode);
	}

	// 如果没有找到任何语言文件，添加默认选项
	if (SettingLanguageCmb->count() == 0) {
		SettingLanguageCmb->addItem("English", "en_us");
		SettingLanguageCmb->addItem(QString::fromUtf8("简体中文"), "zh_cn");
	}
}

void UniversalTimer::selectLanguage() {
	if (!SettingLanguageCmb) return;

	// 根据保存的语言代码选择对应的选项
	for (int i = 0; i < SettingLanguageCmb->count(); ++i) {
		if (SettingLanguageCmb->itemData(i).toString() == currentLangCode) {
			SettingLanguageCmb->setCurrentIndex(i);
			return;
		}
	}

	// 如果没找到，默认选择第一个
	if (SettingLanguageCmb->count() > 0) {
		SettingLanguageCmb->setCurrentIndex(0);
		currentLangCode = SettingLanguageCmb->itemData(0).toString();
	}
}

void UniversalTimer::changeLanguage() {
	if (!SettingLanguageCmb) return;

	int currentIndex = SettingLanguageCmb->currentIndex();
	if (currentIndex >= 0) {
		QString newLangCode = SettingLanguageCmb->itemData(currentIndex).toString();
		loadLanguage(newLangCode);
		writeConfig();
	}
}