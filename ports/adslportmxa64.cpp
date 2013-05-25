#include "adslportmxa64.h"

#include <constant.h>
#include <converters.h>
#include <customsnmpfunctions.h>
#include <configs/dslamprofileconfig.h>

AdslPortMxa64::AdslPortMxa64(long index, QObject *parent) :
    AdslPort(index, parent)
{
}

void AdslPortMxa64::fillPrimaryLevelPdu(SnmpClient::Ptr snmpClient, long portIndex)
{
    if (portIndex == -1)
        portIndex = mIndex;

    snmpClient->addOid(createOidPair(Mib::mxa64DslPortOperStatus, 13,  portIndex));
    snmpClient->addOid(createOidPair(Mib::mxa64DslPortAdminStatus, 13, portIndex));
    snmpClient->addOid(createOidPair(Mib::mxa64DslPortName, 13, portIndex));
    snmpClient->addOid(createOidPair(Mib::mxa64DslPortActiveProfile, 13, portIndex));
}

bool AdslPortMxa64::parsePrimaryLevelPdu(SnmpClient::Ptr snmpClient)
{
    mName = QString("adsl %1").arg(mIndex);

    netsnmp_variable_list *vars = snmpClient->varList();

    if (isValidSnmpValue(vars)
            && isValidSnmpValue(vars->next_variable)) {
        DslPortState::Enum operStatus = DslPortState::from(*vars->val.integer);
        DslPortState::Enum adminStatus = DslPortState::from(*vars->next_variable->val.integer);

        mState = DslPortState::from(operStatus, adminStatus);
    } else {
        return false;
    }

    vars = vars->next_variable->next_variable;
    if (!isValidSnmpValue(vars))
        return false;

    mDescription = toString(vars->val.string, vars->val_len);

    vars = vars->next_variable;
    if (!isValidSnmpValue(vars))
        return false;

    mProfile = QString::number(*vars->val.integer);
    mProfile = DslamProfileConfig::adsl(DeviceModel::MXA64)->displayProfileName(mProfile);

    return true;
}

void AdslPortMxa64::fillSecondaryLevelPdu(SnmpClient::Ptr snmpClient, long portIndex)
{
    if (portIndex == -1)
        portIndex = mIndex;

    snmpClient->addOid(createOidPair(Mib::mxa64DslPortOperStatus, 13, portIndex));
    snmpClient->addOid(createOidPair(Mib::mxa64DslPortAdminStatus, 13, portIndex));
    snmpClient->addOid(createOidPair(Mib::mxa64DslBandActualRateTx, 14, portIndex));
    snmpClient->addOid(createOidPair(Mib::mxa64DslBandActualRateRx, 14, portIndex));
    snmpClient->addOid(createOidPair(Mib::mxa64DslBandLineAttenuationTx, 14, portIndex));
    snmpClient->addOid(createOidPair(Mib::mxa64DslBandLineAttenuationRx, 14, portIndex));
    snmpClient->addOid(createOidPair(Mib::mxa64DslPortActiveProfile, 13, portIndex));
}

bool AdslPortMxa64::parseSecondaryLevelPdu(SnmpClient::Ptr snmpClient)
{
    netsnmp_variable_list *vars = snmpClient->varList();

    if (isValidSnmpValue(vars)
            && isValidSnmpValue(vars->next_variable)) {
        DslPortState::Enum operStatus = DslPortState::from(*vars->val.integer);
        DslPortState::Enum adminStatus = DslPortState::from(*vars->next_variable->val.integer);

        mState = DslPortState::from(operStatus, adminStatus);
    } else {
        return false;
    }

    if (mState != DslPortState::Up) {
        mTxRate = 0;
        mRxRate = 0;
        mTxAttenuation = "";
        mRxAttenuation = "";
        vars = vars->next_variable->next_variable->next_variable->next_variable->next_variable;
    } else {
        vars = vars->next_variable->next_variable;
        if (!isValidSnmpValue(vars))
            return false;

        mTxRate = *vars->val.integer / 1000;

        vars = vars->next_variable;
        if (!isValidSnmpValue(vars))
            return false;

        mRxRate = *vars->val.integer / 1000;

        vars = vars->next_variable;
        if (!isValidSnmpValue(vars))
            return false;

        mTxAttenuation = QString::number(*vars->val.integer);

        vars = vars->next_variable;
        if (!isValidSnmpValue(vars))
            return false;

        mRxAttenuation = QString::number(*vars->val.integer);
    }

    vars = vars->next_variable;
    if (!isValidSnmpValue(vars))
        return false;

    mProfile = QString::number(*vars->val.integer);
    mProfile = DslamProfileConfig::adsl(DeviceModel::MXA64)->displayProfileName(mProfile);

    return true;
}
