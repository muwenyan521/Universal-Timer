#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsEffect>
#include <QSoundEffect>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QRect>

class UniversalTimer : public QWidget
{
	Q_OBJECT

public:
	UniversalTimer(QWidget* parent = nullptr);
	~UniversalTimer();

private:

	void updateLabel();
	void readConfig();
	void writeConfig();
	void readTimeConfig();
	void writeTimeConfig();
	void scanLanguage();
	void selectLanguage();
	void changeLanguage();
	void readLanguage();
	void startShowBigWindowAnimation();

	QRect desktop;
	QRect desktopAvailable;

	QDateTime currentDateTime;
	QString currentDateTimeString;
	QDateTime targetDateTime;
	int timeDifference;
	QString timeDifferenceString;

	QList<QTime> TimeList;

	int BlockLabelShowTimes;

	int borderRadius;
	int border;

	int SmallWindowPosition;

	bool isSetting;

	bool isCountdownAudio, isHeartbeatAudio, isShowBigWindow, SmallWindowOnTopOrBottom;

	float LogoPixelMultiplier;


	// 文本设置
	QString SmallWindowText;
	QString StartWindowText;
	QString StartWindowEnglishText;

	// 配置文件版本
	QString RightConfigVersion;
	QString ConfigVersion;

	// 语言设置
	QString Language;
	// 语言版本
	QString RightLanguageVersion;
	QString LanguageVersion;

	// 配置读取变量
	QString all;
	QStringList line;
	QStringList list;

	// 声音
	QSoundEffect* CountdownSoundEffect;
	QSoundEffect* HeartbeatSoundEffect;


	// 透明度变量
	QGraphicsOpacityEffect* NumberLabelOpacityEffect;
	QGraphicsOpacityEffect* StartWindowBlockLabel1OpacityEffect;
	QGraphicsOpacityEffect* StartWindowBlockLabel2OpacityEffect;
	QGraphicsOpacityEffect* StartWindowBlockLabel3OpacityEffect;
	QGraphicsOpacityEffect* StartWindowBlockLabel4OpacityEffect;


	// 一堆动画
	QPropertyAnimation* SmallWindowStartAnimation;
	QPropertyAnimation* SmallWindowStartOpacityAnimation;
	QPropertyAnimation* SmallWindowCloseOpacityAnimation;
	QPropertyAnimation* StartWindowColorLabelAnimation1;
	QPropertyAnimation* StartWindowColorLabelAnimation2;
	QPropertyAnimation* StartWindowTextColorLabelAnimation;
	QPropertyAnimation* StartWindowNumberLabelOpacityAnimation1;
	QPropertyAnimation* StartWindowNumberLabelOpacityAnimation2;
	QPropertyAnimation* StartWindowNumberLabelOpacityAnimation3;
	QPropertyAnimation* StartWindowNumberLabelOpacityAnimation4;
	QPropertyAnimation* StartWindowNumberLabelOpacityAnimation5;
	QPropertyAnimation* StartWindowCloseOpacityAnimation;

	QPropertyAnimation* StartWindowBlockLabel1OpacityAnimation1;
	QPropertyAnimation* StartWindowBlockLabel1OpacityAnimation2;
	QPropertyAnimation* StartWindowBlockLabel1OpacityAnimation3;
	QPropertyAnimation* StartWindowBlockLabel1OpacityAnimation4;
	QPropertyAnimation* StartWindowBlockLabel2OpacityAnimation1;
	QPropertyAnimation* StartWindowBlockLabel2OpacityAnimation2;
	QPropertyAnimation* StartWindowBlockLabel2OpacityAnimation3;
	QPropertyAnimation* StartWindowBlockLabel2OpacityAnimation4;
	QPropertyAnimation* StartWindowBlockLabel3OpacityAnimation1;
	QPropertyAnimation* StartWindowBlockLabel3OpacityAnimation2;
	QPropertyAnimation* StartWindowBlockLabel3OpacityAnimation3;
	QPropertyAnimation* StartWindowBlockLabel3OpacityAnimation4;
	QPropertyAnimation* StartWindowBlockLabel4OpacityAnimation1;
	QPropertyAnimation* StartWindowBlockLabel4OpacityAnimation2;
	QPropertyAnimation* StartWindowBlockLabel4OpacityAnimation3;
	QPropertyAnimation* StartWindowBlockLabel4OpacityAnimation4;

