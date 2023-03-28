// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "LcdDriver.h"
#include "Connection.h"

namespace UI {
    void LcdDriver::init() {
        if ( _connection ) _connection->init();
        //if ( _definition ) _definition->init();
    }

    void LcdDriver::group(Configuration::HandlerBase& handler) {
        handler.item("definition", _definitionsFilename );
        handler.item("width", _width, 1, 480 );
        handler.item("height", _height, 1, 320 );
        handler.item("rgb_order", _rgbOrder, 0, 1 );
        handler.section("connection", _connection );
    }

    void LcdDriver::validate() {
        
    }

    void LcdDriver::afterParse() {
        log_info( " Use driver definition from " << _definitionsFilename );
    }
}