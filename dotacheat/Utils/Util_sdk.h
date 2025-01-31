#pragma once

#include "pch.h"

#include <cstdint>
#include <vector>
#include <string>
#include "../source2_engine/Convar.h"
#include "../source2_engine/IInputSystem.h"

enum GFX_API
{
    UNKNOWN = -1,
    OPENGL = 0,
    VULKAN = 1,
    DIRECTX = 2,
};

namespace Util
{
    inline std::vector<ConVar*> createdConvars;

	// List of particles that should hid or tracked for performance reasons.
	// Linear will be faster than a hashmap until about ~400 entries.
	inline std::vector<std::string> blacklistedParticles;
	inline std::vector<std::string> trackedParticles;

	ButtonCode_t GetButtonCode(const char* buttonName);
	ConVar *RegisterConVar( const char *name, const char *defaultValue, uint32_t flags = FCVAR_CLIENTDLL | FCVAR_ARCHIVE, const char *helpString = nullptr, bool bMin = false, float fMin = 0.0f, bool bMax = false, float fMax = false );

	void SpewDataMap( Datamap *dMap, bool toLogFile = false );
	//void SpewScriptScopes( GameScriptScopesWrapper *scopes, bool toLogFile = false );
	//void* GetScriptScopeFunction( GameScriptScopesWrapper *scopes, const char *exactName );

	int FindDataMapElementOffset( Datamap *dMap, const char *element );

	bool ReadParticleFiles( const char *pathID, const char *blacklistFileName = nullptr, const char *tracklistFileName = nullptr );

	GFX_API GetGraphicsApiType(void);
}