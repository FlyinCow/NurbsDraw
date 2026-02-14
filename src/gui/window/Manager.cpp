#include "Manager.h"

WindowManager &WindowManager::Get() {
    static WindowManager instance;
    return instance;
}