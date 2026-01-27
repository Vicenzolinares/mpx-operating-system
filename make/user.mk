.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h

user/urtc.o: user/urtc.c include/urtc.h include/mpx/krtc.h \
  include/sys_req.h include/string.h

USER_OBJECTS=\
	user/core.o\
  user/urtc.o
