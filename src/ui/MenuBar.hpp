#pragma once

#include "../system/AssetManager.hpp"

struct MenuBarAction {
    bool openIdleEditor = false;
    bool openTalkEditor = false;
    bool openScreamEditor = false;
    bool openShakeSettings = false;
    bool openShaderEditor = false;
};

class MenuBar {
public:
    MenuBarAction Draw();
};