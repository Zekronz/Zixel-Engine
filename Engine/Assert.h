/*
    Assert.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include "Log.h"

#ifndef NDEBUG
#define ASSERT(expr) if(!(expr)){ ZIXEL_CRITICAL("Assertion failed: {}", #expr); exit(1); }
#else
#define ASSERT(expr)
#endif