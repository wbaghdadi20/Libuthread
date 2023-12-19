# Target library
lib	:= libuthread.a
targets	:= queue uthread context preempt sem
objs	:= queue.o uthread.o context.o preempt.o sem.o
CC	:= gcc

#remove -Werror for now
CFLAGS	:= -Wall -Wextra -Werror -MMD
CFLAGS	+= -g
# queue_tester.o cant be in objs cause otherwise is included in making of library 

## TODO: Phase 1
ifneq ($(V),1)
Q = @
endif

all: $(lib)

# all: $(targets)
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

$(targets): $(objs)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(lib): $(objs)
	@echo "CC $@"
	$(Q) ar rcs  $@ $^

clean:
	@echo "clean"
	$(Q)rm -f $(deps) $(targets) $(objs) $(lib)
