#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON GPIO_NUM_34
#define LED1   GPIO_NUM_21
#define LED2   GPIO_NUM_19
#define LED3   GPIO_NUM_5
#define LED4   GPIO_NUM_16

// Variável global de estado (contagem)
uint8_t v = 0;

void app_main() {
    // Configuração dos LEDs como saída
    gpio_config_t io_conf_leds = {
        .pin_bit_mask = (1ULL<<LED1)
                      | (1ULL<<LED2)
                      | (1ULL<<LED3)
                      | (1ULL<<LED4),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_down_en = 0,
        .pull_up_en   = 0,
        .intr_type    = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_leds);

    // Configuração do botão como entrada com pull-up interno
    gpio_config_t io_conf_button = {
        .pin_bit_mask = (1ULL<<BUTTON),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = 1,   // botão fecha para GND → lemos 0 quando pressionado
        .pull_down_en = 0,
        .intr_type    = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_button);

    while (true) {
        int estado_do_botao = gpio_get_level(BUTTON);

        if (estado_do_botao == 0) {
            // botão pressionado → LEDs apagados e incrementa v
            gpio_set_level(LED1, 0);
            gpio_set_level(LED2, 0);
            gpio_set_level(LED3, 0);
            gpio_set_level(LED4, 0);

            v++;
            if (v > 15) {   // agora usamos 4 bits → valores 0..15
                v = 0;
            }

            vTaskDelay(pdMS_TO_TICKS(500));
        }
        else {
            // botão solto → atualiza LEDs conforme bits de v
            gpio_set_level(LED1, (v >> 0) & 1);
            gpio_set_level(LED2, (v >> 1) & 1);
            gpio_set_level(LED3, (v >> 2) & 1);
            gpio_set_level(LED4, (v >> 3) & 1);

            vTaskDelay(pdMS_TO_TICKS(50));
        }

        printf("v = %d\n", v);
    }
}
