#ifndef NETWORKSETTINGSADDRESSMODEL_H
#define NETWORKSETTINGSADDRESSMODEL_H

#include <QDefs>
#include <QStringListModel>

class NetworkSettingsAddressModel : public QStringListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)

public:
    explicit NetworkSettingsAddressModel(QObject *parent = nullptr);
    explicit NetworkSettingsAddressModel(const QStringList &strings, QObject *parent = nullptr);
    Q_INVOKABLE void append(const QString& address);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void resetChanges();
    void setStringList(const QStringList &addresses);
    int count() const;

Q_SIGNALS:
    void countChanged();
private:
    QStringList m_addresses;
};

#endif // NETWORKSETTINGSADDRESSMODEL_H
