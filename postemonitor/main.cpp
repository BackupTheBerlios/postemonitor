#include <QApplication>
#include <iostream>

#include "mainwindow.h"

int main(int argc,char* argv[])
{
	//Q_INIT_RESOURCE(raccomandata);

	QApplication app(argc,argv);
	
	myMainWindow raccomandataMW;

	raccomandataMW.show();

	return app.exec();
}
