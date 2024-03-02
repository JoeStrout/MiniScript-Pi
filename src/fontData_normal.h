#ifndef _fontData_normal_h
#define _fontData_normal_h

#include <cstdint>

namespace FontData_Normal {

extern uint32_t* lowCharData[];	// font data, indexed by (code point - 32)

void init();

}

#endif // _fontData_normal_h