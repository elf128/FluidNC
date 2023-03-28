// Copyright (c) 2021 -  Stefan de Bruijn
// Copyright (c) 2021 -  Mitch Bradley
// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "../Assert.h"
#include "../Configuration/GenericFactory.h"
#include "../Configuration/HandlerBase.h"
#include "../Configuration/Configurable.h"
/*
#include "../CoolantControl.h"
#include "../Kinematics/Kinematics.h"
#include "../WebUI/BTConfig.h"
#include "../Control.h"
#include "../Probe.h"
#include "src/Parking.h"
#include "../SDCard.h"
#include "../Spindles/Spindle.h"
#include "../Stepping.h"
#include "../Stepper.h"
#include "../Config.h"
#include "../OLED.h"
#include "Axes.h"
#include "SPIBus.h"
#include "I2CBus.h"
#include "I2SOBus.h"
#include "UserOutputs.h"
#include "Macros.h"
*/
namespace UI {
    //using ::Kinematics::Kinematics;
    class LcdDriver;
    class TouchDriver;

    class UIConfig : public Configuration::Configurable {
    public:
        UIConfig() = default;

        LcdDriver*              _lcdDriver      = nullptr;
        TouchDriver*            _touchDriver    = nullptr;

        //Kinematics*           _kinematics     = nullptr;
        //SPIBus*               _spi            = nullptr;
        //I2CBus*               _i2c[MAX_N_I2C] = { nullptr };

        String _screen = "Unknown";
        String _version  = "0.0.0";
#if 1
        static UIConfig*& instance() {
            static UIConfig* instance = nullptr;
            return instance;
        }
#endif

        void afterParse() override;
        void group(Configuration::HandlerBase& handler) override;

        static bool load();
        static bool load(const char* file);
        static bool load(StringRange* input);

        ~UIConfig();
    };
}

extern UI::UIConfig* ui;
