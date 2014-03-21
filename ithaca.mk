## ithaca library
ITHACAINC += $(ITHACALIB)/include/ \
             $(ITHACALIB)/include/misc/

ITHACASRC += $(ITHACALIB)/src/misc/device_id.c \
             $(ITHACALIB)/src/misc/button.c \
             $(ITHACALIB)/src/misc/digital_output.c \
             $(ITHACALIB)/src/sensors/hcsr501.c \
             $(ITHACALIB)/src/misc/keypad44.c \
             $(ITHACALIB)/src/misc/mono_timer.c \
             $(ITHACALIB)/src/misc/pca9633.c \
             $(ITHACALIB)/src/sensors/lm75ad.c \
             $(ITHACALIB)/src/sensors/dht11.c \
             $(ITHACALIB)/src/misc/console.c \
             $(ITHACALIB)/src/misc/rung.c \
             $(ITHACALIB)/src/misc/ladder.c \
             $(ITHACALIB)/src/misc/chsprintf.c \
             $(ITHACALIB)/src/misc/imu.c \
             $(ITHACALIB)/src/radio.c \
             $(ITHACALIB)/src/radio/ppm.c \
             $(ITHACALIB)/src/radio/rfm12b.c \
             $(ITHACALIB)/src/radio/rfm22b.c \
             $(ITHACALIB)/src/radio/mesh.c \
             $(ITHACALIB)/src/radio/rc.c \
             $(ITHACALIB)/src/ithaca.c

ITHACACPPSRC += $(ITHACALIB)/src/lcd.cpp \
                $(ITHACALIB)/src/lcd/st7735.cpp \
                $(ITHACALIB)/src/font.cpp \
                $(ITHACALIB)/src/font/font_std.cpp \
                $(ITHACALIB)/src/widget.cpp \
                $(ITHACALIB)/src/widget/main_window.cpp \
                $(ITHACALIB)/src/widget/frame.cpp \
                $(ITHACALIB)/src/widget/label.cpp \
                $(ITHACALIB)/src/widget/top.cpp

