#pragma once

#include <string_view>
#include <cstddef>
#include <cstdint>

namespace static_assets {

struct EmbeddedAsset {
    const uint8_t* data;
    size_t size;
};

class EmbeddedAssetTable {
public:
    const EmbeddedAsset* find(std::string_view path) const;
    const uint8_t* find(const char* path, size_t& outSize) const;
};

} // namespace static_assets