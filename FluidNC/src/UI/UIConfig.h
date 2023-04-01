// Copyright (c) 2021 -  Stefan de Bruijn
// Copyright (c) 2021 -  Mitch Bradley
// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "../Assert.h"
#include "../Configuration/ConfigRoot.h"

namespace UI {
    class LcdDriver;
    class TouchDriver;
    class Layout;

    class UIConfig : public Configuration::ConfigRoot {
    public:
        UIConfig() = default;

        LcdDriver*   _lcdDriver      = nullptr;
        TouchDriver* _touchDriver    = nullptr;
        Layout*      _uiLayout       = nullptr;

        String _screen = "Unknown";
        String _version  = "0.0.0";

#if 1
        static UIConfig*& instance() {
            static UIConfig* instance = nullptr;

            if ( instance == nullptr )
                instance = new UIConfig();

            return instance;
        }
#endif

        void afterParse() override;
        void group(Configuration::HandlerBase& handler) override;

        bool load() override;
        void cleanup() override;
        const char* name() override;

        ~UIConfig();
    };
}

//extern UI::UIConfig* ui;
