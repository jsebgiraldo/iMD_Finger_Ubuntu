#ifndef DEVICEDISCOVER_H
#define DEVICEDISCOVER_H

#include <QString>
#include <QProcess>

class DeviceDiscover
{
public:
    DeviceDiscover();

public:
    bool GetDevices(const QString var);

private:
    QProcess *process = nullptr;

};

#endif // DEVICEDISCOVER_H
