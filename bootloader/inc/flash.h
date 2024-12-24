#define FLASH_UNLOCK_KEY1 (0x45670123U)
#define FLASH_UNLOCK_KEY2 (0xCDEF89ABU)
#define OB_UNLOCK_KEY1    (0x08192A3BU)
#define OB_UNLOCK_KEY2    (0x4C5D6E7FU)

#define FLASH_BASE        (0x40023C00U)
#define FLASH_ACR         (FLASH_BASE + 0x00U)
#define FLASH_KEYR        (FLASH_BASE + 0x04U)
#define FLASH_OPTKEYR     (FLASH_BASE + 0x08U)
#define FLASH_SR          (FLASH_BASE + 0x0CU)
#define FLASH_CR          (FLASH_BASE + 0x10U)
#define FLASH_OPTCR       (FLASH_BASE + 0x14U)
#define FLASH_OPTCR1      (FLASH_BASE + 0x18U)
#define FLASH_OB1         (0x1FFFC000U)
#define FLASH_OB2         (0x1FFFC008U)
#define FLASH_OB3         (0x1FFEC000U)
#define FLASH_OB4         (0x1FFEC008U)

#define FLASH_OTP_BASE    (0x1FFF7800U)

#define FLASH_SUCCESS     (1U)
#define FLASH_FAIL        (0U)

#define FLASH_SEC_1_START (0U)
#define FLASH_SEC_1_NUM   (12U)
#define FLASH_SEC_2_START (12U)
#define FLASH_SEC_2_NUM   (12U)

unsigned int flash_unlock(void);
unsigned int flash_lock(void);
unsigned int option_byte_unlock(void);
unsigned int option_byte_lock(void);
unsigned int flash_sec_erase(unsigned int sec_no);
unsigned int flash_seq_erase(unsigned int start_sec, unsigned int num_sec);
unsigned int configure_flash_sector_rw_prot(unsigned int sector_details, unsigned char prot_mode);
unsigned int get_sector_status(unsigned int *sector_1, unsigned int *sector_2);
void read_otp_sector(unsigned int block_no, unsigned char *storing_buf);