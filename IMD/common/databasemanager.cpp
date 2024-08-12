#include "databasemanager.h"

// Constructor: Initializes the database connection and creates tables if they don't exist
// Input: const QString &path - the path to the database file
databasemanager::databasemanager(const QString &path)
{
    QString fullpath = path + "/database.db";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fullpath);

    if (!db.open()) {
        qDebug() << "Error: database could not be opened.";
        QFile file(path);
        if (file.open(QIODevice::ReadWrite)) {
            file.close();
            if (db.open()) {
                qDebug() << "Database created at: " << fullpath;
                if (!createTables()) {
                    qDebug() << "Error: Tables in database could not be created.";
                }
            } else {
                qDebug() << "Error: Database could not be created.";
            }
        } else {
            qDebug() << "Error: no se pudo crear el archivo de la base de datos.";
        }
    } else {
        qDebug() << "Opening database.";
        if (!createTables()) {
            qDebug() << "Error: Tables could not be created.";
        }
    }
}

// Checks if the database is open
// Output: bool - true if the database is open, false otherwise
bool databasemanager::isDatabaseOpen(){
    return db.isOpen();
}

// Destructor: Closes the database if it is open
databasemanager::~databasemanager(){
    if (db.isOpen()) {
        db.close();
    }
}

// Creates the necessary tables for storing user and fingerprint data
// Output: bool - true if tables were created successfully, false otherwise
bool databasemanager::createTables() {
    QSqlQuery query;

    QString createUsersTable =
        "CREATE TABLE IF NOT EXISTS Users ("
        "id TEXT PRIMARY KEY, "
        "name TEXT NOT NULL"
        ");";

    QString createFingerprintsTable =
        "CREATE TABLE IF NOT EXISTS Fingerprints ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id TEXT, "
        "finger_position INTEGER, "
        "template BLOB, "
        "FOREIGN KEY (user_id) REFERENCES Users(id), "
        "UNIQUE(user_id, finger_position)"
        ");";

    if (!query.exec(createUsersTable)) {
        qDebug() << "Error: Users table could not be created:" << query.lastError().text();
        return false;
    }

    if (!query.exec(createFingerprintsTable)) {
        qDebug() << "Error: Fingerprints could not be created:" << query.lastError().text();
        return false;
    }

    return true;
}

// Checks if a user exists in the database
// Input: const QString &userID - the ID of the user
// Output: bool - true if the user exists, false otherwise
bool databasemanager::userExists(const QString &userID) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Users WHERE id = :id");
    query.bindValue(":id", userID);

    if (!query.exec()) {
        qDebug() << "Error al verificar la existencia del usuario:" << query.lastError().text();
        return false;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        return true;
    } else {
        return false;
    }
}

// Adds a fingerprint to a registered user
// Input: const QString &userID - the ID of the user
//        int fingerPosition - the position of the finger
//        const QByteArray &fingerTemplate - the fingerprint template data
// Output: bool - true if the fingerprint was added successfully, false otherwise
bool databasemanager::addFingerprint_to_registeredUser(const QString &userID, int fingerPosition, const QByteArray &fingerTemplate) {
    QSqlQuery query;
    query.prepare("INSERT INTO Fingerprints (user_id, finger_position, template) VALUES (:user_id, :finger_position, :template)");
    query.bindValue(":user_id", userID);
    query.bindValue(":finger_position", fingerPosition);
    query.bindValue(":template", fingerTemplate);

    if (!query.exec()) {
        qDebug() << "Error al insertar la huella:" << query.lastError().text();
        return false;
    }
    return true;
}

// Adds a new user with a fingerprint
// Input: const QString &id - the ID of the user
//        const QString &name - the name of the user
//        E_FINGER_POSITION fingerPosition - the position of the finger
//        const QByteArray &fingerprintTemplate - the fingerprint template data
// Output: bool - true if the user and fingerprint were added successfully, false otherwise
bool databasemanager::addNewUserWithFingerprint(const QString &id, const QString &name, E_FINGER_POSITION fingerPosition, const QByteArray &fingerprintTemplate) {
    QSqlQuery query;
    QSqlDatabase::database().transaction();

    // Insert the user
    query.prepare("INSERT INTO Users (id, name) VALUES (:id, :name)");
    query.bindValue(":id", id);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "Error al insertar el usuario:" << query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }

    // Insert the fingerprint
    query.prepare("INSERT INTO Fingerprints (user_id, finger_position, template) VALUES (:user_id, :finger_position, :template)");
    query.bindValue(":user_id", id);
    query.bindValue(":finger_position", static_cast<int>(fingerPosition));
    query.bindValue(":template", fingerprintTemplate);

    if (!query.exec()) {
        qDebug() << "Error al insertar la huella digital:" << query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }

    QSqlDatabase::database().commit();
    return true;
}

