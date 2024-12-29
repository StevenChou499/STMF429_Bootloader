#define VERSION                   0x13
#define ACK                       0x0A
#define NACK                      0x0C

#define DBGMCU_IDCODE_ADDR        (0xE0042000U)

typedef enum BL_CMD {
    BL_GET_VER_CMD            = 0x50,
    BL_GET_HELP_CMD           = 0x51,
    BL_GET_CID_CMD            = 0x52,
    BL_GET_RDP_STATUS_CMD     = 0x53,
    BL_GET_GO_TO_ADDR_CMD     = 0x54,
    BL_FLASH_ERASE_CMD        = 0x55,
    BL_MEM_READ_CMD           = 0x56,
    BL_MEM_WRITE_CMD          = 0x57,
    BL_EN_RW_PROTECTION_CMD   = 0x58,
    BL_DIS_RW_PROTECTION_CMD  = 0x59,
    BL_READ_SECTOR_STATUS_CMD = 0x5A,
    BL_OTP_READ_CMD           = 0x5B,
} BL_CMD_t;

typedef struct {
    unsigned char  btldr_version;
    const char    *btldr_help_msg;
} btldr_strct_t;

void bootloader_init(void);
void parse_bootloader_cmd(void);
void bootloader_handle_getver_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_gethelp_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_getcid_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_getrdp_status_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_goaddr_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_flash_erase_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_mem_read_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_mem_write_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_en_rw_prot_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_dis_rw_prot_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_read_sector_status_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
void bootloader_handle_read_otp_cmd(unsigned char *rx_buffer, unsigned int cmd_len);
unsigned char get_bootloader_version(void);
unsigned int get_mcu_chip_id(void);
unsigned int get_rdp_status(void);

void bootloader_send_ack(void);
void bootloader_send_nack(void);