#include "maclistmodel.h"

#include <QtCore/QStringBuilder>
#include <QtGui/QFont>
#include <QtWidgets/QApplication>
#ifdef _MSC_VER
#include "../constant.h"
#include "../converters.h"
#include "../customsnmpfunctions.h"
#else
#include "constant.h"
#include "converters.h"
#include "customsnmpfunctions.h"
#endif

// Columns
// DisplayRole
// 0 - number port
// 1 - vlan name
// 2 - mac

MacListModel::MacListModel(SwitchInfo::Ptr parentDevice, QObject *parent) :
    QAbstractTableModel(parent),
    mParentDevice(parentDevice)
{
}

int MacListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 3;
}

QVariant MacListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0) {
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        } else {
            return int(Qt::AlignCenter | Qt::AlignVCenter);
        }
    } else if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == 0) {
            return mMacList.at(index.row())->numberPort();
        } else if (index.column() == 1) {
            return mMacList.at(index.row())->vlanName();
        } else if (index.column() == 2) {
            return mMacList.at(index.row())->mac();
        } else {
            return QVariant();
        }
    }

    return QVariant();
}

QVariant MacListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (section == 0) {
            return MacListModelColumn::Port;
        } else if (section == 1) {
            return MacListModelColumn::Vlan;
        } else if (section == 2) {
            return MacListModelColumn::MacAddress;
        } else {
            return QVariant();
        }
    } else if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignCenter | Qt::AlignVCenter);
    } else if (role == Qt::FontRole) {
        QFont font(qApp->font());
        font.setBold(true);
        return font;
    }

    return QVariant();
}

Qt::ItemFlags MacListModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int MacListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return mMacList.size();
}

QString MacListModel::error() const
{
    return mError;
}

bool MacListModel::update()
{
    QScopedPointer<SnmpClient> snmp(new SnmpClient());

    snmp->setIp(mParentDevice->ip());

    if (!snmp->setupSession(SessionType::ReadSession)) {
        mError = SnmpErrors::SetupSession;
        return false;
    }

    if (!snmp->openSession()) {
        mError = SnmpErrors::OpenSession;
        return false;
    }

    beginResetModel();

    mMacList.clear();
    //TODO: Make handling errors
    updateMacInVlan(snmp, mParentDevice->inetVlanTag(), "Inet");
    updateMacInVlan(snmp, mParentDevice->iptvVlanTag(), "IPTV Unicast");

    endResetModel();

    return true;
}

void MacListModel::updateMacInVlan(QScopedPointer<SnmpClient> &snmpClient, long vlanTag, QString vlanName)
{
    oid *vlanMacOid = createOid(Mib::dot1qTpFdbPort, 13, vlanTag);
    size_t lenVlanNameOid = 14;

    snmpClient->createPdu(SNMP_MSG_GETNEXT);
    snmpClient->addOid(vlanMacOid, lenVlanNameOid);

    while (snmpClient->sendRequest()) {
        netsnmp_variable_list *vars = snmpClient->varList();

        if (snmp_oid_ncompare(vlanMacOid, lenVlanNameOid, vars->name, vars->name_length, lenVlanNameOid) != 0)
            break;

        MacInfo::Ptr macInfo = MacInfo::Ptr(new MacInfo());
        macInfo->setNumberPort(*(vars->val.integer));
        macInfo->setVlanName(vlanName);
        macInfo->setMac(decMacAddressToHex(vars->name, vars->name_length));

        mMacList.push_back(macInfo);

        snmpClient->createPduFromResponse(SNMP_MSG_GETNEXT);
    }
}
