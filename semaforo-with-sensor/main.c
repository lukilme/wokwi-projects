#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "ultrasonic.c"

#define ECHO_GPIO           GPIO_NUM_12
#define TRIGGER_GPIO        GPIO_NUM_13
#define LED_VERDE           GPIO_NUM_17
#define LED_AMARELO         GPIO_NUM_5
#define LED_VERMELHO        GPIO_NUM_18

#define MAX_DISTANCE_CM     500
#define DELAY_SEMAFORO_MS   1000 
#define DELAY_SENSOR_MS     200 
#define STACK_SIZE          (configMINIMAL_STACK_SIZE * 4)

#define TEMPO_VERDE         10
#define TEMPO_AMARELO       1
#define TEMPO_VERMELHO      5
#define CICLO_TOTAL         (TEMPO_VERDE + TEMPO_AMARELO + TEMPO_VERMELHO)

TaskHandle_t handle_semaforo = NULL;

typedef enum {
    SEMAFORO_VERDE = 0,
    SEMAFORO_AMARELO,
    SEMAFORO_VERMELHO,
    SEMAFORO_ERRO
} semaforo_state_t;

typedef struct {
    gpio_num_t pin;
    const char* nome;
} led_config_t;

static const led_config_t leds[] = {
    {LED_VERDE, "Verde"},
    {LED_AMARELO, "Amarelo"},
    {LED_VERMELHO, "Vermelho"}
};

static const char *TAG = "SEMAFORO";
static ultrasonic_sensor_t sensor;
static volatile uint8_t contador_ciclo = 0;

void init_leds(void) {
    for (int i = 0; i < 3; i++) {
        gpio_reset_pin(leds[i].pin);
        gpio_set_direction(leds[i].pin, GPIO_MODE_OUTPUT);
        gpio_set_level(leds[i].pin, 0); 
    }
}

void atualizar_semaforo(semaforo_state_t estado) {
    for (int i = 0; i < 3; i++) {
        gpio_set_level(leds[i].pin, 0);
    }

    switch (estado) {
        case SEMAFORO_VERDE:
            gpio_set_level(LED_VERDE, 1);
            break;
        case SEMAFORO_AMARELO:
            gpio_set_level(LED_AMARELO, 1);
            break;
        case SEMAFORO_VERMELHO:
            gpio_set_level(LED_VERMELHO, 1);
            break;
        default:
            for (int i = 0; i < 3; i++) {
                gpio_set_level(leds[i].pin, 1);
            }
            break;
    }
}

semaforo_state_t obter_estado_semaforo(uint8_t contador) {
    uint8_t posicao = contador % CICLO_TOTAL;
    if (posicao < TEMPO_VERDE) return SEMAFORO_VERDE;
    if (posicao < TEMPO_VERDE + TEMPO_AMARELO) return SEMAFORO_AMARELO;
    return SEMAFORO_VERMELHO;
}

void task_controle_semaforo(void *pvParameters) {
    while (true) {
        atualizar_semaforo(obter_estado_semaforo(contador_ciclo));
        contador_ciclo = (contador_ciclo + 1) % CICLO_TOTAL;
        vTaskDelay(pdMS_TO_TICKS(DELAY_SEMAFORO_MS));
    }
}

void task_sensor(void *pvParameters) {
    float distancia;
    while (true) {
        ultrasonic_measure(&sensor, MAX_DISTANCE_CM, &distancia);
        
        if (distancia <= 1.0) {
            if (handle_semaforo != NULL && eTaskGetState(handle_semaforo) != eSuspended) {
                vTaskSuspend(handle_semaforo);
                atualizar_semaforo(SEMAFORO_VERMELHO);
           
            }
        } else {
            if (handle_semaforo != NULL && eTaskGetState(handle_semaforo) == eSuspended) {
                vTaskResume(handle_semaforo);
            
            }
        }

        vTaskDelay(pdMS_TO_TICKS(DELAY_SENSOR_MS));
    }
}

void init_sensor_ultrassonico(void) {
    sensor.trigger_pin = TRIGGER_GPIO;
    sensor.echo_pin = ECHO_GPIO;
    ultrasonic_init(&sensor);
}

void app_main(void) {
    init_leds();
    init_sensor_ultrassonico();
    
    xTaskCreate(
        task_controle_semaforo,
        "controle_semaforo",
        STACK_SIZE,
        NULL,
        5,
        &handle_semaforo 
    );

    xTaskCreate(
        task_sensor,
        "sensor_task",
        STACK_SIZE,
        NULL,
        5,
        NULL
    );
}
