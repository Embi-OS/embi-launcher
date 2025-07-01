#include "network.h"
#include "solid_log.h"

#include <QProcess>
#include <QHostInfo>
#include <QNetworkInterface>

// TODO: Qt6 QNetworkInformation

Network::Network(QObject *parent) :
    QObject(parent)
{
    if(netInfo())
    {
        connect(netInfo(), &QNetworkInformation::isBehindCaptivePortalChanged, this, &Network::captivePortalChanged);
        connect(netInfo(), &QNetworkInformation::isMeteredChanged, this, &Network::meteredChanged);
        connect(netInfo(), &QNetworkInformation::reachabilityChanged, this, &Network::reachabilityChanged);
        connect(netInfo(), &QNetworkInformation::transportMediumChanged, this, &Network::transportMediumChanged);

        connect(netInfo(), &QNetworkInformation::isBehindCaptivePortalChanged, this, &Network::informationsChanged);
        connect(netInfo(), &QNetworkInformation::isMeteredChanged, this, &Network::informationsChanged);
        connect(netInfo(), &QNetworkInformation::reachabilityChanged, this, &Network::informationsChanged);
        connect(netInfo(), &QNetworkInformation::transportMediumChanged, this, &Network::informationsChanged);
    }
}

QNetworkInformation* Network::netInfo()
{
    static QNetworkInformation* s_netInfoInstance = []() {
        QNetworkInformation::loadDefaultBackend();
        QNetworkInformation* instance = QNetworkInformation::instance();
        if(instance)
            QJSEngine::setObjectOwnership(instance, QJSEngine::CppOwnership);
        return instance;
    }();

    return s_netInfoInstance;
}

QString Network::backend()
{
    if(!netInfo())
        return "N/A";

    QString backendName = netInfo()->backendName();

    // backendName.append("\n(Can be networklistmanager (Windows), scnetworkreachability (Apple macOS, iOS), android (Android), networkmanager (Linux))");

    return backendName;
}
QString Network::reachability()
{
    if(!netInfo() || !netInfo()->supportedFeatures().testFlag(QNetworkInformation::Feature::Reachability))
        return "N/A";

    QString reachability = "?????";
    switch (netInfo()->reachability()) {
    case QNetworkInformation::Reachability::Unknown: reachability = "UNKNOWN"; break;
    case QNetworkInformation::Reachability::Disconnected: reachability = "DISCONNECTED"; break;
    case QNetworkInformation::Reachability::Local: reachability = "LOCAL"; break;
    case QNetworkInformation::Reachability::Site: reachability = "SITE"; break;
    case QNetworkInformation::Reachability::Online: reachability = "ONLINE"; break;
    default: break;
    }

    // reachability.append("\n(Can be UNKNOWN, DISCONNECTED, LOCAL, SITE, ONLINE)");

    return reachability;
}
QString Network::transportMedium()
{
    if(!netInfo() || !netInfo()->supportedFeatures().testFlag(QNetworkInformation::Feature::TransportMedium))
        return "N/A";

    QString transportMedium = "?????";
    switch (netInfo()->transportMedium()) {
    case QNetworkInformation::TransportMedium::Unknown: transportMedium = "UNKNOWN"; break;
    case QNetworkInformation::TransportMedium::Ethernet: transportMedium = "ETHERNET"; break;
    case QNetworkInformation::TransportMedium::Cellular: transportMedium = "CELLULAR"; break;
    case QNetworkInformation::TransportMedium::WiFi: transportMedium = "WIFI"; break;
    case QNetworkInformation::TransportMedium::Bluetooth: transportMedium = "BLUETOOTH"; break;
    default: break;
    }

    // transportMedium.append("\n(Can be UNKNOWN, ETHERNET, CELLULAR, WIFI, BLUETOOTH)");

    return transportMedium;
}
QString Network::captivePortal()
{
    if(!netInfo() || !netInfo()->supportedFeatures().testFlag(QNetworkInformation::Feature::CaptivePortal))
        return "N/A";

    return netInfo()->isBehindCaptivePortal() ? "YES" : "NO";
}
QString Network::metered()
{
    if(!netInfo() || !netInfo()->supportedFeatures().testFlag(QNetworkInformation::Feature::Metered))
        return "N/A";

    return netInfo()->isMetered() ? "YES" : "NO";
}

