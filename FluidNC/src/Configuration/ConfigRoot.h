// Copyright (c) 2021 -  Stefan de Bruijn
// Copyright (c) 2021 -  Mitch Bradley
// Copyright (c) 2023 -  Vlad A.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "../Assert.h"
#include "GenericFactory.h"
#include "HandlerBase.h"
#include "Configurable.h"

namespace Configuration {

    class ConfigRoot : public Configuration::Configurable {
    public:
        ConfigRoot() = default;

#if 0
        // this is an example of implementation.
        // Please copy in into child class and adapt to your needs.
        static ConfigRoot*& instance() {
            static ConfigRoot* instance = nullptr;

            if ( instance == nullptr )
                instance = new ConfigRoot();

            return instance;
        }
#endif

        void afterParse() = 0;
        void group(Configuration::HandlerBase& handler) = 0;

        virtual bool load() = 0;
        virtual void cleanup() = 0;
        virtual const char* name() = 0;

        virtual bool load(const char* file, const char* rootSectionName);
        virtual bool load(StringRange* input, const char* rootSectionName);
    public:
        ~ConfigRoot() {};
    };
}

