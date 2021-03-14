// MIT Licensed (see LICENSE.md).
#include "Precompiled.hpp"

namespace Plasma
{
namespace Events
{
DefineEvent(ScriptInitialize);
DefineEvent(PostImport);
} // namespace Events

LightningDefineType(PostImportEvent, builder, type)
{

}

LightningDefineType(TextEvent, builder, type)
{
  LightningBindFieldProperty(Text);
}

LightningDefineType(TextErrorEvent, builder, type)
{
}

LightningDefineType(ProgressEvent, builder, type)
{
}

LightningDefineType(BlockingTaskEvent, builder, type)
{
}

ProgressEvent::ProgressEvent()
{
  ProgressType = ProgressType::Normal;
  Percentage = 0.0f;
}

} // namespace Plasma
