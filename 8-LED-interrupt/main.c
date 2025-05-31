//Bibiotecas------------------------
#include <stdio.h>

#include "freertos/FreeRTOS.h"

#include "freertos/task.h"

#include "driver/gpio.h"

//novo:
#include "driver/timer.h"
 //----------------------------------

//Definindo pinos-------------------
#define BUTTON_VOLTAR GPIO_NUM_14
#define BUTTON_IR GPIO_NUM_27

#define LED1 GPIO_NUM_19
#define LED2 GPIO_NUM_18
#define LED3 GPIO_NUM_5
#define LED4 GPIO_NUM_17
#define LED5 GPIO_NUM_16
#define LED6 GPIO_NUM_4
#define LED7 GPIO_NUM_0
#define LED8 GPIO_NUM_2
//----------------------------------

static void IRAM_ATTR gpio0_isr_handler(void * arg) {
  uint8_t * v = (uint8_t * ) arg;
  * v = 0;
}

static void IRAM_ATTR gpio1_isr_handler(void * arg) {
  uint8_t * v = (uint8_t * ) arg;
  * v = 1;
}
//-----------------------------------------------------------------------------------------------------------------------

volatile uint8_t dir = 0;

void setup() {

  gpio_config_t io_conf_leds = {
    .pin_bit_mask = (1ULL << LED1) |
    (1ULL << LED2) |
    (1ULL << LED3) |
    (1ULL << LED4) |
    (1ULL << LED5) |
    (1ULL << LED6) |
    (1ULL << LED7) |
    (1ULL << LED8),
    .mode = GPIO_MODE_OUTPUT,
    .pull_down_en = 0,
    .pull_up_en = 0,
    .intr_type = GPIO_INTR_DISABLE
  };
  gpio_config( & io_conf_leds);

  gpio_config_t io_conf_button = {
    .pin_bit_mask = (1ULL << BUTTON_VOLTAR) | (1ULL << BUTTON_IR),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = 0,
    .pull_down_en = 0,
    .intr_type = GPIO_INTR_NEGEDGE
  };
  gpio_config( & io_conf_button);
}

void app_main() {

  setup();

  uint8_t valor = 1;

  gpio_install_isr_service(0);

  gpio_isr_handler_add(BUTTON_IR, gpio0_isr_handler, (void * ) & dir);
  gpio_isr_handler_add(BUTTON_VOLTAR, gpio1_isr_handler, (void * ) & dir);
  //---------------------------------------------------------------------

  gpio_num_t leds[8] = {
    LED1,
    LED2,
    LED3,
    LED4,
    LED5,
    LED6,
    LED7,
    LED8
  };

  while (true) {
    for (int i = 0; i < 8; i++) {
      gpio_set_level(leds[i], (valor >> i) & 1);
    }

    printf("Valor atual: %d\n", valor);

    if (dir == 0) {

      valor <<= 1;
      if (valor == 0) {
        valor = 1;
      }
    } else {
      valor >>= 1;
      if (valor < 1) {
        valor = 128;
      }
    }
    vTaskDelay(125 / portTICK_PERIOD_MS);
  }
}