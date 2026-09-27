// hal_port.h —— 芯片平台层的对外接口（App 层只需认识这一个入口）
#ifndef __HAL_PORT_H_
#define __HAL_PORT_H_

/**
 * @brief 初始化整个硬件抽象层（注册所有外设的底层实现）
 *
 * 在 main() 里调用一次即可，之后 App 层就能直接使用 hal_gpio_init() 等接口。
 * 换芯片时：换掉 hal_port.c 对应的实现（或换成新的 *_port.c），本声明不变。
 */
void hal_port_init(void);

#endif /* __HAL_PORT_H_ */
