//
// jaja, mod loader.
//
// 2009/08/14 Waync Cheng
//

#pragma once

#include <string>

// return -1 if failed to load, otherwise return mod handle
int loadMod(const std::string& modName);
int loadMod(const std::string& modName, size_t szStream, const unsigned char* pStream);

// end of ld.h
