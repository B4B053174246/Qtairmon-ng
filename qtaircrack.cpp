#include "qtaircrack.h"

bool randomizeMAC(QString iname)
{
    //Start process
    QProcess process;
    process.start("ifconfig "+iname+" down");
    process.waitForFinished(-1);
    process.start("macchanger --random "+iname);
    process.waitForFinished(-1);
    process.start("ifconfig "+iname+" up");
    process.waitForFinished(-1);

    return true;
}



QString enableMonitorMode(QString iname)
{
    QString errorstr;
    if(isStringValid(iname) == false)
    {
        errorstr = "[Error][Airmon-ng empty interface name]";
        qCritical() << "[Error][Airmon-ng empty interface name]";
        return errorstr;
    }

    int i;
    QProcess process;
    for(i = 0;i< MAX_AIRMON_TRIES;++i)
    {
        process.start("airmon-ng start "+iname);
        process.waitForFinished(-1);
        QString stdout = process.readAllStandardOutput();

        QStringList data = stdout.split('\n');
        QString newiname;
        foreach (const QString &str, data)
        {
            if (str.contains("monitor mode vif enabled") && str.contains(iname))
            {
                int start = str.lastIndexOf(']');
                int end = str.lastIndexOf(')');
                newiname = str.mid(start+1,end-start-1);
            }
        }

        if (isMonitorMode(newiname) == true)
        {
            return NULL;
        }

    }

    errorstr = "[Error][Airmon-ng failed to enable "+iname+" ]";
    qCritical() << "[Error][Airmon-ng failed to enable " << iname << " ]";
    return errorstr;
}

QString disableMonitorMode(QString iname)
{
    QString errorstr;
    if(isStringValid(iname) == false)
    {
        errorstr = "[Error][Airmon-ng empty interface name]";
        qCritical() << "[Error][Airmon-ng empty interface name]";
        return errorstr;
    }

    int i;
    QProcess process;
    for(i = 0;i< MAX_AIRMON_TRIES;++i)
    {
        process.start("airmon-ng stop "+iname);
        process.waitForFinished(-1);

        if (isMonitorMode(iname) == false)
        {
            return NULL;
        }
    }

    errorstr = "[Error][Airmon-ng failed to disable "+iname+" ]";
    qCritical() << "[Error][Airmon-ng failed to disable " << iname << " ]";
    return errorstr;

}



bool isStringValid(QString str)
{
    if (str.isNull())
    {
        return false;
    }

    if (str.isEmpty())
    {
        return false;
    }

    return true;
}

QString parseMAC(QString str)
{
    if(isStringValid(str) == false)
    {
        return NULL;
    }
    QString text = "Current MAC:";

    if (str.contains("Current MAC:") == false)
    {
        return NULL;
    }

    int i;
    for(i=text.length();i<str.length();++i)
    {
        if(str.at(i) != ' ')
        {
            return str.mid(i,17);
        }
        else
        {
            continue;
        }
    }

    return NULL;
}



QString removeRepeatedChar(QString str,QChar c)
{
    if(isStringValid(str) == false)
    {
        return NULL;
    }
    QString ret;
    bool detected = false;
    foreach (const QChar &ctemp, str)
    {
        if (((detected == true) && (ctemp != c)))
        {
            detected = false;
        }
        else if ((detected == true) && (ctemp == c))
        {
            continue;
        }
        else if (ctemp == c)
        {
            detected = true;
        }

        ret.append(ctemp);
    }
    return ret;
}



bool fileExists(QString path) {

    if(isStringValid(path) == false)
    {
        return false;
    }

    QFileInfo check_file(path);

    //Check if exists
    if (check_file.exists() == false)
    {
        return false;
    }

    //Check if file
    if (check_file.isFile() == false)
    {
        return false;
    }

    return true;
}


QString checkDependencies(QStringList filepaths)
{

    //Check all file paths
    foreach (const QString &str, filepaths)
    {
        if (fileExists(str) == false)
        {
            QString errorstr = "[Error][File '" + str +"' Not Found]";
            qCritical() << "[Error][File" << str << "Not Found]";
            return errorstr;
        }
    }

    if (airmonCheck() == false)
    {
        QString errorstr = "[Error][Airmon-ng Not Working]";
        qCritical() << "[Error][Airmon-ng Not Working]";
        return errorstr;
    }

    return NULL;
}


bool isMonitorMode(QString iname)
{
    if(isStringValid(iname) == false)
    {
        return false;
    }

    QProcess process;
    process.start("iwconfig "+iname);
    process.waitForFinished(-1);

    QString stdout = process.readAllStandardOutput();

    return stdout.contains("Mode:Monitor", Qt::CaseSensitive);
}


bool airmonCheck()
{
    QProcess process;
    process.start("airmon-ng");
    process.waitForFinished(-1);

    QString stdout = process.readAllStandardOutput();

    return stdout.contains("Interface", Qt::CaseSensitive);
}


void getInterfacesData(QHash<QString, QStringList> & map)
{
    //Clear data
    map.clear();

    //Start process
    QProcess process;
    process.start("airmon-ng");
    process.waitForFinished(-1);

    QString stdout = process.readAllStandardOutput();
    //QString stderr = process.readAllStandardError();

    QStringList data =  stdout.split("\n");

    //Parse interfaces
    int row;
    for (row = 3; row < (data.size()-2); ++row)
    {
        //Parse data
        QString infostring = data.at(row);
        infostring = removeRepeatedChar(infostring,'\t');
        QStringList info = infostring.split("\t");

        //Get name
        QString interfacename = info.at(1);

        //Insert
        map.insert(interfacename,info);
    }

    //Get monitor mode
    process.start("iwconfig");
    process.waitForFinished(-1);

    stdout = process.readAllStandardOutput();

    data = stdout.split("\n");

    foreach (const QString &str, data)
    {
        if (str.contains("IEEE 802.11", Qt::CaseSensitive))
        {
            QString interfacename = str.section(' ', 0, 0, QString::SectionSkipEmpty);

            //Get MAC
            process.start("macchanger "+interfacename);
            process.waitForFinished(-1);

            stdout = process.readAllStandardOutput();

            map[interfacename].append(parseMAC(stdout));

            //Find monitor mode
            if (str.contains("Mode:Monitor", Qt::CaseSensitive))
            {
                map[interfacename].append("true");
            }
            else
            {
                map[interfacename].append("false");
            }

        }
    }

}
