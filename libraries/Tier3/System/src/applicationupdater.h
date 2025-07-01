#ifndef APPLICATIONUPDATER_H
#define APPLICATIONUPDATER_H

#include <QDefs>

class ApplicationUpdater : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit ApplicationUpdater(QObject* parent=nullptr);

    static void init();

public slots:
    void run();
    void run(const QString& path);

private:
    static void exitWithError(const QString& msg);

    QString m_newVersionPath;
};

#endif // APPLICATIONUPDATER_H
