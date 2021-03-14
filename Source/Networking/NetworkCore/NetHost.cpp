// MIT Licensed (see LICENSE.md).
#include "Precompiled.hpp"

namespace Plasma
{

//                                    NetHost //

LightningDefineType(NetHost, builder, type)
{
  // Bind documentation
  PlasmaBindDocumented();

  // Bind methods
  LightningBindGetterProperty(RoundTripTime);
  LightningBindGetterProperty(Latency);

  // Bind member properties
  LightningBindFieldGetterProperty(mNetwork);
  LightningBindFieldGetterProperty(mIpAddress);
  // LightningBindFieldGetterProperty(mRoundTripTime);
  LightningBindFieldGetterProperty(mBasicHostInfo);
  LightningBindFieldGetterProperty(mExtraHostInfo);
}

NetHost::NetHost() : mNetwork(Network::LAN), mIpAddress(), mRoundTripTime(0), mBasicHostInfo(), mExtraHostInfo()
{
}
NetHost::NetHost(const IpAddress& ipAddress) :
    mNetwork(Network::LAN),
    mIpAddress(ipAddress),
    mRoundTripTime(0),
    mBasicHostInfo(),
    mExtraHostInfo()
{
}
NetHost::NetHost(const NetHost& rhs) :
    mNetwork(rhs.mNetwork),
    mIpAddress(rhs.mIpAddress),
    mRoundTripTime(rhs.mRoundTripTime),
    mBasicHostInfo(rhs.mBasicHostInfo),
    mExtraHostInfo(rhs.mExtraHostInfo)
{
}

NetHost::~NetHost()
{
}

bool NetHost::operator==(const NetHost& rhs) const
{
  return mIpAddress == rhs.mIpAddress;
}
bool NetHost::operator!=(const NetHost& rhs) const
{
  return mIpAddress != rhs.mIpAddress;
}
bool NetHost::operator<(const NetHost& rhs) const
{
  return mIpAddress < rhs.mIpAddress;
}
bool NetHost::operator==(const IpAddress& rhs) const
{
  return mIpAddress == rhs;
}
bool NetHost::operator!=(const IpAddress& rhs) const
{
  return mIpAddress != rhs;
}
bool NetHost::operator<(const IpAddress& rhs) const
{
  return mIpAddress < rhs;
}

uint NetHost::GetRoundTripTime() const
{
  return uint(mRoundTripTime);
}
uint NetHost::GetLatency() const
{
  return uint(mRoundTripTime / TimeMs(2));
}

//                                 NetHostRange //

NetHostRange::NetHostRange() : NetHostSet::range()
{
}
NetHostRange::NetHostRange(const NetHostSet::range& rhs) : NetHostSet::range(rhs)
{
}

} // namespace Plasma
