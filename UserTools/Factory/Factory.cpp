#include "Factory.h"
#include "Unity.h"

Tool* Factory(std::string tool) {
Tool* ret=0;

// if (tool=="Type") tool=new Type;
if (tool=="DummyTool") ret=new DummyTool;
if (tool=="Example") ret=new Example;
if (tool=="SendFakeData") ret=new SendFakeData;
  if (tool=="TriggerEventDisplay") ret=new TriggerEventDisplay;
return ret;
}
