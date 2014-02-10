In order to use this library one must define in chconf.h:

#define EXT_DRIVER_EXT_FIELDS void *user_ptr[EXT_MAX_CHANNELS];
#define SPI_DRIVER_EXT_FIELDS void *user_ptr;

This is needed for radio part.
