#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_

#include <stdint.h>

typedef uint16_t hal_gpio_t;
#define HAL_GPIO_PORT_STRIDE  16
#define HAL_GPIO_(port,pin) ((hal_gpio_t)((port) * HAL_GPIO_PORT_STRIDE + (pin)))

typedef enum {

    HAL_GPIO_PORTA = 0,
    HAL_GPIO_PORTB = 1,
    HAL_GPIO_PORTC = 2,
    HAL_GPIO_PORTD = 3,
    HAL_GPIO_PORTE = 4,
    HAL_GPIO_PORTF = 5,
    HAL_GPIO_PORTG = 6,
    HAL_GPIO_PORTH = 7,

}hal_gpio_port_e;


typedef enum {
    
    HAL_GPIO_MODE_INPUT,
    HAL_GPIO_MODE_INPUT_PULLUP,
    HAL_GPIO_MODE_OUTPUT_PP,
    HAL_GPIO_MODE_OUTPUT_OD,
    HAL_GPIO_MODE_ANALOG,
    HAL_GPIO_MODE_AF,           /* 复用功能：给 UART/SPI/I2C 等外设用 */
    
} hal_gpio_mode_e;

typedef enum {
    
    HAL_GPIO_LEVEL_LOW = 0,
    HAL_GPIO_LEVEL_HIGH = 1,
    
} hal_gpio_level_e;

/* 复用功能编号（GD32 的 AF0~AF15，对应具体外设查芯片数据手册） */
typedef enum {
    HAL_GPIO_AF0  = 0,
    HAL_GPIO_AF1,
    HAL_GPIO_AF2,
    HAL_GPIO_AF3,
    HAL_GPIO_AF4,
    HAL_GPIO_AF5,
    HAL_GPIO_AF6,
    HAL_GPIO_AF7,
    HAL_GPIO_AF8,
    HAL_GPIO_AF9,
    HAL_GPIO_AF10,
    HAL_GPIO_AF11,
    HAL_GPIO_AF12,
    HAL_GPIO_AF13,
    HAL_GPIO_AF14,
    HAL_GPIO_AF15,
} hal_gpio_af_e;

typedef struct {

    uint8_t (*init)(hal_gpio_t io, hal_gpio_mode_e mode);
    uint8_t (*write)(hal_gpio_t io, hal_gpio_level_e level);
    hal_gpio_level_e (*read)(hal_gpio_t io);
    void (*toggle)(hal_gpio_t io);
    uint8_t (*af_set)(hal_gpio_t io, hal_gpio_af_e af);

} hal_gpio_ops_t;

void hal_gpio_register_ops(const hal_gpio_ops_t *ops);

uint8_t hal_gpio_init(hal_gpio_t io, hal_gpio_mode_e mode);
uint8_t hal_gpio_write(hal_gpio_t io, hal_gpio_level_e level);
hal_gpio_level_e hal_gpio_read(hal_gpio_t io);
void hal_gpio_toggle(hal_gpio_t io);
uint8_t hal_gpio_af_set(hal_gpio_t io, hal_gpio_af_e af);

#endif

