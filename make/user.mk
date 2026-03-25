.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h

user/comhand.o: user/comhand.c include/comhand.h include/sys_req.h \
  include/mpx/device.h include/string.h include/urtc.h include/stdlib.h \
  include/loadr3.h include/alarm.h include/upcb.h

user/urtc.o: user/urtc.c include/mpx/krtc.h include/sys_req.h \
  include/string.h

user/upcb.o: user/upcb.c include/mpx/kpcb.h 

user/loadr3.o: user/loadr3.c include/upcb.h include/mpx/sys_call.h \
  include/mpx/kpcb.h include/string.h include/processes.h \
  include/mpx/kpcb.h

user/alarm.o: user/alarm.c include/comhand.h include/sys_req.h \
  include/string.h include/mpx/kpcb.h include/upcb.h

user/umem.o: user/umem.c include/lib_mem.h

USER_OBJECTS=\
	user/core.o\
  user/urtc.o\
  user/comhand.o\
  user/upcb.o\
  user/loadr3.o\
  user/alarm.o\
  user/umem.o
