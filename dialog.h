#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QFile>
#include <QString>
#include <QFileInfo>
#include <QDebug>
#include <QProcess>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QHash>
#include "qtaircrack.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    void updateDevicesTable();
    QHash<QString, QStringList>  interfaceList;
    bool updatingDevicesTable = false;

    ~Dialog();

private slots:
    void on_refreshButton_clicked();

    void on_devicesTable_itemChanged(QTableWidgetItem *item);

    void on_cancelButton_clicked();

    void on_interfaceButton_clicked();

    void on_randMACButton_clicked();

    void on_allmonitorcheckBox_stateChanged(int arg1);

    void on_devicesTable_clicked(const QModelIndex &index);

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
