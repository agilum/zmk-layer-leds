#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>
#include <devicetree_generated.h>  // Explicit for DT symbols

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

static const struct device *lower_led_dev;
static const struct device *raise_led_dev;

static int layer_leds_init(void) {
    lower_led_dev = DEVICE_DT_GET(DT_NODELABEL(lower_led));
    if (!device_is_ready(lower_led_dev)) {
        LOG_ERR("Lower LED device not ready");
        return -ENODEV;
    }

    raise_led_dev = DEVICE_DT_GET(DT_NODELABEL(raise_led));
    if (!device_is_ready(raise_led_dev)) {
        LOG_ERR("Raise LED device not ready");
        return -ENODEV;
    }

    // Light LEDs at 50% brightness after boot (adjust 50 to 0-100 as needed)
    led_set_brightness(lower_led_dev, 0, 50);
    led_set_brightness(raise_led_dev, 0, 50);

    return 0;
}

SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
