#pragma once

#include <map>
#include <string>

#include "subsystem/Texture.h"

template<typename T>
class AssetCache {
public:
  ~AssetCache() {
    free();
  }

  void free() {
    for (auto& [ key, asset ] : assetMap) {
      delete asset;
    }

    assetMap.clear();
  }

  T* request(std::string path) {
    if (assetMap.find(path) == assetMap.end()) {
      createAsset(path);
    }

    return assetMap[path];
  }

private:
  std::map<std::string, T*> assetMap;

  void createAsset(std::string path) {
    auto* asset = new T(path);

    assetMap[path] = asset;
  }
};