// Retrieves fingerprints by finger position
// Input: E_FINGER_POSITION fingerPosition - the position of the finger
// Output: QList<QPair<QString, QByteArray>> - a list of user IDs and fingerprint templates
QList<QPair<QString, QByteArray>> databasemanager::getFingerprintsByPosition(E_FINGER_POSITION fingerPosition) {
    QSqlQuery query;
    query.prepare("SELECT user_id, template FROM Fingerprints WHERE finger_position = :finger_position");
    query.bindValue(":finger_position", static_cast<int>(fingerPosition));

    QList<QPair<QString, QByteArray>> fingerprints;
    if (query.exec()) {
        while (query.next()) {
            QString userId = query.value(0).toString();
            QByteArray templateData = query.value(1).toByteArray();
            fingerprints.append(qMakePair(userId, templateData));
        }
    } else {
        qDebug() << "getFingerprintsByPosition error:" << query.lastError();
    }

    return fingerprints;
}


// Retrieves fingerprints by finger position and user ID
// Input: const QString &userID - the ID of the user
// Output: QMap<E_FINGER_POSITION, QByteArray> - a map of finger positions to fingerprint templates
QMap<E_FINGER_POSITION, QByteArray> databasemanager::getFingerprintsByPositionAndUserID(const QString &userID) {
    QMap<E_FINGER_POSITION, QByteArray> fingerprintsMap;

    QSqlQuery query;
    query.prepare("SELECT finger_position, template FROM Fingerprints WHERE user_id = :user_id");
    query.bindValue(":user_id", userID);

    if (query.exec()) {
        while (query.next()) {
            int fingerPosition = query.value(0).toInt();
            QByteArray templateData = query.value(1).toByteArray();
            fingerprintsMap[static_cast<E_FINGER_POSITION>(fingerPosition)] = templateData;
        }
    } else {
        qDebug() << "getFingerprintsByPositionAndUserID error:" << query.lastError();
    }

    return fingerprintsMap;
}

// Clears the entire database by deleting all records from the tables
// Output: bool - true if the database was cleared successfully, false otherwise
bool databasemanager::clearDatabase() {
    QSqlQuery query;
    QSqlDatabase::database().transaction();

    if (!query.exec("DELETE FROM Fingerprints")) {
        qDebug() << "Error: could not clear Fingerprints table:" << query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }

    if (!query.exec("DELETE FROM Users")) {
        qDebug() << "Error: could not clear Users table:" << query.lastError().text();
        QSqlDatabase::database().rollback();
        return false;
    }

    QSqlDatabase::database().commit();
    return true;
}

QString databasemanager::getUserNameByID(const QString &userID) {
    QSqlQuery query;
    query.prepare("SELECT name FROM Users WHERE id = :id");
    query.bindValue(":id", userID);

    if (!query.exec()) {
        qDebug() << "Error al obtener el nombre del usuario:" << query.lastError().text();
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString();
    } else {
        return QString();
    }
}


bool databasemanager::deleteFingerprint(const QString& userID, E_FINGER_POSITION fingerPosition) {
    QSqlQuery query;
    query.prepare("DELETE FROM Fingerprints WHERE user_id = :user_id AND finger_position = :finger_position");
    query.bindValue(":user_id", userID);
    query.bindValue(":finger_position", static_cast<int>(fingerPosition));

    if (!query.exec()) {
        qDebug() << "Error al eliminar la huella digital existente:" << query.lastError().text();
        return false;
    }

    return true;
}

int databasemanager::getFingerprintCountForUser(const QString &userID) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Fingerprints WHERE user_id = :user_id");
    query.bindValue(":user_id", userID);

    if (!query.exec()) {
        qDebug() << "Error al contar huellas digitales para el usuario:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value(0).toInt();
    } else {
        return 0;
    }
}

int databasemanager::getDatabaseRecords()
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Fingerprints;");

    if (!query.exec()) {
        qDebug() << "Error al contar huellas digitales para el usuario:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value(0).toInt();
    } else {
        return 0;
    }
}
