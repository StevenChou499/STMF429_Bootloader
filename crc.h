#define RCC_BASE       (0x40023800U)
#define RCC_AHB1ENR    (RCC_BASE + 0x30)

#define CRCEN          (1 << 12U)

#define CRC_BASE       (0x40023000U)
#define CRC_DR         (CRC_BASE + 0x00U)
#define CRC_IDR        (CRC_BASE + 0x04U)
#define CRC_CR         (CRC_BASE + 0x08U)

#define CRC_RESET      (1 << 0U)

// Definition for CRC correctness
typedef enum crc_result_t {
    CRC_ERROR   = 0x0,
    CRC_CORRECT = 0x1,
} crc_result;

void CRC32_Init(void);
unsigned int CRC32_Calculate(unsigned char *pData, unsigned int len);
unsigned int CRC32_verify(unsigned char *pData, unsigned int len);