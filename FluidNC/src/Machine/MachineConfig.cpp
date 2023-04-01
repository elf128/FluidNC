// Copyright (c) 2021 -  Stefan de Bruijn
// Copyright (c) 2021 -  Mitch Bradley
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "MachineConfig.h"

#include "../Kinematics/Kinematics.h"

#include "../Motors/MotorDriver.h"
#include "../Motors/NullMotor.h"

#include "../Spindles/NullSpindle.h"
#include "../UartChannel.h"

#include "../SettingsDefinitions.h"  // config_filename
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

Machine::MachineConfig* config = nullptr;

// TODO FIXME: Split this file up into several files, perhaps put it in some folder and namespace Machine?

namespace Machine {
    char _name_[] = "machine";

    void MachineConfig::group(Configuration::HandlerBase& handler) {
        handler.item("board", _board);
        handler.item("name", _name);
        handler.item("meta", _meta);

        handler.section("stepping", _stepping);

        handler.section("uart1", _uarts[1], 1);
        handler.section("uart2", _uarts[2], 2);

        handler.section("uart_channel1", _uart_channels[1]);
        handler.section("uart_channel2", _uart_channels[2]);

        handler.section("i2so", _i2so);

        handler.section("i2c0", _i2c[0], 0);
        handler.section("i2c1", _i2c[1], 1);

        handler.section("spi0", _spi[0], 0 );
        handler.section("spi1", _spi[1], 1 );
        handler.section("sdcard", _sdCard);

        handler.section("kinematics", _kinematics);
        handler.section("axes", _axes);

        handler.section("control", _control);
        handler.section("coolant", _coolant);
        handler.section("probe", _probe);
        handler.section("macros", _macros);
        handler.section("start", _start);
        handler.section("parking", _parking);

        handler.section("user_outputs", _userOutputs);

        handler.section("oled", _oled);
        
        Spindles::SpindleFactory::factory(handler, _spindles);

        // TODO: Consider putting these under a gcode: hierarchy level? Or motion control?
        handler.item("arc_tolerance_mm", _arcTolerance, 0.001, 1.0);
        handler.item("junction_deviation_mm", _junctionDeviation, 0.01, 1.0);
        handler.item("verbose_errors", _verboseErrors);
        handler.item("report_inches", _reportInches);
        handler.item("enable_parking_override_control", _enableParkingOverrideControl);
        handler.item("use_line_numbers", _useLineNumbers);
        handler.item("planner_blocks", _planner_blocks, 10, 120);
    }

    void MachineConfig::afterParse() {
        if (_axes == nullptr) {
            log_info("Axes: using defaults");
            _axes = new Axes();
        }

        if (_coolant == nullptr) {
            _coolant = new CoolantControl();
        }

        if (_kinematics == nullptr) {
            _kinematics = new Kinematics();
        }

        if (_probe == nullptr) {
            _probe = new Probe();
        }

        if (_userOutputs == nullptr) {
            _userOutputs = new UserOutputs();
        }

        if (_sdCard == nullptr) {
            _sdCard = new SDCard();
        }

        if (_spi[0] == nullptr) {
            _spi[0] = new SPIBus();
        }

        if (_stepping == nullptr) {
            _stepping = new Stepping();
        }

        // We do not auto-create an I2SO bus config node
        // Only if an i2so section is present will config->_i2so be non-null

        if (_control == nullptr) {
            _control = new Control();
        }

        if (_start == nullptr) {
            _start = new Start();
        }

        if (_parking == nullptr) {
            _parking = new Parking();
        }

        if (_spindles.size() == 0) {
            _spindles.push_back(new Spindles::Null());
        }

        // Precaution in case the full spindle initialization does not happen
        // due to a configuration error
        spindle = _spindles[0];

        uint32_t next_tool = 100;
        for (auto s : _spindles) {
            if (s->_tool == -1) {
                s->_tool = next_tool++;
            }
        }

        if (_macros == nullptr) {
            _macros = new Macros();
        }
    }

    StringRange defaultConf = StringRange("name: Default (Test Drive)\nboard: None\n");

    bool MachineConfig::load() {
        bool configOkay;
        // If the system crashes we skip the config file and use the default
        // builtin config.  This helps prevent reset loops on bad config files.
        esp_reset_reason_t reason = esp_reset_reason();
        if (reason == ESP_RST_PANIC) {
            log_error("Skipping configuration file due to panic");
            configOkay = false;
        } else {
            configOkay = ConfigRoot::load(config_filename->get(), name() );
        }

        if (!configOkay) {
            log_info("Using default configuration");

            //StringRange* defaultConf = new StringRange(defaultConfig);
            configOkay = ConfigRoot::load(&defaultConf, name() );
            //delete[] defaultConf;
        }

        return configOkay;
    }

    MachineConfig*& MachineConfig::instance() {
        if ( config == nullptr )
            config = new MachineConfig();

        // Currently, many parts of code refer this singleton using global variable 
        // instead of calling MachineConfig::instance directly. 
        // 
        return config;
    }

    void MachineConfig::cleanup() {
        if ( _axes )       { delete _axes;       _axes = nullptr; }
        if ( _kinematics ) { delete _kinematics; _kinematics = nullptr; }
        if ( _i2so )       { delete _i2so;       _i2so = nullptr; }
        if ( _stepping )   { delete _stepping;   _stepping = nullptr; }
        if ( _coolant )    { delete _coolant;    _coolant = nullptr; }
        if ( _probe )      { delete _probe;      _probe = nullptr; }
        if ( _control )    { delete _control;    _control = nullptr; }
        if ( _userOutputs ){ delete _userOutputs;_userOutputs = nullptr; }
        if ( _sdCard )     { delete _sdCard;     _sdCard = nullptr; }
        if ( _macros )     { delete _macros;     _macros = nullptr; }
        if ( _start )      { delete _start;      _start = nullptr; }
        if ( _parking )    { delete _parking;    _parking = nullptr; }
        if ( _oled )       { delete _oled;       _oled = nullptr; }

        for( int i = 0; i < MAX_N_SPI; i++ )
            if ( _spi[i] )     { delete _spi[i]; _spi[i] = nullptr; }

        for( int i = 0; i < MAX_N_I2C; i++ )
            if ( _i2c[i] )     { delete _i2c[i]; _i2c[i] = nullptr; }

        for( int i = 0; i < MAX_N_UARTS; i++ ) {
            if ( _uarts[i] )         { delete _uarts[i];         _uarts[i] = nullptr; }
            if ( _uart_channels[i] ) { delete _uart_channels[i]; _uart_channels[i] = nullptr; }
        }
    }

    const char* MachineConfig::name() {
        return _name_;
    }


    MachineConfig::~MachineConfig() {
        cleanup();
    }
}
