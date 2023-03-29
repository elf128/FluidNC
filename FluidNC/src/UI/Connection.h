// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "../Configuration/Configurable.h"

//namespace Machine {
//    class SPIBus;
//}

namespace UI {
    class SharedSPI;

    class Connection : public Configuration::Configurable {
        Pin      _dc_pin;
        Pin      _rst_pin;
        Pin      _en_pin;

        pinnum_t _dc = 255;
        pinnum_t _rst = 255;
        pinnum_t _en = 255;

//        Machine::SPIBus* _spi       = nullptr;
        SharedSPI*       _sharedSpi = nullptr;

    public:
        Connection() = default;

        void init();
//        void deinit();

        void group(Configuration::HandlerBase& handler) override;
        void validate() override;
        void afterParse() override;

        ~Connection() = default;
    };
}
