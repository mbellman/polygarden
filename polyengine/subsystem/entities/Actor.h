#pragma once

#include <vector>

#include "subsystem/traits/LifeCycle.h"
#include "subsystem/traits/Transformable.h"

class Stage;

class Actor : public LifeCycle, public Transformable {
public:
  virtual ~Actor();

  virtual void onAdded() {};
  virtual void onRegistered() {};
  void setOrientation(const Vec3f& orientation) override;
  void setPosition(const Vec3f& position) override;
  void setScale(const Vec3f& scale) override;
  virtual void setStage(Stage* stage) final;

protected:
  Stage* stage = nullptr;

  void addPositionable(Positionable* positionable);
  void addTransformable(Transformable* transformable);

private:
  std::vector<Positionable*> positionables;
  std::vector<Orientable*> orientables;
  std::vector<Scalable*> scalables;
};