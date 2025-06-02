#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "driver/timer.h"

volatile uint8_t dir = 1;

#define BUTTON_VOLTAR GPIO_NUM_14
#define BUTTON_IR GPIO_NUM_27

static const gpio_num_t leds[] = {
    GPIO_NUM_19,
    GPIO_NUM_18, 
    GPIO_NUM_5, 
    GPIO_NUM_17  
};

static void IRAM_ATTR gpio0_isr_handler(void* arg) {
    uint8_t *v = (uint8_t*) arg;
    *v = 0;
}

static void IRAM_ATTR gpio1_isr_handler(void* arg) {
    uint8_t *v = (uint8_t*) arg;
    *v = 1; 
}
void setup() {
  gpio_config_t io_conf_leds = {
      .pin_bit_mask = (1ULL << leds[0]) | (1ULL << leds[1]) | (1ULL << leds[2]) | (1ULL << leds[3]),
      .mode = GPIO_MODE_OUTPUT,
      .pull_down_en = 0,
      .pull_up_en = 0,
      .intr_type = GPIO_INTR_DISABLE
  };
  gpio_config(&io_conf_leds);
  for (int i = 0; i < 4; i++) {
        io_conf_leds.pin_bit_mask |= (1ULL << leds[i]);
    }

  gpio_config_t io_conf_button = {
      .pin_bit_mask = (1ULL << BUTTON_VOLTAR) | (1ULL << BUTTON_IR),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = 0, 
      .pull_down_en = 0,
      .intr_type = GPIO_INTR_POSEDGE
  };
  gpio_install_isr_service(0);
  gpio_isr_handler_add(BUTTON_VOLTAR, gpio0_isr_handler, (void*)&dir);
  gpio_isr_handler_add(BUTTON_IR, gpio1_isr_handler, (void*)&dir);

  gpio_config(&io_conf_button);
  
}

void update_leds(int led_index) {
    for (int i = 0; i < 4; i++) {
        gpio_set_level(leds[i], i == led_index ? 1 : 0);
    }
}

void app_main() {
  uint8_t v = 4;
  //novo:
  setup();

  while (true) {
    
    v += dir ? 1 : -1;
    if (v > 9) v = 1;
    if (v < 1) v = 9;

    int led_to_light;
    switch (v) {
      case 1: case 6: led_to_light = 0; break;
      case 2: case 7: led_to_light = 1; break;
      case 3: case 8: led_to_light = 2; break;
      case 4: case 9: led_to_light = 3; break;
      default: led_to_light = -1; break;
    }
    if (led_to_light >= 0) {
      update_leds(led_to_light);
    }

    printf("%d\n", v);

    vTaskDelay(pdMS_TO_TICKS(500));
  }

}
