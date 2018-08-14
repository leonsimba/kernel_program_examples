CPPFLAGS := -D_GNU_SOURCE
CFLAGS := -g -Wall -W -Wextra -O2
LDFLAGS := $(CFLAGS)

all: fanotify

fanotify: fanotify.o
		${CC} ${CFLAGS} ${LDFLAGS} -o $@ fanotify.o

clean:
	rm -f fanotify fanotify.o *.orig *.rej
