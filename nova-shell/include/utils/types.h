#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include "utils/ordered_map.hpp"


// Aliases
using umap_str = std::unordered_map<std::string, std::string>;
using omap_str = OrderedMap<std::string, std::string>;
using set_str = std::unordered_set<std::string>;
using vec_str   = std::vector<std::string>;
