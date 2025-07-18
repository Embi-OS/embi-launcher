#include "version.h"
#include "axion_log.h"

Version::Version(QObject *parent) :
    QObject(parent),
    m_globalVersion(PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH),
    m_versionSuffix(PROJECT_DESCRIPTION),
    m_company(PROJECT_COMPANY),
    m_website(PROJECT_WEBSITE),
    m_copyright(PROJECT_COPYRIGHT),
    m_version(m_versionSuffix.isEmpty() ? QString("%1").arg(m_globalVersion.toString()) : QString("%1 [%2]").arg(m_globalVersion.toString(),m_versionSuffix))
{

}

void Version::dumpInfos() const
{
    QString infos;
    infos += QString("\n\n");
    infos += m_productName.isEmpty() ? "" : QString("\t %1\n").arg(m_productName);
    infos += m_version.isEmpty() ? "" : QString("\t Version: %1\n").arg(m_version);
    infos += m_company.isEmpty() ? "" : QString("\t Company: %1\n").arg(m_company);
    infos += m_website.isEmpty() ? "" : QString("\t Website: %1\n").arg(m_website);
    infos += m_copyright.isEmpty() ? "" : QString("\t %1\n").arg(m_copyright);
    AXIONLOG_INFO().noquote()<<infos;
}

QString Version::about() const
{
    QString about;
    about += m_productName.isEmpty() ? "" : tr("Produit: %1\n").arg(m_productName);
    about += m_version.isEmpty() ? "" : tr("Version: %1\n").arg(m_version);
    about += tr("Version Qt: %1\n").arg(QT_VERSION_STR);
    about += m_company.isEmpty() ? "" : tr("Entreprise: %1\n").arg(m_company);
    about += m_website.isEmpty() ? "" : tr("Site web: %1\n").arg(m_website);
    about += m_copyright.isEmpty() ? "" : tr("Copyright: %1\n").arg(m_copyright);
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
