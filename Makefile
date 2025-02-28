MODEL = atmega8
F_CPU=16000000UL

.PHONY: lcd_module

lcd_module:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

countdown: lcd_module
	$(MAKE) -C $@
	$(MAKE) -C $@ install

encoder: lcd_module
	$(MAKE) -C $@
	$(MAKE) -C $@ install

