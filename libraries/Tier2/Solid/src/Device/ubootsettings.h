#ifndef UBOOTSETTINGS_H
#define UBOOTSETTINGS_H

#include <QDefs>
#include <QModels>
#include "qsingleton.h"

#include <QFutureWatcher>

class UBootSettings : public QObject,
                      public QQmlSingleton<UBootSettings>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool canPrintEnv READ canPrintEnv NOTIFY capabilitiesChanged FINAL)
    Q_PROPERTY(bool canSetEnv   READ canSetEnv   NOTIFY capabilitiesChanged FINAL)

protected:
    friend QQmlSingleton<UBootSettings>;
    explicit UBootSettings(QObject *parent = nullptr);

public:
    bool canPrintEnv();
    bool canSetEnv();

    Q_INVOKABLE QString printEnv(const QString& name);
    Q_INVOKABLE bool setEnv(const QString& name, const QString& value);
    Q_INVOKABLE bool clearEnv(const QString& name);

signals:
    void capabilitiesChanged();

private:
    bool m_canPrintEnv;
    bool m_canSetEnv;
};

class UBootEnvModel: public QVariantListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit UBootEnvModel(QObject* parent=nullptr);

protected:
    bool doSelect() override final;

    QPointer<QFutureWatcher<QVariantList>> m_selectWatcher;
};

#endif // UBOOTSETTINGS_H
