#include "version.h"
#include "axion_log.h"

Version::Version(QObject *parent) :
    QObject(parent),
    m_globalVersion(ECOSYSTEM_VERSION_MAJOR, ECOSYSTEM_VERSION_MINOR, ECOSYSTEM_VERSION_PATCH),
    m_versionSuffix(ECOSYSTEM_DESCRIPTION),
    m_company(ECOSYSTEM_COMPANY),
    m_website(ECOSYSTEM_WEBSITE),
    m_copyright(ECOSYSTEM_COPYRIGHT),
    m_version(QString("%1 [%2]").arg(m_globalVersion.toString(),m_versionSuffix))
{

}

void Version::dumpInfos() const
{
    QString debugStr;
    debugStr.append(QString("\n\n"));
    debugStr.append(qPrintable(QString("\t %1\n").arg(getProductName())));
    debugStr.append(qPrintable(QString("\t Version: %1 [%2]\n").arg(getGlobalVersion().toString(),getVersionSuffix())));
    debugStr.append(qPrintable(QString("\t Company: %1 (%2)\n").arg(getCompany(), getWebsite())));
    debugStr.append(qPrintable(QString("\t %1\n").arg(getCopyright())));
    AXIONLOG_INFO().noquote()<<debugStr;
}

QString Version::about() const
{
    QString about;
    about += tr("Produit")+QString(": %1").arg(getProductName())+"\n";
    about += tr("Version")+QString(": %1 [%2]").arg(getGlobalVersion().toString(),getVersionSuffix())+"\n";
    about += tr("Version Qt")+QString(": %1").arg(QT_VERSION_STR)+"\n";
    about += tr("Entreprise")+QString(": %1 (%2)").arg(getCompany(), getWebsite())+"\n";
    about += tr("Copyright")+QString(": %1").arg(getCopyright());
    return about;
}

QString Version::aboutQt() const
{
    QString about = QString(
                        "<h2>This program runs on Qt version %1</h2>"
                        "<p></p>"
                        "<p>Qt is a C++ toolkit for cross-platform application development.</p>"
                        "<p>Qt provides single-source portability across all major desktop "
                        "operating systems. It is also available for embedded Linux and other "
                        "embedded and mobile operating systems.</p>"
                        ).arg(QT_VERSION_STR);
    return about;
}

QString Version::versionToString(const QVersionNumber& version)
{
    return version.toString();
}
