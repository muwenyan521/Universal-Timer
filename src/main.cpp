#include "UniversalTimer.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	UniversalTimer w;
	w.show();
	return a.exec();
}
