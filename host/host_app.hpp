#include <string>
#include <iostream>
#include <cstdint>

#define TX_BUFFER_LEN (1024U)
#define RX_BUFFER_LEN (512U)

typedef enum {
    BTLDR_GET_VER      = 0x1, 
    BTLDR_GET_HELP     = 0x2, 
    BTLDR_GET_CID      = 0x3, 
    BTLDR_GET_RDP      = 0x4, 
    BTLDR_GO_ADDR      = 0x5, 
    BTLDR_FLASH_ERASE  = 0x6, 
    BTLDR_MEM_READ     = 0x7, 
    BTLDR_MEM_WRITE    = 0x8, 
    BTLDR_EN_RW_PROT   = 0x9, 
    BTLDR_DIS_RW_PROT  = 0xA, 
    BTLDR_READ_SEC_STA = 0xB, 
    BTLDR_READ_OTP     = 0xC,
} bootloader_cmd_t;

using std::string;
    
typedef union {
    uint32_t int_value;
    uint8_t  ch_array[4];
} int_or_chars;

class host_app {
private:
    typedef void (host_app::*functionpointer)();
    int32_t mcu_fd;
    uint32_t current_cmd;
    uint8_t tx_buffer[TX_BUFFER_LEN];
    uint8_t rx_buffer[RX_BUFFER_LEN];
    uint32_t tx_cmd_len;
    uint32_t rx_cmd_len;
    uint32_t mem_read_addr;
    uint32_t mem_read_len;
    functionpointer func_table[13];

    void write_getver_cmd(void);
    void write_gethelp_cmd(void);
    void write_getcid_cmd(void);
    void write_get_read_prot_cmd(void);
    void write_goto_addr_cmd(void);
    void write_erase_flash_section_cmd(void);
    void write_mem_read_cmd(void);
    void write_mem_write_cmd(void);
    void write_en_rw_prot_cmd(void);
    void write_dis_rw_prot_cmd(void);
    void write_read_sector_status_cmd(void);
    void write_read_otp_cmd(void);

    uint32_t get_crc(uint8_t *pBuf, uint32_t length);
    void append_crc(uint8_t *pBuf, uint32_t length);
    void r_read(uint8_t *pBuf, uint32_t length);
        
        
public:
    host_app(string portname);
    ~host_app();
    void show_prompt(void);
    bool get_user_input(void);
    bool parse_command(void);
    void get_bootloader_respond(void);
};