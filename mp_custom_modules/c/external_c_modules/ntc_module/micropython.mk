NTC_MODULE_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
ifeq($(MICROPY_BOARD) "ESP32_GENERIC_S3")
	SRC_USERMOD += $(NTC_MODULE_MOD_DIR)/adc_ESP32S3.c
	SRC_USERMOD += $(NTC_MODULE_MOD_DIR)/main.c
else
	SRC_USERMOD += $(NTC_MODULE_MOD_DIR)/adc_ESP32.c
	SRC_USERMOD += $(NTC_MODULE_MOD_DIR)/main.c
endif

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(NTC_MODULE_MOD_DIR)
