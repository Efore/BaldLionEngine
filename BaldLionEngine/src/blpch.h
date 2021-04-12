#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <sstream>
#include <array>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "BaldLion/Core/Log.h"
#include "BaldLion/Core/Core.h"
#include "optick.h"
#include "BaldLion/Core/Memory/MemoryManager.h"
#include "BaldLion/Core/StringId.h"

#ifdef BL_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

using namespace BaldLion::Memory;

