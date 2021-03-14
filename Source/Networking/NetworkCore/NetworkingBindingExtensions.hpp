// MIT Licensed (see LICENSE.md).
#pragma once

namespace Plasma
{

// Meta Net Property
class MetaNetProperty : public MetaAttribute
{
public:
  LightningDeclareType(MetaNetProperty, TypeCopyMode::ReferenceType);

  /// The net property type name.
  String mNetPropertyConfig;

  /// Desired net channel name.
  String mNetChannelConfig;
};

} // namespace Plasma
