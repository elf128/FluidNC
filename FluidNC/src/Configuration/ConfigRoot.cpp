// Copyright (c) 2021 -  Stefan de Bruijn
// Copyright (c) 2021 -  Mitch Bradley
// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include "ConfigRoot.h"

//#include "LcdDriver.h"
//#include "TouchDriver.h"

//#include "../SettingsDefinitions.h"  // ui_config_filename
#include "../FileStream.h"

#include "Parser.h"
#include "ParserHandler.h"
#include "Validator.h"
#include "AfterParse.h"
#include "ParseException.h"
#include "../Config.h"  // ENABLE_*

#include <cstdio>
#include <cstring>
#include <atomic>


namespace Configuration {

    char defaultConfig[] = "\n";

    bool ConfigRoot::load( const char* filename, const char* rootSectionName) {
        char* buffer = nullptr;
        try {
            log_debug("[ " << name() << " ] load( \"" << filename << "\") ");
            FileStream file( filename, "r", "" );

            auto filesize = file.size();
            if (filesize <= 0) {
                log_info("[ " << name() << " ] configuration file:" << filename << " is empty");
                return false;
            }

            char* buffer     = new char[filesize + 1];
            buffer[filesize] = '\0';
            auto actual      = file.read(buffer, filesize);

            if (actual != filesize) {
                log_info("[ " << name() << " ] configuration file:" << filename << " read error");

                delete[] buffer;
                return false;
            }

            log_info("[ " << name() << " ] configuration file:" << filename);
            
            StringRange* strings = new StringRange(buffer, buffer + filesize); 

            bool retval = load(strings, rootSectionName);

            delete[] strings;
            delete[] buffer;
            return retval;

        } catch (...) {
            log_warn("Cannot open " << name() << " configuration file:" << filename);

            if (buffer)
                delete[] buffer;

            return false;
        }
    }

    bool ConfigRoot::load( StringRange* input, const char* rootSectionName ) {
        bool successful = false;
        try {
            Configuration::Parser        parser(input->begin(), input->end());
            Configuration::ParserHandler handler(parser);

            cleanup();

            handler.enterSection(rootSectionName,  this);

            log_debug("[ " << name() << " ] Running after-parse tasks");

            try {
                Configuration::AfterParse afterParseInstance;

                afterParse();
                group(afterParseInstance);

            } catch (std::exception& ex) { log_info("[ "<< name() << " ] validation error: " << ex.what()); }

            log_debug("[ " << name() << " ] checking configuration");

            try {
                Configuration::Validator validator;

                validate();
                group(validator);

            } catch (std::exception& ex) { log_info("[ "<< name() << " ] validation error: " << ex.what()); }

            // log_info("Heap size after configuation load is " << uint32_t(xPortGetFreeHeapSize()));

            successful = (sys.state != State::ConfigAlarm);

            if (!successful) {
                log_info("[ "<< name() << " ] configuration is invalid");
            }

        } catch (const Configuration::ParseException& ex) {
            sys.state = State::ConfigAlarm;
            log_error("[ "<< name() << " ] configuration parse error on line " << ex.LineNumber() << ": " << ex.What());
        } catch (const AssertionFailed& ex) {
            sys.state = State::ConfigAlarm;
            // Get rid of buffer and return
            log_error("[ "<< name() << " ] configuration loading failed: " << ex.what());
        } catch (std::exception& ex) {
            sys.state = State::ConfigAlarm;
            // Log exception:
            log_error("[ "<< name() << " ] configuration validation error: " << ex.what());
        } catch (...) {
            sys.state = State::ConfigAlarm;
            // Get rid of buffer and return
            log_error("Unknown error while processing " << name() << " config file");
        }

        std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);
        log_info("[ "<< name() << " ] configuration is valid");
        return successful;
    }

}
