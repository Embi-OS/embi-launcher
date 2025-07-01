#include "systemservicecontroller.h"
#include "solid_log.h"

SystemServiceController::SystemServiceController(QObject *parent) :
    QObject(parent)
{
    connect(this, &SystemServiceController::serviceChanged, this, &SystemServiceController::refreshStatus);
}

void SystemServiceController::refreshStatus()
{
    checkService();
    checkStatus();
}

void SystemServiceController::enable()
{
    if(m_service.isEmpty())
        return;

    setProcessing(true);
    setLoaded(true);

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        emit this->enableFinished(ok, ok ? QStringLiteral("Service enabled") : proc->readAllStandardError());
        proc->deleteLater();
        checkStatus();
    }, Qt::QueuedConnection);
    proc->start("systemctl", {"enable", "--now", m_service});
}

void SystemServiceController::disable()
{
    if(m_service.isEmpty())
        return;

    setProcessing(true);
    setLoaded(false);

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        emit this->disableFinished(ok, ok ? QStringLiteral("Service disabled") : proc->readAllStandardError());
        proc->deleteLater();
        checkStatus();
    }, Qt::QueuedConnection);
    proc->start("systemctl", {"disable", "--now", m_service});
}

void SystemServiceController::start()
{
    if(m_service.isEmpty())
        return;

    setProcessing(true);
    setActive(true);

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        emit this->enableFinished(ok, ok ? QStringLiteral("Service started") : proc->readAllStandardError());
        proc->deleteLater();
        checkStatus();
    }, Qt::QueuedConnection);
    proc->start("systemctl", {"start", "--now", m_service});
}

void SystemServiceController::stop()
{
    if(m_service.isEmpty())
        return;

    setProcessing(true);
    setActive(false);

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        emit this->disableFinished(ok, ok ? QStringLiteral("Service stoped") : proc->readAllStandardError());
        proc->deleteLater();
        checkStatus();
    }, Qt::QueuedConnection);
    proc->start("systemctl", {"stop", "--now", m_service});
}

QStringList SystemServiceController::services()
{
    QStringList result;

    QProcess proc;
    proc.start("systemctl", {"list-unit-files", "--no-pager"});
    proc.waitForFinished();
    int exitCode = proc.exitCode();
    bool ok = (exitCode >= 0);
    const QString output = ok ? proc.readAllStandardOutput() : proc.readAllStandardError();

    if (ok) {
        static QRegularExpression regExp = QRegularExpression("\\s+");
        const QStringList lines = output.split('\n');
        for (const QString& line : lines) {
            const QString trimmedLine = line.trimmed();
            if (!trimmedLine.isEmpty() && trimmedLine.contains(".service")) {
                const QStringList parts = trimmedLine.split(regExp);
                if (!parts.isEmpty()) {
                    result.append(parts.first());
                }
            }
        }
    }

    return result;
}

void SystemServiceController::checkService()
{
    if(m_service.isEmpty()) {
        setServiceExists(false);
        return;
    }

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        QString output = ok ? proc->readAllStandardOutput() : proc->readAllStandardError();
        bool exists = output.contains(m_service);
        setServiceExists(exists);
        proc->deleteLater();
    });
    proc->start("systemctl", {"list-unit-files", "--no-pager"});
}

void SystemServiceController::checkStatus()
{
    if(m_service.isEmpty()) {
        setLoaded(false);
        setActive(false);
        setProcessing(false);
        return;
    }

    setProcessing(true);

    // Query systemctl status for m_service
    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int, QProcess::ExitStatus) {
        const QString output =proc->readAllStandardOutput();
        const QString error =proc->readAllStandardError();
        parseStatusOutput(output.isEmpty() ? error : output);
        proc->deleteLater();
    });
    proc->start("systemctl", {"status", m_service, "--no-pager"});
}

void SystemServiceController::parseStatusOutput(const QString &output)
{
    setStatus(output);

    // Example output to parse:
    // ● <service> - ...
    //    Loaded: loaded (/lib/systemd/system/<service>; enabled; vendor preset: enabled)
    //    Active: active
    //    ...
    bool loaded = output.contains("Loaded: loaded");
    bool enabled = output.contains("; enabled");
    bool active = output.contains("Active: active");

    setLoaded(loaded && enabled);
    setActive(active);
    setProcessing(false);
}
