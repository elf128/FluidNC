// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "../Configuration/Configurable.h"

namespace UI {
    class Connection;
    class Definition;

    class TouchDriver : public Configuration::Configurable {
        String _definitionsFilename;

        int    _x_left    = -1;
        int    _x_right   = -1;
        int    _y_up      = -1;
        int    _y_down    = -1;

        Connection* _connection = nullptr;
        Definition* _definition = nullptr;

    public:
        TouchDriver() = default;

        void init();
        
        void group(Configuration::HandlerBase& handler) override;
        void validate() override;
        void afterParse() override;

        ~TouchDriver() = default;
    };
}