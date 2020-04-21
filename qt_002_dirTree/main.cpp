#include "qt_002_dirtree.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	qt_002_dirTree w;
	w.show();
	return a.exec();
}