	QPropertyAnimation* SmallWindowMoveAnimation;

	QPropertyAnimation* SmallWindowStartSettingAnimation;
	QPropertyAnimation* SmallWindowUnderlyingLabelStartSettingAnimation;
	QPropertyAnimation* SmallWindowCloseSettingAnimation;
	QPropertyAnimation* SmallWindowUnderlyingLabelCloseSettingAnimation;




	QSequentialAnimationGroup* StartWindowAnimationGroup;
	QSequentialAnimationGroup* StartWindowNumberLabelAnimationGroup;

	QSequentialAnimationGroup* StartWindowBlockLabel1AnimationGroup;
	QSequentialAnimationGroup* StartWindowBlockLabel2AnimationGroup;
	QSequentialAnimationGroup* StartWindowBlockLabel3AnimationGroup;
	QSequentialAnimationGroup* StartWindowBlockLabel4AnimationGroup;




	QParallelAnimationGroup* SmallWindowStartAnimationGroup;


	// Widgets
	QWidget* StartWindow;


	// MessageBoxes
	QMessageBox* AskExitMsgBox;


	// GroupBoxes
	QGroupBox* SmallWindowSettingTextGb;
	QGroupBox* SmallWindowSettingBigWindowGb;
	QGroupBox* SmallWindowSettingSmallWindowOnTopOrBottomGb;



	// PushButtons
	QPushButton* SmallWindowMoreInfBtn;

	QPushButton* SettingTextYesBtn;

	QPushButton* SettingBigWindowTryBtn;

	QPushButton* BilibiliBtn;
	QPushButton* GithubBtn;

	QPushButton* ExitBtn;


	// CheckBoxes
	QCheckBox* SettingBigWindowIsShowCb;
	QCheckBox* SettingBigWindowIsCountdownAudioCb;
	QCheckBox* SettingBigWindowIsHeartbeatAudioCb;


	// RadioButtons
	QRadioButton* SettingSmallWindowOnTopRbtn;
	QRadioButton* SettingSmallWindowOnBottomRbtn;


	// Comboboxes
	QComboBox* SettingLanguageCmb;


	// Labels
	QLabel* SmallWindowUnderlyingLabel;
	QLabel* SmallWindowLabel;
	QLabel* SmallWindowLogoLabel;

	QLabel* StartWindowUnderlyingLabel;
	QLabel* StartWindowColorLabel;
	QLabel* StartWindowTextColorLabel;
	QLabel* StartWindowTextLabel1;
	QLabel* StartWindowTextLabel2;
	QLabel* StartWindowTextLabel3;
	QLabel* StartWindowNumberLabel;
	QLabel* StartWindowTextLabelEnglish;

	QLabel* StartWindowBlockLabel1;
	QLabel* StartWindowBlockLabel2;
	QLabel* StartWindowBlockLabel3;
	QLabel* StartWindowBlockLabel4;

	QLabel* SettingTextSmallWindowTextLabel;
	QLabel* SettingTextStartWindowTextLabel;
	QLabel* SettingTextStartWindowEnglishLabel;
	QLabel* SettingTextTimeLabel;



	// LineEdits
	QLineEdit* SettingTextSmallWindowTextLedt;
	QLineEdit* SettingTextStartWindowTextLedt;
	QLineEdit* SettingTextStartWindowEnglishLedt;
	QLineEdit* SettingTextTimeLedt;




	float startX;
	float startY;
protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void resizeEvent(QResizeEvent* event);
};
