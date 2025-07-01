#ifndef SYSTEMSERVICECONTROLLER_H
#define SYSTEMSERVICECONTROLLER_H

#include <QDefs>
#include <QProcess>

class SystemServiceController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, service, Service, "")
    Q_READONLY_VAR_PROPERTY(bool, serviceExists, ServiceExists, false)
    Q_READONLY_VAR_PROPERTY(bool, processing, Processing, false)

    Q_READONLY_VAR_PROPERTY(bool, loaded, Loaded, false)
    Q_READONLY_VAR_PROPERTY(bool, active, Active, false)
    Q_READONLY_REF_PROPERTY(QString, status, Status, "")

public:
    explicit SystemServiceController(QObject *parent = nullptr);

    Q_INVOKABLE static QStringList services();

public slots:
    void refreshStatus();

    void enable();
    void disable();
    void start();
    void stop();

signals:
    void enableFinished(bool success, const QString &message);
    void disableFinished(bool success, const QString &message);

private:
    void checkService();
    void checkStatus();
    void parseStatusOutput(const QString &output);
};

#endif // SYSTEMSERVICECONTROLLER_H
