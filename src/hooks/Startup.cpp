#include "../managers/API.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    API::getModerators([](bool) {});
}