##
## 
## 

## misc
INCDIR += $(ITHACA)/include/misc/
CSRC += $(ITHACA)/src/misc/chsprintf.c

## font definitions
INCDIR += $(ITHACA)/include/font/
CPPSRC += $(ITHACA)/src/font/font.cpp \
          $(ITHACA)/src/font/font_std.cpp

## lcd drivers
INCDIR += $(ITHACA)/include/lcd/
CPPSRC += $(ITHACA)/src/lcd/lcd.cpp # \
          $(ITHACA)/src/lcd/lcd_st7735r.cpp # \
          $(ITHACA)/src/lcd/lcd_fsmc.cpp

