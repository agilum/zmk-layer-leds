# If the config option is enabled (CONFIG_LAYER_LEDS=y in your keyboard.conf), compile the source
if(CONFIG_LAYER_LEDS)
    # Create a Zephyr library for the module
    zephyr_library()
    # Add the source file(s) to the library
    zephyr_library_sources(src/layer_leds.c)
endif()

