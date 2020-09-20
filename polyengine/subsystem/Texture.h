#pragma once

#include <string>

#include "SDL_image.h"
#include "subsystem/AssetCache.h"

class Texture {
public:
  Texture(std::string path);
  ~Texture();

  static void freeCache();
  static Texture* use(std::string);

  const SDL_Surface* getData() const;
  const std::string& getPath() const;
  int getId() const;

private:
  static int total;
  static AssetCache<Texture> textureCache;

  std::string path;
  SDL_Surface* surface = nullptr;
  int id;
};