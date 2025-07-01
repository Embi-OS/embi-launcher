#ifndef SNACKBARLOADER_H
#define SNACKBARLOADER_H

#include <QDefs>

class SnackbarLoader : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit SnackbarLoader(QObject* parent = nullptr);
    virtual ~SnackbarLoader();
};

#endif // SNACKBARLOADER_H
