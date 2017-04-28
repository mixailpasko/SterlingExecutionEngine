#include "ExecEngine.h"
#include <QtGui/QApplication>
#include <ObjBase.h>


int main(int argc, char *argv[])
{
	HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );

	QApplication a(argc, argv);
	ExecEngine w;
	w.hr = hr;
	w.setFixedSize(w.size());
	w.setWindowFlags(Qt::Dialog);
	w.show();
	return a.exec();
	
}
