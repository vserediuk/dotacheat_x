#pragma once
#include "source2_engine/CSource2Client.h"
#include "unordered_map"
#include <string>

namespace Netvars
{
    void DumpNetvars( CSource2Client *clientInterface, const char *fileName );
    void CacheNetvars( CSource2Client *client );

    inline std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>> netvars;
}