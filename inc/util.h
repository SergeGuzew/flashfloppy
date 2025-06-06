/*
 * util.h
 * 
 * Utility definitions.
 * 
 * Written & released by Keir Fraser <keir.xen@gmail.com>
 * 
 * This is free and unencumbered software released into the public domain.
 * See the file COPYING for more details, or visit <http://unlicense.org>.
 */

struct slot {
    char name[FF_MAX_LFN+1];
    char type[7];
    uint8_t attributes;
    uint32_t firstCluster;
    uint32_t size;
    uint32_t dir_sect, dir_ptr;
};
void fatfs_from_slot(FIL *file, const struct slot *slot, BYTE mode);

bool_t lba_within_fat_volume(uint32_t lba);

void filename_extension(const char *filename, char *extension, size_t size);

/* Fast memset/memcpy: Pointers must be word-aligned, count must be a non-zero 
 * multiple of 32 bytes. */
void memset_fast(void *s, int c, size_t n);
void memcpy_fast(void *dest, const void *src, size_t n);

void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);
int strcmp_ci(const char *s1, const char *s2); /* case insensitive */
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *dest, const char *src);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
int tolower(int c);
int toupper(int c);
int isspace(int c);

long int strtol(const char *nptr, char **endptr, int base);

void qsort_p(void *base, unsigned int nr,
             int (*compar)(const void *, const void *));

uint32_t rand(void);

unsigned int popcount(uint32_t x);

int vsnprintf(char *str, size_t size, const char *format, va_list ap)
    __attribute__ ((format (printf, 3, 0)));

int snprintf(char *str, size_t size, const char *format, ...)
    __attribute__ ((format (printf, 3, 4)));

#define le16toh(x) (x)
#define le32toh(x) (x)
#define htole16(x) (x)
#define htole32(x) (x)
#define be16toh(x) _rev16(x)
#define be32toh(x) _rev32(x)
#define htobe16(x) _rev16(x)
#define htobe32(x) _rev32(x)

uint32_t udiv64(uint64_t dividend, uint32_t divisor);

/* Arena-based memory allocation */
void *arena_alloc(uint32_t sz);
uint32_t arena_total(void);
uint32_t arena_avail(void);
void arena_init(void);

/* Board-specific callouts */
void board_init(void);

#if LEVEL == LEVEL_debug || LEVEL == LEVEL_logfile
/* Log output, to serial console or logfile. */
int vprintk(const char *format, va_list ap)
    __attribute__ ((format (printf, 1, 0)));
int printk(const char *format, ...)
    __attribute__ ((format (printf, 1, 2)));
#elif LEVEL == LEVEL_prod
static inline int vprintk(const char *format, va_list ap) { return 0; }
static inline int printk(const char *format, ...) { return 0; }
#endif

#define log(f, a...) printk("%s: " f, LOG_PREFIX, ## a)

#if LEVEL == LEVEL_logfile
/* Logfile management */
void logfile_flush(FIL *file);
#else
#define logfile_flush(f) ((void)0)
#endif

#if LEVEL == LEVEL_debug
/* Serial console control */
void console_init(void);
void console_sync(void);
void console_crash_on_input(void);
#else
#define console_init() ((void)0)
#define console_sync() IRQ_global_disable()
#define console_crash_on_input() ((void)0)
#endif

/* CRC-CCITT */
uint16_t crc16_ccitt(const void *buf, size_t len, uint16_t crc);

/* Display setup and identification. */
void display_init(void);
extern uint8_t display_type;
#define DT_NONE     0
#define DT_LCD_OLED 1
#define DT_LED_7SEG 2

/* Speaker. */
void speaker_init(void);
void speaker_pulse(void);
void speaker_notify_insert(unsigned int slotnr);
void speaker_notify_eject(void);

/* Display: 3-digit 7-segment display */
void led_7seg_init(void);
void led_7seg_write_raw(const uint8_t *d);
void led_7seg_write_string(const char *p);
void led_7seg_write_decimal(unsigned int val);
void led_7seg_display_setting(bool_t enable);
int led_7seg_nr_digits(void);

/* Display: I2C 16x2 LCD */
bool_t lcd_init(void);
void lcd_clear(void);
void lcd_write(int col, int row, int min, const char *str);
void lcd_backlight(bool_t on);
void lcd_sync(void);
extern uint8_t lcd_columns, lcd_rows;

/* FF OSD (On Screen Display) */
extern bool_t has_osd;
extern uint8_t osd_buttons_tx; /* Gotek -> FF_OSD */
extern uint8_t osd_buttons_rx; /* FF_OSD -> Gotek */

/* USB stack processing */
void usbh_msc_init(void);
void usbh_msc_buffer_set(uint8_t *buf);
void usbh_msc_process(void);
bool_t usbh_msc_inserted(void);

/* Navigation/UI frontend */
uint16_t get_slot_nr(void);
bool_t set_slot_nr(uint16_t slot_nr);
void set_slot_name(const char *name);
bool_t get_img_cfg(struct slot *slot);
void IRQ_rotary(void);

enum { DM_normal=0, DM_banner, DM_menu };
extern uint8_t display_mode;
extern uint8_t board_id;
extern uint8_t has_kc30_header;

/* Gotek board revisions */
#define BRDREV_Gotek_standard 0xf
#define BRDREV_Gotek_enhanced 0x0
#define BRDREV_Gotek_sd_card  0x1
#define gotek_enhanced() (board_id != BRDREV_Gotek_standard)

extern uint32_t board_rotary_exti_mask;
void board_setup_rotary_exti(void);
unsigned int board_get_rotary(void);
unsigned int board_get_buttons(void);
#define B_LEFT 1
#define B_RIGHT 2
#define B_SELECT 4
void board_jc_set_mode(unsigned int mode);
bool_t board_jc_strapped(void);

/* Build info. */
extern const char fw_ver[];
extern const char build_date[];
extern const char build_time[];

/* Bootloader mode flag. */
extern volatile uint32_t _reset_flag;
#define RESET_FLAG_BOOTLOADER 0xdeadbeefu

/* Text/data/BSS address ranges. */
extern char _stext[], _etext[];
extern char _smaintext[], _emaintext[];
extern char _sdat[], _edat[], _ldat[];
extern char _sbss[], _ebss[];

/* Stacks. */
extern uint32_t _thread_stacktop[], _thread_stackbottom[];
extern uint32_t _irq_stacktop[], _irq_stackbottom[];

/* Default exception handler. */
void EXC_unused(void);

/* IRQ priorities, 0 (highest) to 15 (lowest). */
#define RESET_IRQ_PRI         0
#define FLOPPY_IRQ_SEL_PRI    1
#define FLOPPY_IRQ_WGATE_PRI  2
#define FLOPPY_IRQ_STEP_PRI   3
#define TIMER_IRQ_PRI         4
#define WDATA_IRQ_PRI         7
#define RDATA_IRQ_PRI         8
#define FLOPPY_SOFTIRQ_PRI    9
#define I2C_IRQ_PRI          13
#define USB_IRQ_PRI          14
#define CONSOLE_IRQ_PRI      15

/*
 * Local variables:
 * mode: C
 * c-file-style: "Linux"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
