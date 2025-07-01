#include "applicationupdater.h"
#include "solid_log.h"

#include <QUtils>
#include <QtConcurrentRun>
#include <QDeferred>
#include <Axion>

#include "core/paths.h"
#include "dialogs/dialogmanager.h"

ApplicationUpdater::ApplicationUpdater(QObject *parent) :
    QObject(parent)
{

}

void ApplicationUpdater::init()
{
    QString newVersionLocalFile = Paths::local(Paths::applicationName()+"_new");
    QString oldVersionLocalFile = Paths::local(Paths::applicationName()+"_old");
    if(QFile::exists(newVersionLocalFile))
    {
        QUtils::Filesystem::remove(newVersionLocalFile);
    }
    if(QFile::exists(oldVersionLocalFile))
    {
        QUtils::Filesystem::remove(oldVersionLocalFile);
    }

#ifdef Q_OS_BOOT2QT
    QString localFile = Paths::local(Paths::applicationName());
    SOLIDLOG_INFO()<<"Making default"<<localFile;
    QUtils::Process::exec("appcontroller",QStringList({"--make-default",localFile}));
#endif
}

void ApplicationUpdater::run()
{
    QVariantMap settings;
    settings["title"] = tr("Mettre à jour");
    settings["message"] = tr("Sélectionner le fichier de mise à jour");
    settings["selectionType"] = FolderTreeTypes::File;
    DialogObject* dialog = DialogManager::Get()->showFileTree(settings);
    dialog->onPathSelected([this](const QString& path) mutable {
        run(path);
    }, Qt::QueuedConnection);
}

void ApplicationUpdater::run(const QString& path)
{
    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists() || !fileInfo.isFile())
    {
        exitWithError(tr("Le fichier %1 n'existe pas").arg(path));
        return;
    }
    if(fileInfo.fileName()!=Paths::applicationName())
    {
        exitWithError(tr("La mise à jour n'est pas compatible").arg(path));
        return;
    }

    QString localFile = Paths::local(Paths::applicationName());
    QString newVersionLocalFile = Paths::local(Paths::applicationName()+"_new");

    auto importStep = [this, path, newVersionLocalFile]() {
        DialogObject* dialog = DialogManager::Get()->showBusy(tr("Chargement de la mise à jour!"));

        QDefer defer;
        auto future = QtConcurrent::run([path, newVersionLocalFile](){
            return QUtils::Filesystem::copy(path, newVersionLocalFile, true);
        });

        future.then(this, [defer](bool result) mutable {
            defer.end(result);
        });

        defer.complete([dialog](bool){ dialog->hide(); });

        return defer;
    };

    auto controlStep = [newVersionLocalFile, localFile]() {
        QDeferred<QString> defer;

        QFile newVersion(newVersionLocalFile);
        QFile oldVersion(localFile);

        if(!(newVersion.exists() && oldVersion.exists()))
        {
            SOLIDLOG_WARNING()<<"newVersion or oldVersion does not exits";
            defer.reject(tr("Il semble manquer un fichier"));
        }
        else if(!newVersion.setPermissions(QFileDevice::Permission(0x0777)))
        {
            SOLIDLOG_WARNING()<<"Can't set permissions to newVersion";
            defer.reject(tr("Impossible de lire le fichier de mise à jour"));
        }
        else if(!oldVersion.rename(localFile+"_old"))
        {
            SOLIDLOG_WARNING()<<"Can't rename oldVersion to"<<localFile+"_old";
            defer.reject(tr("Impossible de remplacer la version actuelle"));
        }
        else if(!newVersion.rename(localFile))
        {
            SOLIDLOG_WARNING()<<"Can't rename newVersion to"<<localFile;
            defer.reject(tr("Impossible d'importer la nouvelle version"));
        }
        else
        {
            defer.resolve("");
        }

        return defer;
    };

    auto endStep = []() {
        AxionHelper::criticalRestart(tr("Import terminé"), tr("Ne mettez pas le système hors tension"));
    };

    QVariantMap settings;
    settings["message"] = tr("Etes-vous certain de vouloir effectuer une mise à jour?");
    settings["infos"] = tr("Le processus est irréversible");
    settings["buttonAccept"] = tr("Mettre à jour");
    settings["buttonReject"] = tr("Annuler");
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);
    dialog->onAccepted([importStep, controlStep, endStep]() mutable {
        importStep()
        .fail([](){ exitWithError(tr("Erreur lors de l'import de la mise à jour!")); })
        .then<QString>([controlStep](){ return controlStep(); })
        .fail([](const QString& message){ exitWithError(message); })
        .done([endStep](const QString&){ endStep(); });
    });
}

void ApplicationUpdater::exitWithError(const QString& msg)
{
    AxionHelper::criticalRestart(tr("La mise à jour a échoué"), msg);
}
