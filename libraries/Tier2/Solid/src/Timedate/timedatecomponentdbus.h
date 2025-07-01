#ifndef TIMEDATECOMPONENTDBUS_H
#define TIMEDATECOMPONENTDBUS_H

#include <QtDBus>

class OrgFreedesktopTimedate1Interface : public QDBusAbstractInterface
{
    Q_OBJECT
    Q_PROPERTY(bool CanNTP READ canNTP CONSTANT FINAL)
    Q_PROPERTY(bool LocalRTC READ localRTC CONSTANT FINAL)
    Q_PROPERTY(bool NTP READ nTP CONSTANT FINAL)
    Q_PROPERTY(QString Timezone READ timezone CONSTANT FINAL)

public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.timedate1"; }

    OrgFreedesktopTimedate1Interface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr):
        QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {}

    inline bool canNTP() const { return qvariant_cast< bool >(property("CanNTP")); }
    inline bool localRTC() const { return qvariant_cast< bool >(property("LocalRTC")); }
    inline bool nTP() const { return qvariant_cast< bool >(property("NTP")); }
    inline QString timezone() const { return qvariant_cast< QString >(property("Timezone")); }

public Q_SLOTS: // METHODS
    inline QDBusMessage SetLocalRTC(bool local_rtc, bool fix_system, bool user_interaction) {
        QVariantList argumentList;
        argumentList<<QVariant::fromValue(local_rtc)<<QVariant::fromValue(fix_system)<<QVariant::fromValue(user_interaction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetLocalRTC"), argumentList);
    }

    inline QDBusMessage SetNTP(bool use_ntp, bool user_interaction) {
        QVariantList argumentList;
        argumentList<<QVariant::fromValue(use_ntp)<<QVariant::fromValue(user_interaction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetNTP"), argumentList);
    }

    inline QDBusMessage SetTime(qlonglong usec_utc, bool relative, bool user_interaction) {
        QVariantList argumentList;
        argumentList<<QVariant::fromValue(usec_utc)<<QVariant::fromValue(relative)<<QVariant::fromValue(user_interaction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetTime"), argumentList);
    }

    inline QDBusMessage SetTimezone(const QString &timezone, bool user_interaction) {
        QVariantList argumentList;
        argumentList<<QVariant::fromValue(timezone)<<QVariant::fromValue(user_interaction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetTimezone"), argumentList);
    }
};

namespace org {
    namespace freedesktop {
        typedef ::OrgFreedesktopTimedate1Interface timedate1;
    }
}

#include "abstracttimedatecomponent.h"
class TimedateComponentDBus : public AbstractTimedateComponent
{
public:
    TimedateComponentDBus();

    int getCapabilities() override;

    QString getTimezone() const override;
    bool setTimezone(const QString& aTimeZone) override;

    bool getNtp() const override;
    bool setNtp(bool aNtp) override;

    bool setSystemTime(const QDateTime& aTime) override;

private:
    OrgFreedesktopTimedate1Interface *m_interface;
};

#endif // TIMEDATECOMPONENTDBUS_H
