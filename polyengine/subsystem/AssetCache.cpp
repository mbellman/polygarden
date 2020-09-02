#include "subsystem/AssetCache.h"
#include "subsystem/Texture.h"

AssetCache::~AssetCache() {
  for (auto& [key, texture] : textureMap) {
    delete texture;
  }

  textureMap.clear();
}

void AssetCache::addTexture(std::string path) {
  auto* texture = new Texture(path);

  textureMap.emplace(path, texture);
}

const Texture* AssetCache::createTexture(std::string path) {
  if (textureMap.find(path) == textureMap.end()) {
    addTexture(path);
  }

  return textureMap.at(path);
}