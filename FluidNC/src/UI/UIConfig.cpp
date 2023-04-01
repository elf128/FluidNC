// Copyright (c) 2021 -  Stefan de Bruijn
// Copyright (c) 2021 -  Mitch Bradley
// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "UIConfig.h"

#include "LcdDriver.h"
#include "TouchDriver.h"

#include "../SettingsDefinitions.h"  // ui_config_filename
#include "../FileStream.h"

#include <cstdio>
#include <cstring>

namespace UI {
    char _name_[] = "ui";

    void UIConfig::group(Configuration::HandlerBase& handler) {
        handler.item("screen", _screen);
        handler.item("version", _version);

        handler.section("lcd_driver",   _lcdDriver);
        handler.section("touch_driver", _touchDriver);
    }

    void UIConfig::afterParse() {
    }

    bool UIConfig::load() {
        return ConfigRoot::load( ui_config_filename->get(), name() );
    }

    void UIConfig::cleanup() {
        if ( _lcdDriver ) {
            delete _lcdDriver;
            _lcdDriver = nullptr;
        }

        if ( _touchDriver ) {
            delete _touchDriver;
            _touchDriver = nullptr;
        }

        if ( _uiLayout ) {
//            delete _uiLayout;
            _uiLayout = nullptr;
        }
    }

    const char* UIConfig::name() {
        return _name_;
    }


    UIConfig::~UIConfig() {
        cleanup();
    }
}
