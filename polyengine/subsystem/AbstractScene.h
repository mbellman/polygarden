#pragma once

#include <vector>
#include <functional>

#include "subsystem/Stage.h"
#include "subsystem/AssetCache.h"
#include "subsystem/entities/Entity.h"
#include "subsystem/entities/Camera.h"
#include "subsystem/traits/LifeCycle.h"
#include "subsystem/InputSystem.h"
#include "subsystem/Types.h"

class AbstractScene : public LifeCycle {
public:
  virtual ~AbstractScene() {};

  const Camera& getCamera() const;
  virtual InputSystem& getInputSystem() final;
  virtual const Stage& getStage() const final;
  void onEntityAdded(Callback<Entity*> handler);
  void onEntityRemoved(Callback<Entity*> handler);
  virtual void onUpdate(float dt) override;

protected:
  using super = AbstractScene;

  Stage stage;
  InputSystem input;
  Camera camera;
};