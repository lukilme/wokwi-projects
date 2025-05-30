//Bibiotecas------------------------
#include <stdio.h>

#include "freertos/FreeRTOS.h"

#include "freertos/task.h"

#include "driver/gpio.h"
 //----------------------------------

//Definindo pinos-------------------
#define BUTTON_VOLTAR GPIO_NUM_14
#define BUTTON_IR GPIO_NUM_27

static
const gpio_num_t leds[] = {
  GPIO_NUM_19,
  GPIO_NUM_18,
  GPIO_NUM_5,
  GPIO_NUM_17
};
//----------------------------------
void update_leds(int led_index) {
  for (int i = 0; i < 4; i++) {
    gpio_set_level(leds[i], i == led_index ? 1 : 0);
  }
}

void app_main() {
  uint8_t v = 4;
  bool flag = true;
  // Configuração dos LEDs como saída
  gpio_config_t io_conf_leds = {
    .pin_bit_mask = (1 ULL << leds[0]) | (1 ULL << leds[1]) | (1 ULL << leds[2]) | (1 ULL << leds[3]),
    .mode = GPIO_MODE_OUTPUT,
    .pull_down_en = 0,
    .pull_up_en = 0,
    .intr_type = GPIO_INTR_DISABLE
  };
  gpio_config( & io_conf_leds);

  // Configuração do botão como entrada
  gpio_config_t io_conf_button = {
    .pin_bit_mask = (1 ULL << BUTTON_VOLTAR) | (1 ULL << BUTTON_IR),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = 0,
    .pull_down_en = 0,
    .intr_type = GPIO_INTR_DISABLE
  };
  gpio_config( & io_conf_button);

  while (true) {
    // Aqui pode colocar leitura dos botões e lógica para os LEDs depois
    if (gpio_get_level(BUTTON_VOLTAR) == 0) {
      flag = false;
    } else if (gpio_get_level(BUTTON_IR) == 0) {
      flag = true;
    }
    v += flag ? 1 : -1;
    if (v > 9) v = 1;
    if (v < 1) v = 9;

    int led_to_light;
    switch (v) {
    case 1:
    case 6:
      led_to_light = 0;
      break;
    case 2:
    case 7:
      led_to_light = 1;
      break;
    case 3:
    case 8:
      led_to_light = 2;
      break;
    case 4:
    case 9:
      led_to_light = 3;
      break;
    default:
      led_to_light = -1;
      break;
    }
    if (led_to_light >= 0) {
      update_leds(led_to_light);
    }

    printf("%d\n", v);

    vTaskDelay(pdMS_TO_TICKS(500));
  }

}