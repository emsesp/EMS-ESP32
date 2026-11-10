/* Copyright 2015, Kenneth MacKay. Licensed under the BSD 2-clause license. */

/* EMS-ESP only verifies signatures, which needs no random numbers. This replaces the upstream file, whose
   default RNG for Windows needs extra system libraries, so no default RNG is defined. */

#ifndef _UECC_PLATFORM_SPECIFIC_H_
#define _UECC_PLATFORM_SPECIFIC_H_

#include "types.h"

#endif /* _UECC_PLATFORM_SPECIFIC_H_ */
