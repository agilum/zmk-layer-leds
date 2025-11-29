#include <zephyr/devicetree.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

// Define Node IDs using the literal alias name from your DTS
// lower-layer-led and raise-layer-led are the exact names from your DTS aliases block.
#define LOWER_LED_NODE_ID DT_ALIAS_GET(lower-layer-led)
#define RAISE_LED_NODE_ID DT_ALIAS_GET(raise-layer-led)

// 2. Safely check if the nodes exist before trying to use them
// DT_NODE_HAS_STATUS checks the status of the node ID
#if !DT_NODE_HAS_STATUS(LOWER_LED_NODE_ID, okay) || !DT_NODE_HAS_STATUS(RAISE_LED_NODE_ID, okay)
#error "One or both layer-led aliases (lower-layer-led or raise-layer-led) is not defined or disabled in Devicetree."
#endif

// 3. Get device pointers once the nodes are verified
// DEVICE_DT_GET uses the node ID to get the device pointer
static const struct device *lower_led_dev = DEVICE_DT_GET(LOWER_LED_NODE_ID);
static const struct device *raise_led_dev = DEVICE_DT_GET(RAISE_LED_NODE_ID);


static int boot_leds_init(void) {
    if (lower_led_dev == NULL || !device_is_ready(lower_led_dev)) {
        LOG_WRN("Lower LED device not ready or disabled.");
    } else {
        // Corrected: Added LED index '0'
        led_set_brightness(lower_led_dev, 0, 50); 
    }

    if (raise_led_dev == NULL || !device_is_ready(raise_led_dev)) {
        LOG_WRN("Raise LED device not ready or disabled.");
    } else {
        // Corrected: Added LED index '0'
        led_set_brightness(raise_led_dev, 0, 50);
    }

    return 0;
}

SYS_INIT(boot_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
