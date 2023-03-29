// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "SharedSPI.h"

namespace UI {
    void SharedSPI::init() {
    }

    void SharedSPI::group(Configuration::HandlerBase& handler) {
        handler.item("cs_pin",         _csPin );
        handler.item("frequency",      _mainClk, 20000, 160000000 );
        handler.item("read_frequency", _readClk, 20000, 160000000 );
        handler.item("spi_host", _hostId, 0, MAX_N_SPI);
    }

    void SharedSPI::validate() {        
    }

    void SharedSPI::afterParse() {
    }
}
