// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "../Configuration/Configurable.h"

namespace UI {
    class SharedSPI : public Configuration::Configurable {
        Pin    _csPin;

        int _mainClk;
        int _readClk;

        int _hostId = 0;

    public:
        SharedSPI() = default;

        void init();
        
        void group(Configuration::HandlerBase& handler) override;
        void validate() override;
        void afterParse() override;

        ~SharedSPI() = default;
    };
}