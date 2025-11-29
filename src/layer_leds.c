#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>
#include <zephyr/devicetree.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

// Layer indices (0-based)
#define LOWER_LAYER 1
#define RAISE_LAYER 2

// 1. Define nodes using DT_ALIAS for stability.
#define LOWER_LED_NODE DT_ALIAS(lower_layer_led)
#define RAISE_LED_NODE DT_ALIAS(raise_layer_led)


static const struct device *lower_led_dev;
static const struct device *raise_led_dev;

static void set_lower_led(bool active) {
    if (lower_led_dev == NULL) { return; }
    LOG_DBG("Setting Lower LED to %s", active ? "ON" : "OFF");
    if (active) {
        // Index 0, full on
        led_set_brightness(lower_led_dev, 0, 100);
    } else {
        // Index 0, off
        led_set_brightness(lower_led_dev, 0, 0);
    }
}

static void set_raise_led(bool active) {
    if (raise_led_dev == NULL) { return; }
    LOG_DBG("Setting Raise LED to %s", active ? "ON" : "OFF");
    if (active) {
        // Index 0, full on
        led_set_brightness(raise_led_dev, 0, 100);
    } else {
        // Index 0, off
        led_set_brightness(raise_led_dev, 0, 0);
    }
}

static int handle_layer_state_changed(const zmk_event_t *eh) {
    LOG_DBG("Layer handler triggered");

    // CRITICAL FIX: Use the 'as_' macro to correctly cast the event type
    struct zmk_layer_state_changed *ev = as_zmk_layer_state_changed(eh);
    
    // Check if the event is the one we care about
    if (ev == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    bool lower_active = zmk_keymap_layer_active(LOWER_LAYER);
    bool raise_active = zmk_keymap_layer_active(RAISE_LAYER);
    int highest = zmk_keymap_highest_layer_active();

    LOG_DBG("Layer event: highest_active=%d, lower_active=%d, raise_active=%d", highest, lower_active, raise_active);

    set_lower_led(lower_active);
    set_raise_led(raise_active);
    
    return ZMK_EV_EVENT_BUBBLE;
}

static int layer_leds_init(void) {
    LOG_DBG("Layer LEDs module init started");

    // Check if nodes are defined AND enabled in the devicetree
    if (!DT_NODE_HAS_STATUS(LOWER_LED_NODE, okay)) {
        LOG_ERR("Lower LED node not found/enabled via alias 'lower_layer_led'. Skipping init.");
        lower_led_dev = NULL;
    } else {
        lower_led_dev = DEVICE_DT_GET(LOWER_LED_NODE);
        if (!device_is_ready(lower_led_dev)) {
            LOG_ERR("Lower LED device not ready");
            lower_led_dev = NULL;
            return -ENODEV;
        }
        LOG_DBG("Lower LED ready: %p", lower_led_dev);
    }

    if (!DT_NODE_HAS_STATUS(RAISE_LED_NODE, okay)) {
        LOG_ERR("Raise LED node not found/enabled via alias 'raise_layer_led'. Skipping init.");
        raise_led_dev = NULL;
    } else {
        raise_led_dev = DEVICE_DT_GET(RAISE_LED_NODE);
        if (!device_is_ready(raise_led_dev)) {
            LOG_ERR("Raise LED device not ready");
            raise_led_dev = NULL;
            return -ENODEV;
        }
        LOG_DBG("Raise LED ready: %p", raise_led_dev);
    }
    
    // Safety check to exit cleanly if no LEDs are configured.
    if (lower_led_dev == NULL && raise_led_dev == NULL) {
        LOG_WRN("No LEDs configured for layer_leds module. Exiting init.");
        return 0;
    }

    // Test LEDs on boot (lights them for 5s)
    set_lower_led(true);
    set_raise_led(true);
    // Use k_msleep instead of k_sleep for clarity
    k_msleep(500); 
    set_lower_led(false);
    set_raise_led(false);

    LOG_DBG("Layer LEDs init complete");

    return 0;
}

ZMK_LISTENER(layer_leds_listener, handle_layer_state_changed);
ZMK_SUBSCRIPTION(layer_leds_listener, zmk_layer_state_changed);

SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
