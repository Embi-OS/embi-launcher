#include "timedatecomponentdbus.h"

TimedateComponentDBus::TimedateComponentDBus() :
    AbstractTimedateComponent()
{
    m_interface = new OrgFreedesktopTimedate1Interface(QStringLiteral("org.freedesktop.timedate1"),
                                                       QStringLiteral("/org/freedesktop/timedate1"),
                                                       QDBusConnection::systemBus());

}

int TimedateComponentDBus::getCapabilities()
{
    return Capabilities::Timezone |
           Capabilities::Ntp |
           Capabilities::SystemDateTime;
}

QString TimedateComponentDBus::getTimezone() const
{
    if(!m_interface)
        return QString();

    return m_interface->timezone();
}

bool TimedateComponentDBus::setTimezone(const QString& aTimeZone)
{
    if(!m_interface)
        return false;

    do {
        m_interface->SetTimezone(aTimeZone, true);
    } while(getTimezone() != aTimeZone);

    return true;
}

bool TimedateComponentDBus::getNtp() const
{
    if(!m_interface)
        return false;

    return m_interface->nTP();
}

bool TimedateComponentDBus::setNtp(const bool aNtp)
{
    if(!m_interface)
        return false;

    do {
        m_interface->SetNTP(aNtp, true);
    } while(getNtp() != aNtp);

    return true;
}

bool TimedateComponentDBus::setSystemTime(const QDateTime& aTime)
{
    if(!m_interface)
        return false;

    qlonglong usecsSinceEpoch = aTime.toMSecsSinceEpoch()*1000;
    m_interface->SetTime(usecsSinceEpoch, false, true);

    return true;
}



