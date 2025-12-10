#pragma once

#ifdef FT_DEBUG
#define FT_ASSERT(x, msg) assert((msg, x));
#else
#define FT_ASSERT(x, msg) 
#endif