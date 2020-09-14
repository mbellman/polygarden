#pragma once

#include "subsystem/entities/Object.h"
#include "subsystem/entities/Light.h"
#include "subsystem/traits/LifeCycle.h"

class Stage;

class Actor : public LifeCycle {
public:
  virtual ~Actor() {};

  virtual void onRegistered() {};
  virtual void setStage(Stage* stage) final;

protected:
  Stage* stage = nullptr;
};