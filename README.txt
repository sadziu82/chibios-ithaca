In order to use this library one must:

* include following in Makefile of your project (might be just below PROJECT definition):

ITHACA = /path/to/library
include $(ITHACA)/boards/home_node_v1/board.mk

* include at the end of path section:

include $(ITHACA)/ithaca.mk

* if you need radio support, depending on radio type:

** apply patch for EXT driver structure:

diff --git a/os/hal/platforms/STM32/ext_lld.h b/os/hal/platforms/STM32/ext_lld.h
index 7a59fa7..c21741b 100644
--- a/os/hal/platforms/STM32/ext_lld.h
+++ b/os/hal/platforms/STM32/ext_lld.h
@@ -120,6 +120,9 @@ struct EXTDriver {
    */
   const EXTConfig           *config;
   /* End of the mandatory fields.*/
+#if defined(EXT_DRIVER_EXT_FIELDS)^M
+  EXT_DRIVER_EXT_FIELDS^M
+#endif^M
 };
 
 /*===========================================================================*/

** define in ithacaconf.h:

#define EXT_DRIVER_EXT_FIELDS void *user_ptr[EXT_MAX_CHANNELS];
#define SPI_DRIVER_EXT_FIELDS void *user_ptr;

