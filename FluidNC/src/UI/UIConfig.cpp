// Copyright (c) 2021 -  Stefan de Bruijn
// Copyright (c) 2021 -  Mitch Bradley
// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "UIConfig.h"

#include "LcdDriver.h"
#include "TouchDriver.h"

#include "../SettingsDefinitions.h"  // ui_config_filename
#include "../FileStream.h"

#include "../Configuration/Parser.h"
#include "../Configuration/ParserHandler.h"
#include "../Configuration/Validator.h"
#include "../Configuration/AfterParse.h"
#include "../Configuration/ParseException.h"
#include "../Config.h"  // ENABLE_*

#include <cstdio>
#include <cstring>
#include <atomic>

UI::UIConfig* ui;

namespace UI {
    void UIConfig::group(Configuration::HandlerBase& handler) {
        handler.item("screen", _screen);
        handler.item("version", _version);

        handler.section("lcd_driver",   _lcdDriver);
        handler.section("touch_driver", _touchDriver);
        //handler.section("lcd_driver", _lcdDriver);

    }

    void UIConfig::afterParse() {
        //if (_axes == nullptr) {
        //    log_info("Axes: using defaults");
        //    _axes = new Axes();
        //}
    }

    char defaultUI[] = "name: Unknown \nboard: 0.0.0\n";

    bool UIConfig::load() {
        bool configOkay;
        // If the system crashes we skip the config file and use the default
        // builtin config.  This helps prevent reset loops on bad config files.
        esp_reset_reason_t reason = esp_reset_reason();
        if (reason == ESP_RST_PANIC) {
            log_error("Skipping ui configuration file due to panic");
            configOkay = false;
        } else {
            configOkay = load( ui_config_filename->get() );
        }

        if (!configOkay) {
            log_info("Using default configuration");
            configOkay = load(new StringRange(defaultUI));
        }

        return configOkay;
    }

    bool UIConfig::load( const char* filename ) {
        try {
            FileStream file(filename, "r", "");

            auto filesize = file.size();
            if (filesize <= 0) {
                log_info("UI configuration file:" << filename << " is empty");
                return false;
            }

            char* buffer     = new char[filesize + 1];
            buffer[filesize] = '\0';
            auto actual      = file.read(buffer, filesize);
            if (actual != filesize) {
                log_info("UI configuration file:" << filename << " read error");
                return false;
            }
            log_info("UI configuration file:" << filename);
            bool retval = load(new StringRange(buffer, buffer + filesize));
            delete[] buffer;
            return retval;
        } catch (...) {
            log_warn("Cannot open UI configuration file:" << filename);
            return false;
        }
    }

    bool UIConfig::load( StringRange* input ) {
        bool successful = false;
        try {
            Configuration::Parser        parser(input->begin(), input->end());
            Configuration::ParserHandler handler(parser);

            // instance() is by reference, so we can just get rid of an old instance and
            // create a new one here:
            {
                auto& uiConfig = instance();
                if (uiConfig != nullptr) {
                    delete uiConfig;
                }
                uiConfig = new UIConfig();
            }
            ui = instance();

            handler.enterSection("machine", ui);

            log_debug("Running after-parse tasks");

            try {
                Configuration::AfterParse afterParse;
                ui->afterParse();
                ui->group(afterParse);
            } catch (std::exception& ex) { log_info("UI validation error: " << ex.what()); }

            log_debug("Checking ui configuration");

            try {
                Configuration::Validator validator;
                ui->validate();
                ui->group(validator);
            } catch (std::exception& ex) { log_info("UI validation error: " << ex.what()); }

            // log_info("Heap size after configuation load is " << uint32_t(xPortGetFreeHeapSize()));

            successful = (sys.state != State::ConfigAlarm);

            if (!successful) {
                log_info("UI configuration is invalid");
            }

        } catch (const Configuration::ParseException& ex) {
            sys.state = State::ConfigAlarm;
            log_error("UI configuration parse error on line " << ex.LineNumber() << ": " << ex.What());
        } catch (const AssertionFailed& ex) {
            sys.state = State::ConfigAlarm;
            // Get rid of buffer and return
            log_error("UI configuration loading failed: " << ex.what());
        } catch (std::exception& ex) {
            sys.state = State::ConfigAlarm;
            // Log exception:
            log_error("UI configuration validation error: " << ex.what());
        } catch (...) {
            sys.state = State::ConfigAlarm;
            // Get rid of buffer and return
            log_error("Unknown error while processing ui config file");
        }
        delete[] input;

        std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);

        return successful;
    }

    UIConfig::~UIConfig() {
        if ( _lcdDriver ) delete _lcdDriver;
//        delete _axes;
//        delete _i2so;
//        delete _coolant;
//        delete _probe;
//        delete _sdCard;
//        delete _spi;
//        delete _control;
//        delete _macros;
    }
}
