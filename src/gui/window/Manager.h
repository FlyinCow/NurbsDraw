#pragma once

#include <string>

class WindowManager {
public:
    static WindowManager &Get();
    WindowManager() noexcept;

    void register_window(std::string window_id);

private:
};