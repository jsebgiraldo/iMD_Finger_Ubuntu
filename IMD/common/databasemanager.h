#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QIODevice>
#include "IMD/common/fingersList.h"


#define TEMPLATE_SIZE  1024

class databasemanager
{
public:
    databasemanager(const QString &path);
    ~databasemanager();

    bool addFingerprint_to_registeredUser(const QString &userID, int fingerPosition, const QByteArray &fingerTemplate);
    bool addNewUserWithFingerprint(const QString &id, const QString &name, E_FINGER_POSITION fingerPosition, const QByteArray &fingerprintTemplate);
    bool userExists(const QString &userID);
    bool fingerprintExistsForUser(const QString &userID, E_FINGER_POSITION fingerPosition, const QByteArray &fingerTemplate);
    QList<QPair<QString, QByteArray>> getFingerprintsByPosition(E_FINGER_POSITION fingerPosition);
    QMap<E_FINGER_POSITION, QByteArray> getFingerprintsByPositionAndUserID(const QString &userID);
    bool isDatabaseOpen();
    bool clearDatabase();
    QString getUserNameByID(const QString &userID);
    bool deleteFingerprint(const QString& userID, E_FINGER_POSITION fingerPosition);
    int getFingerprintCountForUser(const QString &userID);
    int getDatabaseRecords();

private:
    QSqlDatabase db;

    bool createTables();
};

#endif // DATABASEMANAGER_H
