// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "Connection.h"
#include "../Machine/SPIBus.h"
#include "SharedSPI.h"

namespace UI {
    void Connection::init() {
        if ( _spi ) _spi->init();

        if ( _dc_pin.defined() ) {
            log_info(" LCD connection DC: " << _dc_pin.name() );
            _dc  = _dc_pin.getNative(Pin::Capabilities::Output | Pin::Capabilities::Native);
        }

        if ( _rst_pin.defined() ) {
            log_info(" LCD connection RST: " << _rst_pin.name() );
            _rst  = _rst_pin.getNative(Pin::Capabilities::Output | Pin::Capabilities::Native);
        }

        if ( _en_pin.defined() ) {
            log_info(" LCD connection EN: " << _en_pin.name() );
            _en  = _en_pin.getNative(Pin::Capabilities::Output | Pin::Capabilities::Native);
        }
    }

    void Connection::deinit() {
        if ( _spi ) _spi->deinit();
    }

    void Connection::group(Configuration::HandlerBase& handler) {
        handler.item("dc_pin",  _dc_pin );
        handler.item("rst_pin", _rst_pin);
        handler.item("en_pin",  _en_pin );

        handler.section("spi", _spi, 1 );
    }

    void Connection::validate() {
        Assert( _spi || _sharedSpi, "Currently, only SPI drivers are supported for LCD, but none defined");
    }

    void Connection::afterParse() {

    }
}