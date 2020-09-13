#include <Window.h>
#include <opengl/OpenGLVideoController.h>

#include "GameController.h"
#include "GardenScene.h"

int main(int argc, char *argv[]) {
  Window window;

  window.open("Polygarden", { 100, 100, 1200, 720 });
  window.setVideoController(new OpenGLVideoController());
  window.setGameController(new GameController());
  window.run();

  return 0;
}