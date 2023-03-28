// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "../Configuration/Configurable.h"

namespace UI {
    class Connection;
    class Definition;

    class LcdDriver : public Configuration::Configurable {
        String _definitionsFilename;

        int    _width    = -1;
        int    _height   = -1;
        int    _rotate   = -1;
        int    _rgbOrder = -1;

        Connection* _connection = nullptr;
        Definition* _definition = nullptr;

    public:
        LcdDriver() = default;

        void init();
        
        void group(Configuration::HandlerBase& handler) override;
        void validate() override;
        void afterParse() override;

        ~LcdDriver() = default;
    };
}