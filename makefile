
all:
	mkdir -p bin
	$(MAKE) -C src/
	$(MAKE) -C test/

clean:
	$(MAKE) -C src/ clean
	$(MAKE) -C test/ clean
	rmdir bin
