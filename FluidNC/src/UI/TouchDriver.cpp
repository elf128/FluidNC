// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "TouchDriver.h"
#include "Connection.h"

namespace UI {
    void TouchDriver::init() {
        if ( _connection ) _connection->init();
        //if ( _definition ) _definition->init();
    }

    void TouchDriver::group(Configuration::HandlerBase& handler) {
        handler.item("definition", _definitionsFilename );

        handler.item("x_left",     _x_left,  -1000, 1000 );
        handler.item("x_right",    _x_right, -1000, 1000 );
        handler.item("y_up",       _y_up,    -1000, 1000 );
        handler.item("y_down",     _y_down,  -1000, 1000 );

        handler.section("connection", _connection );
    }

    void TouchDriver::validate() {
        
    }

    void TouchDriver::afterParse() {
        log_info( " Use driver definition from " << _definitionsFilename );
    }
}