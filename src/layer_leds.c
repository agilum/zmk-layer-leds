#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h>

// --- ZMK HEADERS ---
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/backlight.h>

LOG_MODULE_REGISTER(layer_leds, CONFIG_ZMK_LOG_LEVEL);

// --- CONFIGURATION ---
// Define the indices of your layers (0-based)
#define LOWER_LAYER_ID 1
#define RAISE_LAYER_ID 2

// Define the desired brightness (0-100) when Layer  is ON
#define DEFAUL_LED_BRIGHTNESS 10

// Define the LED channel indices (0 and 1) on the controller
#define LOWER_LED_INDEX 0
#define RAISE_LED_INDEX 1

// Get the PARENT device (the controller)
// Assuming your DTS has: layer_leds: layer_leds { ... };
static const struct device *layer_leds_dev = DEVICE_DT_GET(DT_NODELABEL(layer_leds));

// --- HELPER FUNCTION ---
static void update_leds(void) {
    if (!device_is_ready(layer_leds_dev)) {
        return;
    }

    // Check layer state
    bool lower_active = zmk_keymap_layer_active(LOWER_LAYER_ID);
    bool raise_active = zmk_keymap_layer_active(RAISE_LAYER_ID);

    // start of adaptive brightness
    uint8_t brightness_percent;

#if IS_ENABLED(CONFIG_ZMK_BACKLIGHT)
        // 1. Get the current persistent backlight level (0-100% or levels)
        //int current_level = zmk_backlight_get_level();
        
        // 2. Convert level to percentage (0-100)
        // FIX: Corrected typo from zmk_backlight_calc_brighness_val to zmk_backlight_calc_brightness_val
        brightness_percent = zmk_backlight_get_brt();
        LOG_INF("Stored backlight brightness: %d%%", brightness_percent);
        // 3. Logic: If backlight is ON, make Caps Lock slightly brighter (+20%)
        // If backlight is OFF (0%), keep Caps Lock somewhat visible (e.g. 20%) or off?
        if (brightness_percent == 0) {
             brightness_percent = 10; // Standalone brightness if backlight is off
        } else {
             brightness_percent += 40; // Boost above ambient backlight
        }
#else
        // Fallback if CONFIG_ZMK_BACKLIGHT is not enabled in Kconfig
        brightness_percent = DEFAULT_LED_BRIGHTNESS;
#endif
        
        // 4. Clamp to maximum 100% to avoid driver errors
        if (brightness_percent > 100) {
            brightness_percent = 100;
        }

        // 5. Final calculation: 0 if inactive, calculated percent if active
        //int final_brightness = laer_lock_active ? brightness_percent : 0;
    

    // Set brightness (0 = OFF, 50 = ON at 50% brightness)
    // Note: The second argument is the CHILD INDEX (0 or 1)
    led_set_brightness(layer_leds_dev, LOWER_LED_INDEX, lower_active ? brightness_percent : 0);
    led_set_brightness(layer_leds_dev, RAISE_LED_INDEX, raise_active ? brightness_percent : 0);
    
    LOG_DBG("Lower=%d, Raise=%d", lower_active, raise_active);
}

// --- ZMK EVENT LISTENER ---
static int layer_event_handler(const zmk_event_t *eh) {
    // We only care about layer state changes
    if (as_zmk_layer_state_changed(eh)) {
        update_leds();
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(layer_leds, layer_event_handler);
ZMK_SUBSCRIPTION(layer_leds, zmk_layer_state_changed);

// --- INITIALIZATION ---
static int layer_leds_init(void) {
    if (!device_is_ready(layer_leds_dev)) {
        LOG_ERR("Layer LEDs device '%s' not ready", layer_leds_dev->name);
        return -ENODEV;
    }

    // --- Added Debug Message on Device Ready ---
    LOG_INF("Layer LEDs device ready and initialized.");
    
    // --- Removed Boot Blink Code ---
    // The previous k_sleep(K_MSEC(1000)) block is now removed.
    
    // Initial sync: This sets the LEDs to the current active layer state (likely both off)
    update_leds();

    return 0;
}

// Use APPLICATION priority to ensure ZMK keymap services are ready
SYS_INIT(layer_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
