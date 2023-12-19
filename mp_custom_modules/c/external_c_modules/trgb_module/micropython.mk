TRGB_MODULE_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(TRGB_MODULE_MOD_DIR)/main.c
SRC_USERMOD += $(TRGB_MODULE_MOD_DIR)/XL9535_driver.c
SRC_USERMOD += $(TRGB_MODULE_MOD_DIR)/ft3267.c
SRC_USERMOD += $(TRGB_MODULE_MOD_DIR)/ui/src/*.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(TRGB_MODULE_MOD_DIR)
CFLAGS_USERMOD += -I$(TRGB_MODULE_MOD_DIR)/ui/src