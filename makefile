# globals
SRC=bin/*.o
OUT=main.out

# mac defines
MAC_CC=clang++
FRAMEWORK=-F../Frameworks -framework SDL2 -framework SDL2_image
MAC_RPATH=-rpath @executable_path/../Frameworks
MAC_LIBS=-L../Frameworks -ltinyxml
MAC_ARGS=$(FRAMEWORK) $(MAC_LIBS) $(MAC_RPATH)


default:
	@echo "no target specified. valid targets:"
	@echo "    mac"
	@exit 1

mac:
	$(MAC_CC) $(MAC_ARGS) -o $(OUT) $(SRC)
