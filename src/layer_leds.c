#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h> // Corrected Include path as suggested by user

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

#define BRIGHTNESS_LEVEL 10  // Adjust this value from 0 (off) to 100 (full brightness)

// Get the device pointer using the Devicetree Node Label
// This relies on the successful DTS structure you created: layer_leds: layer_leds { ... };
#if DT_NODE_HAS_STATUS(DT_NODELABEL(layer_leds), okay)
static const struct device *layer_leds_dev = DEVICE_DT_GET(DT_NODELABEL(layer_leds));

// Define the two LED indices based on the child nodes in DTS
#define LOWER_LED_INDEX 0
#define RAISE_LED_INDEX 1

#else
// If the DTS node is missing or disabled, define a dummy device pointer
static const struct device *layer_leds_dev = NULL;
#define LOWER_LED_INDEX -1
#define RAISE_LED_INDEX -1
#warning "Layer LEDs device not found or not enabled in Devicetree."
#endif


static int layer_leds_init(void)
{
    if (!device_is_ready(layer_leds_dev)) {
        LOG_ERR("LED CONTROLLER FAILURE: Device '%s' not ready.", 
                layer_leds_dev ? layer_leds_dev->name : "NULL");
        return -ENODEV;
    }

    LOG_INF("LED CONTROLLER SUCCESS: Device '%s' found and ready.", layer_leds_dev->name);
    LOG_INF("Ready to use LOWER LED (Index %d) and RAISE LED (Index %d) at brightness %d%%.", 
            LOWER_LED_INDEX, RAISE_LED_INDEX, BRIGHTNESS_LEVEL);

    // 1. Turn on the LOWER LED (Index 0) at specified brightness
    int ret_lower = led_set_brightness(layer_leds_dev, LOWER_LED_INDEX, BRIGHTNESS_LEVEL);
    if (ret_lower != 0) {
        LOG_ERR("Failed to set LOWER LED brightness (err %d)", ret_lower);
    } else {
        LOG_INF("LOWER LED is now at %d%% brightness.", BRIGHTNESS_LEVEL);
    }

    // 2. Turn on the RAISE LED (Index 1) at specified brightness
    int ret_raise = led_set_brightness(layer_leds_dev, RAISE_LED_INDEX, BRIGHTNESS_LEVEL);
    if (ret_raise != 0) {
        LOG_ERR("Failed to set RAISE LED brightness (err %d)", ret_raise);
    } else {
        LOG_INF("RAISE LED is now at %d%% brightness.", BRIGHTNESS_LEVEL);
    }

    return 0;
}

// Ensure the module runs after devices are initialized, which we confirmed works
SYS_INIT(layer_leds_init, POST_KERNEL, 90);
