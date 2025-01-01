#define RCC_BASE        (0x40023800U)
#define RCC_AHB1ENR     (RCC_BASE + 0x30)
#define GPIOBEN         (1 << 1U)
#define GPIOCEN         (1 << 2U)
#define GPIOB           (0x40020400U)
#define GPIOC           (0x40020800U)
#define PIN_13          (1 << 13U)
#define PIN_14          (1 << 14U)

typedef enum {
    RESET = 0, 
    SET   = 1,
} GPIO_State_t;

void LD3_Init(void);
void LD3_Toggle(void);

void User_Btn_Init(void);
unsigned char User_Btn_Read(void);