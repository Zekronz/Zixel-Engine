/*
    ZixelPCH.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <stack>
#include <cstdint>
#include <stdint.h>
#include <cmath>
#include <string>
#include <vector>
#include <codecvt>
#include <functional>
#include <algorithm>

#include <glad/glad.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <NFDExtended/nfd.h>

#include "Engine/Log.h"
#include "Engine/Types.h"

//@TODO: Platform specific.
#include <Windows.h>
#include <ShObjIdl_core.h>
#include <ShObjIdl.h>
#include <shlwapi.h>
#include <timeapi.h>