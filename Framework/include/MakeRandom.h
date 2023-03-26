#pragma once
#include <random>
#include <cstdint>

static std::random_device   rd;
static std::mt19937         gen(rd());

// ƒNƒ\‚ª
#ifdef max
#undef max
#endif
static std::uniform_int_distribution<std::int64_t> distrib(0, std::numeric_limits<std::int64_t>::max());

static std::int64_t GetRandomId() {
    return distrib(gen);
}