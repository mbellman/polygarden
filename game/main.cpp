#include <Window.h>
#include <opengl/OpenGLVideoController.h>

#include "GardenScene.h"

int main(int argc, char *argv[]) {
  Window window;

  auto* controller = new OpenGLVideoController();

  controller->setScene(new GardenScene());

  window.setVideoController(controller);
  window.open("Polygarden", { 100, 100, 1200, 720 });
  window.run();

  return 0;
}