#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>
#include <devicetree_generated.h>  // Correct include for generated DT symbols
#include <zephyr/devicetree.h>  // For DT_PATH

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);
//*****this is removed for testing 12.01.
//static const struct device *lower_led_dev;
//static const struct device *raise_led_dev;

// added for testing 12.01
// This macro automatically finds the PARENT device and the CHILD index.
static const struct led_dt_spec lower_led = LED_DT_SPEC_GET(DT_NODELABEL(lower_led));
static const struct led_dt_spec raise_led = LED_DT_SPEC_GET(DT_NODELABEL(raise_led));
//**** end testing 12/01
static int layer_leds_init(void) {
    /*lower_led_dev = DEVICE_DT_GET(DT_PATH(layer_leds, lower_led));
    if (!device_is_ready(lower_led_dev)) {
        LOG_ERR("Lower LED device not ready");
        return -ENODEV;
    }

    raise_led_dev = DEVICE_DT_GET(DT_PATH(layer_leds, raise_led));
    if (!device_is_ready(raise_led_dev)) {
        LOG_ERR("Raise LED device not ready");
        return -ENODEV;
    }
// ***** added for testin 12.01
    // Light LEDs at 50% brightness after boot (adjust 50 to 0-100 as needed)
    led_set_brightness(lower_led_dev, 0, 50);
    led_set_brightness(raise_led_dev, 0, 50);*/

    // 2. Check if the PARENT device is ready
    if (!device_is_ready(lower_led.dev)) {
        LOG_ERR("Lower LED parent device not ready");
        return -ENODEV;
    }
    if (!device_is_ready(raise_led.dev)) {
        LOG_ERR("Raise LED parent device not ready");
        return -ENODEV;
    }

    // 3. Turn them on using the _dt API
    // This helper function uses the spec to call the driver correctly.
    // led_set_brightness_dt(spec, brightness_0_to_100)
    
    int ret;
    ret = led_set_brightness_dt(&lower_led, 50);
    if (ret < 0) LOG_ERR("Failed to set lower led");

    ret = led_set_brightness_dt(&raise_led, 50);
    if (ret < 0) LOG_ERR("Failed to set raise led");
//****** end of testing 12.01
    return 0;
}



SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
