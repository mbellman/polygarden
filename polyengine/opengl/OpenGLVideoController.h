#pragma once

#include <vector>
#include <map>

#include "SDL.h"
#include "subsystem/AbstractVideoController.h"
#include "opengl/ShaderProgram.h"
#include "opengl/OpenGLObject.h"
#include "opengl/OpenGLShadowCaster.h"
#include "opengl/FrameBuffer.h"
#include "opengl/OpenGLPostShader.h"
#include "opengl/OpenGLPreShader.h"
#include "opengl/GBuffer.h"
#include "opengl/SBuffer.h"
#include "opengl/PointShadowBuffer.h"
#include "subsystem/Geometry.h"
#include "subsystem/entities/Entity.h"
#include "subsystem/entities/Object.h"
#include "subsystem/entities/Light.h"
#include "subsystem/HeapList.h"
#include "glut.h"

class OpenGLVideoController final : public AbstractVideoController {
  friend class OpenGLIlluminator;

public:
  OpenGLVideoController();
  ~OpenGLVideoController();

  void onDestroy() override;
  void onInit(SDL_Window* sdlWindow, int width, int height) override;
  void onRender(SDL_Window* sdlWindow) override;
  void onSceneChange(AbstractScene* scene) override;
  void onScreenSizeChange(int width, int height) override;

private:
  SDL_GLContext glContext;
  GBuffer* gBuffer = nullptr;
  SBuffer* sBuffer = nullptr;
  PointShadowBuffer* pointShadowBuffer = nullptr;
  OpenGLIlluminator* glIlluminator = nullptr;
  HeapList<OpenGLPreShader> glPreShaders;
  HeapList<OpenGLPostShader> glPostShaders;
  HeapList<OpenGLObject> glObjects;
  HeapList<OpenGLShadowCaster> glShadowCasters;

  void createPostShaders();
  void createPreShaders();
  Matrix4 createViewMatrix();
  void onEntityAdded(Entity* entity);
  void onEntityRemoved(Entity* entity);
  void renderEmissiveSurfaces();
  void renderGeometry();
  void renderPostShaders();
  void renderPreShaders();
  void renderShadowCasters();
  void setObjectEffects(ShaderProgram& program, OpenGLObject* glObject);
};