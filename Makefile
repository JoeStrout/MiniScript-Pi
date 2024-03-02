#
# Makefile
#

include circle-stdlib/Config.mk

CIRCLEHOME = circle-stdlib/libs/circle
NEWLIBDIR = circle-stdlib/install/$(NEWLIB_ARCH)

OBJS	= src/main.o src/kernel.o src/MSFontRenderer.o \
	src/fontData_normal.o \
	src/TextDisplay.o \
	src/ScreenManager.o \
	src/Console.o \
	src/Shell.o \
	MiniScript/Dictionary.o \
	MiniScript/List.o \
	MiniScript/MiniscriptInterpreter.o \
	MiniScript/MiniscriptIntrinsics.o \
	MiniScript/MiniscriptKeywords.o \
	MiniScript/MiniscriptLexer.o \
	MiniScript/MiniscriptParser.o \
	MiniScript/MiniscriptTAC.o \
	MiniScript/MiniscriptTypes.o \
	MiniScript/QA.o \
	MiniScript/SimpleString.o \
	MiniScript/SimpleVector.o \
	MiniScript/SplitJoin.o \
	MiniScript/UnicodeUtil.o \
	MiniScript/UnitTest.o
	

include $(CIRCLEHOME)/Rules.mk

CFLAGS += -Wno-sign-compare
CFLAGS += -I "$(NEWLIBDIR)/include" -I $(STDDEF_INCPATH) -I circle-stdlib/include -I MiniScript
LIBS := "$(NEWLIBDIR)/lib/libm.a" "$(NEWLIBDIR)/lib/libc.a" \
	"$(NEWLIBDIR)/lib/libcirclenewlib.a" \
 	$(CIRCLEHOME)/addon/SDCard/libsdcard.a \
  	$(CIRCLEHOME)/lib/usb/libusb.a \
 	$(CIRCLEHOME)/lib/input/libinput.a \
 	$(CIRCLEHOME)/addon/fatfs/libfatfs.a \
 	$(CIRCLEHOME)/lib/fs/libfs.a \
  	$(CIRCLEHOME)/lib/net/libnet.a \
  	$(CIRCLEHOME)/lib/sched/libsched.a \
  	$(CIRCLEHOME)/lib/libcircle.a

-include $(DEPS)

install:
	@echo "Copying kernel7l.img to /Volumes/BOOT/"
	@cp -f kernel7l.img /Volumes/BOOT/
	@echo "Unmounting /Volumes/BOOT/"
	@diskutil unmount /Volumes/BOOT/

	