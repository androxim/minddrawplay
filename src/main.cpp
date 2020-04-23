#include "mainwindow.h"
#include <QApplication>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <complex>
#include <valarray>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QDebug>
#include <QElapsedTimer>
#include <QMainWindow>
#include <QScrollArea>
#include <qcustomplot.h>
#include <plotwindow.h>
#include "appconnect.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setFixedSize(648,363);
    w.setWindowTitle("MindDrawPlay beta 2.4");

    w.show();

    return a.exec();
}
