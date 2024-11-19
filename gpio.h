#define RCC_BASE        (0x40023800U)
#define RCC_AHB1ENR     (RCC_BASE + 0x30)
#define GPIOBEN         (1 << 1U)
#define GPIOB           (0x40020400U)
#define PIN_14          (1 << 14U)

void LD3_Init(void);
void LD3_Toggle(void);