intrpn : intrpn.c
	gcc -o intrpn -Wall -lm -lncurses intrpn.c

install : intrpn
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f intrpn $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/intrpn
