.PHONY: app bl clean

all: bl app ota

bl:
	@$(MAKE) -C bootloader/

app:
	@$(MAKE) -C application/

ota:
	@$(MAKE) -C application/ full ota

clean:
	@rm -rf _build/