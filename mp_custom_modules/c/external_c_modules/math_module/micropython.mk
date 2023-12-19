MATH_MODULE := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(MATH_MODULE)/main.c
SRC_USERMOD += $(MATH_MODULE)/simple_math.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(MATH_MODULE)
