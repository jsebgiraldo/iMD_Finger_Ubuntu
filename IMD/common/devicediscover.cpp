#include "devicediscover.h"
#include "qdebug.h"

#include <QString>

#include <QProcess>

#include <QDebug>


DeviceDiscover::DeviceDiscover() {

    process = new QProcess();
}


bool DeviceDiscover::GetDevices(const QString var){

    QString program = "powershell";
    QStringList arguments;
    arguments << "-Command" << "$usbDevices = Get-WmiObject -Class Win32_USBControllerDevice -ComputerName . | "
                 "ForEach-Object { [Wmi]$_.Dependent } ; $usbDevices | Where-Object { $_.PNPDeviceID -like '*" + var + "*' } | "
                 "Format-List Name, PNPDeviceID";
    process->start(program, arguments);
    if (!process->waitForStarted() || !process->waitForFinished()) {
        qDebug() << "Error: Failed to execute PowerShell command.";
        return 0;
    }

    QByteArray result = process->readAllStandardOutput();
    QString resultString(result);

    QStringList lines = resultString.split("\n", Qt::SkipEmptyParts);

    QString DeviceID;
    for (const QString &line : lines) {
        if (line.contains("PNPDeviceID")) {
            DeviceID = line.section(':', 1).trimmed();
            break;
        }
    }

    return DeviceID.contains(var) ? true : false;
}
