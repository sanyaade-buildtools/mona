ifndef $(MONADIR)
# export MONADIR=$(PWD)/../mona
export MONADIR=$(shell cd $(PWD)/../../../../mona; pwd)
endif

TARGET  = ACLI
SOURCES = main.cpp

INSTDIR = $(BINDIR)/APPS/
INSTFILES =
CLEANFILES =
$(TARGET).EX5: $(INSTFILES)

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
ADDLINK    = -lmonalibc-imp --enable-auto-import
ADDLINKDEP = 
include $(SHAREDIR)/configs/monapi-ex5.inc
else
ADDLINK    = -lmonalibc-imp -lmonapi-imp 
ADDLINKDEP = 
include $(SHAREDIR)/configs/monapi-el5.inc
endif

ADDLINK += -laudioapi-imp

# overwrite install
install: all
	mkdir -p $(INSTDIR)
	$(INSTALL) -p $(TARGET).EX5 $(INSTFILES) $(INSTDIR)


CXXFLAGS += -fpermissive -DDEBUG
INCLUDE += -I .

-include dependencies