QString Network::getIPv4Address()
{
#ifndef QT_NO_NETWORKINTERFACE
    const QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface& iface : ifs)
    {
        if (iface.type()!=QNetworkInterface::Loopback && !iface.name().contains("usb") && iface.isValid() && iface.flags() & QNetworkInterface::IsUp)
        {
            const QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for(const QNetworkAddressEntry& entry : entries)
            {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    return entry.ip().toString();
            }
        }
    }
#endif
    return "N/A";
}

QString Network::getEthernetIPv4Address()
{
#ifndef QT_NO_NETWORKINTERFACE
    const QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface& iface : ifs)
    {
        if (iface.type()==QNetworkInterface::Ethernet && !iface.name().contains("usb") && iface.isValid() && iface.flags() & QNetworkInterface::IsUp)
        {
            const QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for(const QNetworkAddressEntry& entry : entries)
            {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    return entry.ip().toString();
            }
        }
    }
#endif
    return "N/A";
}

QString Network::getWifiIPv4Address()
{
#ifndef QT_NO_NETWORKINTERFACE
    const QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface& iface : ifs)
    {
        if (iface.type()==QNetworkInterface::Wifi && iface.isValid() && iface.flags() & QNetworkInterface::IsUp)
        {
            const QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for(const QNetworkAddressEntry& entry : entries)
            {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    return entry.ip().toString();
            }
        }
    }
#endif
    return "N/A";
}

QString Network::getUsbIPv4Address()
{
#ifndef QT_NO_NETWORKINTERFACE
    const QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface& iface : ifs)
    {
        if (iface.type()==QNetworkInterface::Ethernet && iface.name().contains("usb") && iface.isValid() && iface.flags() & QNetworkInterface::IsUp)
        {
            const QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for(const QNetworkAddressEntry& entry : entries)
            {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    return entry.ip().toString();
            }
        }
    }
#endif
    return "N/A";
}

QString Network::getHostname()
{
    return QHostInfo::localHostName();
}

QString Network::getDomain()
{
    return QHostInfo::localDomainName();
}

bool Network::waitForReachability(QNetworkInformation::Reachability reachability, int timeout)
{
    if(!netInfo() || !netInfo()->supportedFeatures().testFlag(QNetworkInformation::Feature::Reachability))
    {
        SOLIDLOG_WARNING()<<"Network reachability can not be determined";
        return true;
    }

    if(netInfo()->reachability()>=reachability)
    {
        SOLIDLOG_INFO()<<"Network reachability is already"<<netInfo()->reachability();
        return true;
    }

    QElapsedTimer elapsed;
    QTimer timer;
    timer.setInterval(timeout);
    timer.setSingleShot(true);

    QEventLoop loop;

    auto conn1 = connect(netInfo(), &QNetworkInformation::reachabilityChanged, netInfo(), [&](QNetworkInformation::Reachability newReachability) {
        if(!loop.isRunning())
            return;
        if(newReachability>=reachability)
            loop.exit(0);
    });

    auto conn2 = connect(&timer, &QTimer::timeout, netInfo(), [&]() {
        if(!loop.isRunning())
            return;
        if(netInfo()->reachability()>=reachability)
            loop.exit(0);
        else
            loop.exit(1);
    });

    elapsed.start();
    timer.start();
    int result = loop.exec();

    QObject::disconnect(conn1);
    QObject::disconnect(conn2);

    if(result==0)
        SOLIDLOG_INFO()<<"Network reachability changed to"<<netInfo()->reachability()<<"after"<<elapsed.nsecsElapsed()/1000000.0<<"ms";
    else
        SOLIDLOG_WARNING()<<"Network reachability is still"<<netInfo()->reachability()<<"after"<<elapsed.nsecsElapsed()/1000000.0<<"ms";

    return result==0;
}
