#include "swupdate.h"
#include "solid_log.h"

#include "axion_helpertypes.h"
#include "dialogs/snackbarloader.h"
#include "dialogs/dialogloader.h"

#include <QSocketNotifier>
#include <QtConcurrent>

#ifdef SWUPDATE_FOUND
#include <progress_ipc.h>
#include <network_ipc.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#endif

SwupdateProgressMessage::SwupdateProgressMessage():
    apiVersion(),
    status(SwupdateRecoveryStatuses::Idle),
    source(SwupdateSourceTypes::Unknown),
    info(),
    downloadPercent(0),
    downloadBytes(0),
    nbSteps(0),
    currentStep(0),
    currentStepPercent(0),
    currrentImage(),
    handlerName()
{

}

QString SwupdateProgressMessage::toString() const
{
    return QStringLiteral("%1 [%2], %3%, step %4/%5  %6% (%7)")
        .arg(SwupdateRecoveryStatuses::asString(status), SwupdateSourceTypes::asString(source)).arg(downloadPercent)
        .arg(currentStep).arg(nbSteps).arg(currentStepPercent)
        .arg(info);
}

QDebug operator<<(QDebug dbg, const SwupdateProgressMessage &msg)
{
    dbg.nospace().noquote()<<"SwupdateProgressMessage("<<msg.toString()<<")";
    return dbg.space().quote();
}

Swupdate::Swupdate(QObject* parent):
    QObject(parent),
    m_progressFd(-1)
{
    open();
}

void Swupdate::init()
{
    Swupdate::Get();

    SnackbarLoader* loader = new SnackbarLoader(Swupdate::Get());
    loader->setTitle(tr("Mise à jour en cours!"));
    connect(Swupdate::Get(), &Swupdate::isRunningChanged, loader, &SnackbarLoader::setActive);
    connect(Swupdate::Get(), &Swupdate::statusChanged, loader, &SnackbarLoader::setCaption);
    connect(Swupdate::Get(), &Swupdate::progressChanged, loader, &SnackbarLoader::setProgress);
}

void Swupdate::unInit()
{
    Swupdate::Get()->deleteLater();
}

bool Swupdate::isReady() const
{
#ifdef SWUPDATE_FOUND
    return m_progressFd>=0;
#else
    return false;
#endif
}

void Swupdate::open()
{
#ifdef SWUPDATE_FOUND
    m_progressFd = progress_ipc_connect(false);
    if (m_progressFd < 0) {
        SOLIDLOG_WARNING()<<"Swupdate: Failed to connect to SWUpdate progress IPC";
        QTimer::singleShot(500, this, &Swupdate::open);
        return;
    }
    else {
        QSocketNotifier* socketNotifier = new QSocketNotifier(m_progressFd, QSocketNotifier::Read, this);
        connect(socketNotifier, &QSocketNotifier::activated, this, &Swupdate::onProgressMessage);

        connect(this, &QObject::destroyed, this, [fd=m_progressFd]() {
            close(fd);
        });
    }
#endif
}

bool Swupdate::update(const QString& file)
{
#ifdef SWUPDATE_FOUND
    return QProcess::startDetached("swupdate-client", {"-q", file});
#else
    return false;
#endif
}

void Swupdate::onProgressMessage()
{
    if(m_progressFd < 0)
        return;

    SwupdateProgressMessage msg;

#ifdef SWUPDATE_FOUND
    struct progress_msg raw;
    int rc = progress_ipc_receive(&m_progressFd, &raw);
    if (rc <= 0) {
        SOLIDLOG_WARNING()<<"Swupdate: Error receiving progress message";
        return;
    }

    msg.apiVersion       = QString("%1.%2")
                         .arg((raw.apiversion >> 16) & 0xFF)
                         .arg((raw.apiversion >> 8)  & 0xFF);
    msg.status           = (SwupdateRecoveryStatus)raw.status;
    msg.source           = (SwupdateSourceType)raw.source;
    msg.info             = QString::fromUtf8(raw.info, raw.infolen);
    msg.downloadPercent  = static_cast<int>(raw.dwl_percent);
    msg.downloadBytes    = static_cast<long long>(raw.dwl_bytes);
    msg.nbSteps          = static_cast<int>(raw.nsteps);
    msg.currentStep      = static_cast<int>(raw.cur_step);
    msg.currentStepPercent = static_cast<int>(raw.cur_percent);
    msg.currrentImage    = QString::fromUtf8(raw.cur_image);
    msg.handlerName      = QString::fromUtf8(raw.hnd_name);
#endif

    // update property
    setProgressMessage(msg);

    // optionally emit log
    SOLIDLOG_DEBUG().noquote()<<msg.toString();
    qDebug().noquote()<<msg.toString();

    setIsRunning(msg.status!=SwupdateRecoveryStatuses::Idle &&
                 msg.status!=SwupdateRecoveryStatuses::Done &&
                 msg.status!=SwupdateRecoveryStatuses::Success &&
                 msg.status!=SwupdateRecoveryStatuses::Failure);

    if(msg.status==SwupdateRecoveryStatuses::Download)
    {
        setProgress(msg.downloadPercent*100.0);

        setStatus(QString("%1, %2  %3%")
                      .arg(SwupdateRecoveryStatuses::asString(msg.status), ::bytes(msg.downloadBytes))
                      .arg(msg.downloadPercent));
    }
    else if(msg.status==SwupdateRecoveryStatuses::Progress || msg.status==SwupdateRecoveryStatuses::Run)
    {
        setProgress(msg.currentStepPercent*100.0);

        setStatus(QString("%1, step %2/%3  %4%")
                      .arg(SwupdateRecoveryStatuses::asString(msg.status))
                      .arg(msg.currentStep).arg(msg.nbSteps).arg(msg.currentStepPercent));
    }
    else
    {
        if(msg.status==SwupdateRecoveryStatuses::Done || msg.status==SwupdateRecoveryStatuses::Success)
        {
            AxionHelper::warningReboot(tr("Mise à jour terminée"));
        }
        else if(msg.status==SwupdateRecoveryStatuses::Failure)
        {
            AxionHelper::warningReboot(tr("Echec de la mise à jour"));
        }

        resetProgress();
        resetStatus();
    }
}
