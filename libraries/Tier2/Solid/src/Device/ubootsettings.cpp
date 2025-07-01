#include "ubootsettings.h"
#include "solid_log.h"

#include <QtConcurrentRun>
#include <QProcess>

#define PRINTENV_CMD QStringLiteral("fw_printenv")
#define SETENV_CMD QStringLiteral("fw_setenv")

UBootSettings::UBootSettings(QObject *parent) :
    QObject(parent),
    m_canPrintEnv(false),
    m_canSetEnv(false)
{
    QString program="which";
    QStringList arguments = QStringList()<<PRINTENV_CMD<<SETENV_CMD;

    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();

    process.waitForFinished(1000);

    QString processOutput = process.readAllStandardOutput();

    if(processOutput.contains(PRINTENV_CMD))
        m_canPrintEnv = true;
    if(processOutput.contains(SETENV_CMD))
        m_canSetEnv = true;
}

bool UBootSettings::canPrintEnv()
{
    return m_canPrintEnv;
}

bool UBootSettings::canSetEnv()
{
    return m_canSetEnv;
}

QString UBootSettings::printEnv(const QString& name)
{
    if(!canPrintEnv())
    {
        SOLIDLOG_WARNING()<<"Cannot print u-boot env"<<name;
        return "";
    }

    QStringList arguments;
    if(!name.isEmpty())
        arguments<<name;

    QProcess process;
    process.setProgram(PRINTENV_CMD);
    process.setArguments(arguments);
    process.start();

    bool result = process.waitForFinished(1000);

    QString processError = process.readAllStandardError();
    QString processOutput = process.readAllStandardOutput();

    if(!result)
        return processError;

    return processOutput.mid(name.size()+1, processOutput.size()-(name.size()+2));
}

bool UBootSettings::setEnv(const QString& name, const QString& value)
{
    if(!canSetEnv())
    {
        SOLIDLOG_WARNING()<<"Cannot set u-boot env"<<name<<"with"<<value;
        return false;
    }

    QStringList arguments = QStringList()<<name;
    if(!value.isEmpty())
        arguments<<value;

    QProcess process;
    process.setProgram(SETENV_CMD);
    process.setArguments(arguments);
    process.start();

    return process.waitForFinished(1000);
}

bool UBootSettings::clearEnv(const QString& name)
{
    if(!canSetEnv())
    {
        SOLIDLOG_WARNING()<<"Cannot clear u-boot env"<<name;
        return false;
    }

    return setEnv(name, "");
}

UBootEnvModel::UBootEnvModel(QObject *parent) :
    QVariantListModel(parent)
{
    m_baseName = "U-Boot";
    m_primaryField = "key";
    m_syncable = true;
    m_selectPolicy = QVariantListModelPolicies::Manual;
    m_submitPolicy = QVariantListModelPolicies::Disabled;

    select();
}

bool UBootEnvModel::doSelect()
{
    if(!m_selectWatcher.isNull()) {
        m_selectWatcher->cancel();
        m_selectWatcher->deleteLater();
    }

    if(!UBootSettings::Get()->canPrintEnv())
    {
        emitSelectDone(false);
        return false;
    }

    QFuture<QVariantList> future = QtConcurrent::run([]() {
        QVariantList storage;
        QStringList envs = UBootSettings::Get()->printEnv("").split("\n", Qt::SkipEmptyParts);
        envs.sort();
        for(const QString& env: std::as_const(envs))
        {
            QStringList lineSplit = env.split("=");
            QVariantMap map;
            map["key"] = lineSplit.takeFirst();
            map["value"] = lineSplit.join("=");
            storage.append(map);
        }
        return storage;
    });

    m_selectWatcher = new QFutureWatcher<QVariantList>(this);
    connect(m_selectWatcher, &QFutureWatcherBase::finished, this, [this]() {
        if(m_selectWatcher.isNull())
            return;
        QVariantList storage = m_selectWatcher->future().result();
        setStorage(std::move(storage));
        emitSelectDone(true);
    });
    m_selectWatcher->setFuture(future);

    return true;
}
