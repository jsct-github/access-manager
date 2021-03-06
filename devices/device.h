#ifndef DEVICE_H
#define DEVICE_H

#include <customtypes.h>
#include <snmpclient.h>

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = 0);
    Device(QString name, QString ip, QObject *parent = 0);
    explicit Device(Device *source, QObject *parent = 0);

    QString name() const;
    void setName(const QString name);

    QString ip() const;
    void setIp(const QString ip);

    virtual DeviceModel::Enum deviceModel() const;
    virtual DeviceType::Enum deviceType() const;

    QString error() const;

    virtual bool getServiceDataFromDevice();

    virtual int  maxLengthPortDescription();
    virtual bool setPortDescription(long snmpPortIndex, QString description);

    typedef QSharedPointer<Device> Ptr;

signals:
    void modified();

private:
    QString mIp;
    QString mName;

protected:
    QString mError;
};



#endif // DEVICE_H
