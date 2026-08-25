#pragma once

#ifdef HT_ENABLE_LOG
#include <iostream>
#define HT_LOG(expr) do { std::cerr << expr << '\n'; } while (false)
#else
#define HT_LOG(expr) do { } while (false)
#endif

