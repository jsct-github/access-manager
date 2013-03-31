#ifndef OLTINFOLTP8X_H
#define OLTINFOLTP8X_H

#include "oltinfo.h"

class OltInfoLtp8x : public OltInfo
{
    Q_OBJECT
public:
    explicit OltInfoLtp8x(QObject *parent = 0);
    OltInfoLtp8x(QString name, QString ip, QObject *parent = 0);

    DeviceModel::Enum deviceModel() const;
};

#endif // OLTINFOLTP8X_H