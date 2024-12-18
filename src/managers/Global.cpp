#include "Global.hpp"

Global* Global::instance = nullptr;
std::mutex Global::mutex;
