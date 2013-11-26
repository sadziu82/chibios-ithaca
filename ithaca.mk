##
## 
## 

## misc
INCDIR += $(ITHACA)/include/
CSRC += $(ITHACA)/src/misc/device_id.c \
        $(ITHACA)/src/misc/button.c \
        $(ITHACA)/src/misc/digital_output.c \
        $(ITHACA)/src/misc/hcsr501.c \
        $(ITHACA)/src/misc/keypad.c \
        $(ITHACA)/src/misc/mono_timer.c \
        $(ITHACA)/src/misc/pca9633.c \
        $(ITHACA)/src/misc/console.c \
        $(ITHACA)/src/misc/rung.c \
        $(ITHACA)/src/misc/ladder.c \
        $(ITHACA)/src/misc/chsprintf.c \
        $(ITHACA)/src/misc/font.c \
        $(ITHACA)/src/misc/font_std.c \
        $(ITHACA)/src/misc/lcd_st7735.c \
        $(ITHACA)/src/misc/imu.c \
        $(ITHACA)/src/radio/packet.c \
        $(ITHACA)/src/radio/rfm12b.c \
        $(ITHACA)/src/radio/rc.c \
        $(ITHACA)/src/ithaca.c

### ## misc
### INCDIR += $(ITHACA)/include/
### CSRC += $(ITHACA)/src/misc/chsprintf.c
### CPPSRC += $(ITHACA)/src/misc/console.cpp
### 
### ## radio support
### ifeq ($(ITHACA_RADIO),yes)
### INCDIR += $(ITHACA)/include/radio/
### CPPSRC += $(ITHACA)/src/radio/radio.cpp
### ## rfm12b support
### ifeq ($(ITHACA_RADIO_RFM12B),yes)
### CPPSRC += $(ITHACA)/src/radio/radio_rfm12b.cpp
### endif
### ## rfm22b support
### ifeq ($(ITHACA_RADIO_RFM22B),yes)
### CPPSRC += $(ITHACA)/src/radio/radio_rfm22b.cpp
### endif
### endif
### 
### ifeq ($(ITHACA_FONT),yes)
### ## font definitions
### INCDIR += $(ITHACA)/include/font/
### CPPSRC += $(ITHACA)/src/font/font.cpp \
###           $(ITHACA)/src/font/font_std.cpp
### endif
### 
### ifeq ($(ITHACA_LCD),yes)
### ## font definitions
### ## lcd drivers
### INCDIR += $(ITHACA)/include/lcd/
### CPPSRC += $(ITHACA)/src/lcd/lcd.cpp # \
###           $(ITHACA)/src/lcd/lcd_st7735r.cpp # \
###           $(ITHACA)/src/lcd/lcd_fsmc.cpp
### endif

