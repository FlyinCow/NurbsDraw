#include "Manager.h"

WindowManager::WindowManager() noexcept {
}

WindowManager &WindowManager::Get() {
    static WindowManager instance;
    return instance;
}