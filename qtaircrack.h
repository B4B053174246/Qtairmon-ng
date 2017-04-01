#ifndef QTAIRCRACK_H
#define QTAIRCRACK_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QFileInfo>
#include <QDebug>
#include <QProcess>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QHash>
#include <QTime>
#include <QCoreApplication>

#define MAX_AIRMON_TRIES   4

bool isStringValid(QString str);
QString removeRepeatedChar(QString str,QChar c);
bool fileExists(QString path);
QString checkDependencies(QStringList filepaths);
bool airmonCheck();


bool isMonitorMode(QString iname);
QString enableMonitorMode(QString iname);
QString disableMonitorMode(QString iname);


void getInterfacesData(QHash<QString, QStringList> & map);
QString parseMAC(QString str);
bool randomizeMAC(QString iname);

#endif // QTAIRCRACK_H
