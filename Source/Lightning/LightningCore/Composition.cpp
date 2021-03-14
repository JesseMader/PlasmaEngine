// MIT Licensed (see LICENSE.md).

#include "Precompiled.hpp"

namespace Lightning
{
Type* TypeHelperGetBaseType(Type* derivedType)
{
  return Type::GetBaseType(derivedType);
}

Composition::Composition()
{
}

Composition::~Composition()
{
}

Composition* Composition::GetBaseComposition()
{
  return nullptr;
}

void Composition::ClearComponents()
{
  this->Components.Clear();
}
} // namespace Lightning
