// Copyright (c) 2018 -	Bart Dring
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "Config.h"

#include "SDCard.h"
#include "Machine/MachineConfig.h"
#include "Channel.h"
#include "Report.h"

#include "Driver/sdspi.h"
#include "src/SettingsDefinitions.h"
#include "FluidPath.h"

SDCard::SDCard() : _state(State::Idle) {}

void SDCard::init() {
    static bool init_message = true;  // used to show messages only once.
    pinnum_t    csPin;
    int         csFallback;
    if (_hostId < 0 || _hostId >= MAX_N_SPI ) {
        log_error("spi number for SD card is out of range. ");
        return;
    }

    if (_cs.defined()) {
        if (!config->_spi[_hostId]->defined()) {
            log_error("SD needs SPI defined");
        } else {
            log_info("SD Card cs_pin:" << _cs.name() << " detect:" << _cardDetect.name() << " freq:" << _frequency_hz);
            init_message = false;
        }
        _cs.setAttr(Pin::Attr::Output);
        csPin = _cs.getNative(Pin::Capabilities::Output | Pin::Capabilities::Native);
    } else if ((csFallback = sd_fallback_cs->get()) != -1) {
        csPin = static_cast<pinnum_t>(csFallback);
        log_info("Using fallback CS pin " << int(csPin));
    } else {
        log_debug("See http://wiki.fluidnc.com/en/config/sd_card#sdfallbackcs-access-sd-without-a-config-file");
        return;
    }

    if (_cardDetect.defined()) {
        _cardDetect.setAttr(Pin::Attr::Input);
        auto cdPin = _cardDetect.getNative(Pin::Capabilities::Input | Pin::Capabilities::Native);
        sd_init_slot(_frequency_hz, csPin, _hostId + 1, cdPin); // busId + 1 since we don't want to use spi host dedicated for flash memory
    } else {
        sd_init_slot(_frequency_hz, csPin, _hostId + 1 ); // busId + 1 since we don't want to use spi host dedicated for flash memory
    }
}

void SDCard::afterParse() {
    // if (_cs.undefined()) {
    //     _cs = Pin::create("gpio.5");
    // }
}

SDCard::~SDCard() {}
