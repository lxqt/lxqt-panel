#ifndef _KBDLAYOUT_H_
#define _KBDLAYOUT_H_

#ifdef X11_ENABLED
#include "x11/kbdlayout.h"
typedef X11Kbd KbdLayout;
#endif

#endif
