#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON GPIO_NUM_34
#define LED1 GPIO_NUM_19
#define LED2 GPIO_NUM_21 //*

// Variável global de estado (contagem)
uint8_t v = 0;

void app_main() {

    // Configuração dos LEDs como saída
    gpio_config_t io_conf_leds = {
        .pin_bit_mask = (1ULL << LED1) | (1ULL << LED2), //*
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 0,
        .pull_up_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_leds);

    // Configuração do botão como entrada
    gpio_config_t io_conf_button = {
        .pin_bit_mask = (1ULL << BUTTON),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0, 
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_button);
  
  while (true) {
    
    int estado_do_botao = gpio_get_level(BUTTON);

    //*
    if (estado_do_botao == 0) {
      // Quando o botão for pressionado, apaga os LEDs e incrementa a variável v
      gpio_set_level(LED1, 0);
      gpio_set_level(LED2, 0);

      v++;
      if (v > 3) {  // máximo com 2 bits: 00, 01, 10, 11
          v = 0;
      }

      vTaskDelay(pdMS_TO_TICKS(500)); 
    } 
    
    else {
      // Atualiza os LEDs com base nos bits de v
      gpio_set_level(LED1, (v >> 0) & 1);
      gpio_set_level(LED2, (v >> 1) & 1);

      vTaskDelay(pdMS_TO_TICKS(500));
    }

    printf("v = %d\n", v);

  }

}
