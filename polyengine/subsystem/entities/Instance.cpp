#include "subsystem/entities/Instance.h"

Instance::~Instance() {
  reference->untrackInstance(this);
}

void Instance::from(Object* reference) {
  this->reference = reference;

  reference->trackInstance(this);
}