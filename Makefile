SUBDIRS=src

default: all
all: build

build: 
	@for i in $(SUBDIRS); do \
	echo "build ==> $$i"; \
	cd $$i; \
	${MAKE} build; cd ..; \
	done

clean:
	@for i in $(SUBDIRS); do \
	echo "clean ==> $$i"; \
	cd $$i; \
	${MAKE} clean; cd ..; \
	done

depend:
	@for i in $(SUBDIRS); do \
	echo "depend ==> $$i"; \
	cd $$i; \
	${MAKE} depend; cd ..; \
	done
