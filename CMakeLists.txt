if(CONFIG_LAYER_LEDS)
    #removed for testing 
    zephyr_include_directories(${ZEPHYR_BASE}/../zmk/app/include)
    zephyr_include_directories(${PROJECT_BINARY_DIR}/zephyr/include/generated)
    target_sources(app PRIVATE src/layer_leds.c)
    #target_sources(app PRIVATE src/layer_leds.c)
    #target_include_directories(app PRIVATE include)    
endif()

    
