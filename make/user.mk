.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h

user/comhand.o: user/comhand.c include/comhand.h include/sys_req.h \
  include/mpx/device.h include/string.h include/urtc.h include/stdlib.h

user/urtc.o: user/urtc.c include/mpx/krtc.h include/sys_req.h \
  include/string.h

user/upcb.o: user/upcb.c include/mpx/kpcb.h

USER_OBJECTS=\
	user/core.o\
  user/urtc.o\
  user/comhand.o\
  user/upcb.o
