#include "mainwindow.h"

#include <QApplication>
#include <QMetaType>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    qRegisterMetaType< QVector< QVector<double> > > ();
    qRegisterMetaType< QVector< QString > > ();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
