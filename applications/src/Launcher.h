#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <Axion>

class Launcher : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(Launcher)

private:
    explicit Launcher(QObject *parent = nullptr);

public:
    bool init() final override;

    Q_INVOKABLE void install();
    Q_INVOKABLE void launch();
};

#endif // LAUNCHER_H
