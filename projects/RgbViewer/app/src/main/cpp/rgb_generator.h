#pragma once
#include <vector>
#include <cstdint>

#include "logging.h"

static const std::vector<uint8_t> RED = {0xff, 0, 0, 0xff};
static const std::vector<uint8_t> GREEN = {0, 0xff, 0, 0xff};
static const std::vector<uint8_t> BLUE = {0, 0, 0xff, 0xff};

class RgbGenerator {
public:
    RgbGenerator(int width, int height) {
        if (width <= 0 || height <= 0) {
            LOGE("invalid parameters");
        }

        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < height; ++j) {
                if (j < height / 3) {
                    data_.insert(data_.end(), RED.begin(), RED.end());
                } else if (j >= height / 3 && j < height * 2 / 3) {
                    data_.insert(data_.end(), GREEN.begin(), GREEN.end());
                } else {
                    data_.insert(data_.end(), BLUE.begin(), BLUE.end());
                }
            }
        }
    }

    ~RgbGenerator() = default;

    std::vector<uint8_t> data() const { return data_; }

private:
    std::vector<uint8_t> data_;
};