#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define EDGE_LEDS 25    // кол-во диодов на ребре куба

/**
 * LED DATA PIN.
 * Dont use GPIO_NUM_2 for Automatic bootloader mode correct work.
 */
#define LED_DI GPIO_NUM_4
#define BRIGHT 100      // яркость
#define CHANGE_PRD 10   // смена режима, секунд
#define CUR_LIMIT 1000  // лимит тока в мА (0 - выкл)