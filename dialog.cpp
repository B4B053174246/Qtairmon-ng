#include "dialog.h"
#include "ui_dialog.h"




void Dialog::updateDevicesTable()
{

    if (updatingDevicesTable == true)
    {
        return;
    }

    updatingDevicesTable = true;

    //Disable
    Dialog::setEnabled(false);

    //Disable button
    ui->interfaceButton->setEnabled(false);

    //Get data
    getInterfacesData(interfaceList);

    if (interfaceList.isEmpty())
    {
        return;
    }

    //Clear rows
    ui->devicesTable->setRowCount(0);

    QStringList phyList;

    int tableRow = 0;
    bool allchecked = true;
    QHashIterator<QString, QStringList> info(interfaceList);
    while (info.hasNext()) {
        info.next();

        //Insert row
        ui->devicesTable->insertRow(tableRow);

        //Get phy number
        phyList << info.value().at(0);

        //Set data
        int tableColumn;
        for (tableColumn = 1;tableColumn < 5 ;++tableColumn)
        {
            QTableWidgetItem* tableItem = new QTableWidgetItem(info.value().at(tableColumn));
            ui->devicesTable->setItem(tableRow,tableColumn-1,tableItem);
        }

        //Set checkbox
        QTableWidgetItem* tableCheckbox = new QTableWidgetItem("");
        tableCheckbox->setCheckState(Qt::Unchecked);

        if (info.value().at(5) == "true")
        {
            tableCheckbox->setCheckState(Qt::Checked);
        }
        else
        {
            //Not all checked
            allchecked = false;
        }

        ui->devicesTable->setItem(tableRow,4,tableCheckbox);
        tableRow++;
    }

    //All enabled checkbox
    if(allchecked)
    {
        ui->allmonitorcheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        ui->allmonitorcheckBox->setCheckState(Qt::Unchecked);
    }

    //Set headers
    ui->devicesTable->setVerticalHeaderLabels(phyList);
    ui->devicesTable->resizeColumnsToContents();

    //Renable
    Dialog::setEnabled(true);
    updatingDevicesTable = false;
}


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{

    //List of binaries
    QStringList filepaths{"/bin/airmon-ng",
                          "/bin/iwconfig",
                          "/bin/macchanger"
                         };

    QString ret = checkDependencies(filepaths);

    //Check deps
    if (isStringValid(ret) == true)
    {
        QMessageBox messageBox;
        messageBox.critical(parent, "Error", ret);
        abort();
    }
    interfaceList.reserve(20);
    ui->setupUi(this);

    updateDevicesTable();
}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_refreshButton_clicked()
{
    updateDevicesTable();
}

void Dialog::on_devicesTable_itemChanged(QTableWidgetItem *item)
{

    if (updatingDevicesTable == true)
    {
        return;
    }

    ui->statusLabel->setText("Status: Working");

    //Disable
    Dialog::setEnabled(false);

    int rownum = item->row();
    if (rownum < 0)
    {
        return;
    }
    //Get name
    QString interfacename = ui->devicesTable->item(rownum,0)->text();

    if (isStringValid(interfacename) == false)
    {
        return;
    }

    //Get state
    QString ret;
    if (item->checkState() == Qt::Checked)
    {
        ret = enableMonitorMode(interfacename);
    }
    else
    {
        ret = disableMonitorMode(interfacename);
    }

    //Check for errors
    if (isStringValid(ret) == true)
    {
        QMessageBox messageBox;
        messageBox.critical(this, "Error", ret);
        abort();
    }


    updateDevicesTable();

    //Renable
    Dialog::setEnabled(true);

    ui->statusLabel->setText("Status: Done");
}


void Dialog::on_cancelButton_clicked()
{
    close();
    qApp->quit();
}

void Dialog::on_interfaceButton_clicked()
{
    int rownum = ui->devicesTable->currentRow();
    if (rownum < 0)
    {
        return;
    }

    qInfo() << "[Info]["<< ui->devicesTable->item(rownum,0)->text() <<"]";
    close();
    qApp->quit();
}



void Dialog::on_randMACButton_clicked()
{
    ui->statusLabel->setText("Status: Working");

    //Disable
    Dialog::setEnabled(false);

    QHashIterator<QString, QStringList> info(interfaceList);
    while (info.hasNext()) {
        info.next();
        randomizeMAC(info.key());

    }
    updateDevicesTable();

    //Renable
    Dialog::setEnabled(true);

    ui->statusLabel->setText("Status: Done");
}

void Dialog::on_allmonitorcheckBox_stateChanged(int arg1)
{
    if (updatingDevicesTable == true)
    {
        return;
    }

    ui->statusLabel->setText("Status: Working");

    //Disable
    Dialog::setEnabled(false);

    QString iname;
    QHashIterator<QString, QStringList> info(interfaceList);
    while (info.hasNext()) {
        info.next();
        iname = info.key();
        if (arg1 == Qt::Checked && (isMonitorMode(iname) == false))
        {
            enableMonitorMode(iname);
        }
        else if (arg1 == Qt::Unchecked && (isMonitorMode(iname) == true))
        {
            disableMonitorMode(iname);
        }
    }


    updateDevicesTable();
    //Renable
    Dialog::setEnabled(true);

    ui->statusLabel->setText("Status: Done");
}

void Dialog::on_devicesTable_clicked(const QModelIndex &index)
{
    ui->interfaceButton->setEnabled(true);
}
