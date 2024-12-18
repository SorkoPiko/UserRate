#include "../managers/Global.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    Global::getInstance()->checkAccountStatus();
}