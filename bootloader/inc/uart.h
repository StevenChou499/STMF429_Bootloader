#define RCC_BASE       (0x40023800U)
#define RCC_AHB1ENR    (RCC_BASE + 0x30)
#define RCC_APB1ENR    (RCC_BASE + 0x40)
#define GPIOAEN        (1U << 0)
#define GPIODEN        (1U << 3)
#define USART2EN       (1U << 17)
#define USART3EN       (1U << 18)

#define GPIOA_BASE     (0x40020000U)
#define GPIOA_MODER    (GPIOA_BASE + 0x00)
#define GPIOA_AFRL     (GPIOA_BASE + 0x20)
#define GPIOA_AFRH     (GPIOA_BASE + 0x24)

#define GPIOD_BASE     (0x40020C00U)
#define GPIOD_MODER    (GPIOD_BASE + 0x00)
#define GPIOD_AFRH     (GPIOD_BASE + 0x24)

#define INPUT_MODE     (0x0U)
#define OUTPUT_MODE    (0x1U)
#define ALT_FUNC_MODE  (0x2U)
#define ANALOG_MODE    (0x3U)

#define ALT_FUNC_0     (0x0U)
#define ALT_FUNC_1     (0x1U)
#define ALT_FUNC_2     (0x2U)
#define ALT_FUNC_3     (0x3U)
#define ALT_FUNC_4     (0x4U)
#define ALT_FUNC_5     (0x5U)
#define ALT_FUNC_6     (0x6U)
#define ALT_FUNC_7     (0x7U)
#define ALT_FUNC_8     (0x8U)
#define ALT_FUNC_9     (0x9U)
#define ALT_FUNC_10    (0x10U)
#define ALT_FUNC_11    (0x11U)
#define ALT_FUNC_12    (0x12U)
#define ALT_FUNC_13    (0x13U)
#define ALT_FUNC_14    (0x14U)
#define ALT_FUNC_15    (0x15U)

#define USART2_BASE    (0x40004400U)
#define USART2_SR      (USART2_BASE + 0x00)
#define USART2_DR      (USART2_BASE + 0x04)
#define USART2_BRR     (USART2_BASE + 0x08)
#define USART2_CR1     (USART2_BASE + 0x0C)

#define USART3_BASE    (0x40004800U)
#define USART3_SR      (USART3_BASE + 0x00)
#define USART3_DR      (USART3_BASE + 0x04)
#define USART3_BRR     (USART3_BASE + 0x08)
#define USART3_CR1     (USART3_BASE + 0x0C)

#define USART_EN       (1U << 13)  // USART enable
#define USART_TE       (1U << 3)   // USART transmitter enable
#define USART_RE       (1U << 2)   // USART receiver enable
#define USART_TXE      (1U << 7)   // USART transmit data register empty
#define USART_TC       (1U << 6)   // USART transmitter complete
#define USART_RXNE     (1U << 5)   // USART read data register not empty

void UART2_Init(void);

void UART2_Transmit(const unsigned char *buf, int len);

void UART2_Receive(unsigned char *buf, int len);

void UART3_Init(void);

void UART3_Transmit(const unsigned char *buf, int len);

void UART3_Receive(unsigned char *buf, int len);