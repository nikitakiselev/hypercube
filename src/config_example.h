#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

//#define WEB_SERVER_ENABLE

#define EDGE_LEDS 25    // кол-во диодов на ребре куба

/**
 * LED DATA PIN.
 * Dont use GPIO_NUM_2 for Automatic bootloader mode correct work.
 */
#define LED_DI GPIO_NUM_4
#define CHANGE_PRD 10   // смена режима, секунд
#define CUR_LIMIT 1500  // лимит тока в мА (0 - выкл)


#define ADC_PIN GPIO_NUM_34 // Audio input pin
#define VOL_THR 30 // noise amount

#define BTN_PIN GPIO_NUM_33 // Action Button pin
#define TRANSISTOR_PIN GPIO_NUM_32 // transistor pin

#define MINUTES_TO_MILLIS(MINUTES) (MINUTES * 60000)

#define IRC_PIN GPIO_NUM_16