I2C_MODULE_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(I2C_MODULE_MOD_DIR)/i2c_module.c
SRC_USERMOD += $(I2C_MODULE_MOD_DIR)/i2c_master.c
SRC_USERMOD += $(I2C_MODULE_MOD_DIR)/i2c_slave.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(I2C_MODULE_MOD_DIR)
