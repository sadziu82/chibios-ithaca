##
## 
## 

## misc
INCDIR += $(ITHACA)/include/misc/
CSRC += $(ITHACA)/src/misc/chsprintf.c
CPPSRC += $(ITHACA)/src/misc/console.cpp

ifeq ($(ITHACA_RADIO),yes)
## font definitions
INCDIR += $(ITHACA)/include/radio/
CPPSRC += $(ITHACA)/src/radio/radio.cpp
endif

ifeq ($(ITHACA_FONT),yes)
## font definitions
INCDIR += $(ITHACA)/include/font/
CPPSRC += $(ITHACA)/src/font/font.cpp \
          $(ITHACA)/src/font/font_std.cpp
endif

ifeq ($(ITHACA_LCD),yes)
## font definitions
## lcd drivers
INCDIR += $(ITHACA)/include/lcd/
CPPSRC += $(ITHACA)/src/lcd/lcd.cpp # \
          $(ITHACA)/src/lcd/lcd_st7735r.cpp # \
          $(ITHACA)/src/lcd/lcd_fsmc.cpp
endif

