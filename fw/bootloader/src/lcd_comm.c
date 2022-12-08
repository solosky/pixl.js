#include "lcd_comm.h"
#include <string.h>
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"

#define DATAINTERFACE_WAIT_TICKS        	0x5000

#define BUFF_MAX      										260
typedef struct {
    volatile bool   enable;
    volatile bool   xfer_done;
    uint16_t        event_wait;
    nrf_drv_spi_t   instance;
    uint8_t         tx_buff[BUFF_MAX];
}   lcd_spi_t;

static lcd_spi_t m_lcd_spi;

static void tft_spi_event(nrf_drv_spi_evt_t const * p_event, void * p_context)
{
    m_lcd_spi.xfer_done = true;
}

static uint32_t lcd_spi_init(void)
{
    if (m_lcd_spi.enable)
        return NRF_ERROR_BUSY;

    uint32_t err_code = NRF_SUCCESS;
    nrf_drv_spi_config_t spi_1_cfg = {  .ss_pin       = NRF_DRV_SPI_PIN_NOT_USED,             		\
                                        .sck_pin      = LCD_SPI_SCL_PIN,            		\
                                        .miso_pin     = FLASH_SPI_MISO_PIN,          									\
                                        .mosi_pin     = LCD_SPI_MOSI_PIN,           		\
                                        .irq_priority = APP_IRQ_PRIORITY_LOW_MID,  			\
                                        .orc          = 0xFF,                           \
                                        .mode         = NRF_DRV_SPI_MODE_0,             \
                                        .frequency    = NRF_DRV_SPI_FREQ_8M,            \
                                        .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST,\
                                     };
    
    memset(&m_lcd_spi, 0x00, sizeof(lcd_spi_t));
    
    nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(0);
    m_lcd_spi.instance = spi_instance;
    err_code = nrf_drv_spi_init(&m_lcd_spi.instance, &spi_1_cfg, tft_spi_event, NULL);
    if (err_code != NRF_SUCCESS)
        return err_code;
    
    m_lcd_spi.enable = true;
    return err_code;
}

static void lcd_spi_uninit(void)
{
    if (m_lcd_spi.enable == false)
        return;
    nrf_drv_spi_uninit(&m_lcd_spi.instance);
    nrf_gpio_cfg_default(LCD_SPI_CS_PIN);
    nrf_gpio_cfg_default(LCD_SPI_SCL_PIN);
    //nrf_gpio_cfg_default(LCD_SPI_MISO_PIN);
    nrf_gpio_cfg_default(LCD_SPI_MOSI_PIN);
    nrf_gpio_cfg_default(LCD_BL_PIN);
    memset(&m_lcd_spi, 0x00, sizeof(lcd_spi_t));
}

static uint32_t lcd_spi_write(const uint8_t * data, const uint8_t len)
{
    if (m_lcd_spi.enable == false)
        return NRF_ERROR_INVALID_STATE;

    uint32_t err_code;
		
    memset(m_lcd_spi.tx_buff, 0xff, len);
		if(data != NULL){
				memcpy(m_lcd_spi.tx_buff, data, len);
		}
    m_lcd_spi.xfer_done = false;
    m_lcd_spi.event_wait = DATAINTERFACE_WAIT_TICKS;
    err_code = nrf_drv_spi_transfer(&m_lcd_spi.instance, m_lcd_spi.tx_buff, len, NULL, 0);
    if (err_code == NRF_SUCCESS)
    {
        while (!m_lcd_spi.xfer_done && m_lcd_spi.event_wait)
            m_lcd_spi.event_wait--;

        if (!m_lcd_spi.event_wait)
        {
            m_lcd_spi.xfer_done = true;
            return NRF_ERROR_TIMEOUT;
        }
    }
    return err_code;
}


static uint32_t lcd_spi_read(const uint8_t * data, const uint8_t len)
{
    if (m_lcd_spi.enable == false)
        return NRF_ERROR_INVALID_STATE;

    uint32_t err_code;
		
    m_lcd_spi.xfer_done = false;
    m_lcd_spi.event_wait = DATAINTERFACE_WAIT_TICKS;
    err_code = nrf_drv_spi_transfer(&m_lcd_spi.instance, NULL, 0, (uint8_t*)data, len);
    if (err_code == NRF_SUCCESS)
    {
        while (!m_lcd_spi.xfer_done && m_lcd_spi.event_wait)
            m_lcd_spi.event_wait--;

        if (!m_lcd_spi.event_wait)
        {
            m_lcd_spi.xfer_done = true;
            return NRF_ERROR_TIMEOUT;
        }
    }
    return err_code;
}



void lcd_comm_init(void)
{
		lcd_spi_init();
        nrf_gpio_cfg_output(LCD_RST_PIN);
		nrf_gpio_cfg_output(LCD_DC_RS_PIN);
        nrf_gpio_cfg_output(LCD_BL_PIN);
        nrf_gpio_cfg_output(LED_1);

        nrf_gpio_cfg_output(FALSH_SPI_CS_PIN);
          nrf_gpio_cfg_output(LCD_SPI_CS_PIN);

		nrf_gpio_pin_set(LCD_RST_PIN);
		nrf_gpio_pin_clear(LCD_DC_RS_PIN);
        nrf_gpio_pin_clear(LCD_BL_PIN);
        nrf_gpio_pin_set(LED_1);
        nrf_gpio_pin_set(FALSH_SPI_CS_PIN);
        nrf_gpio_pin_set(LCD_SPI_CS_PIN);
}

void lcd_comm_uninit(void)
{
		lcd_spi_uninit();
		nrf_gpio_cfg_default(LCD_RST_PIN);
		nrf_gpio_cfg_default(LCD_DC_RS_PIN);
        nrf_gpio_cfg_default(LCD_BL_PIN);
        nrf_gpio_cfg_default(LED_1);
}

void lcd_reset(void)
{
		nrf_delay_ms(50);
		LCD_RST_SET(ON);
		nrf_delay_ms(100);
		LCD_RST_SET(OFF);
		nrf_delay_ms(120);
		LCD_RST_SET(ON);
}

void lcd_write_command(uint8_t data)
{  
        nrf_gpio_pin_write(LCD_SPI_CS_PIN, 0);
		LCD_DC_RS_SET(LCD_RS);
		lcd_spi_write(&data, 1);
        nrf_gpio_pin_write(LCD_SPI_CS_PIN, 1);
}

void lcd_write_data(uint8_t *data, uint8_t len)
{
      nrf_gpio_pin_write(LCD_SPI_CS_PIN, 0);
		LCD_DC_RS_SET(LCD_DC);
		lcd_spi_write(data, len);
        nrf_gpio_pin_write(LCD_SPI_CS_PIN, 1);
}



/////flash test

void flash_write_read(uint8_t* tx_data, uint8_t tx_len, uint8_t* rx_data, uint32_t rx_len){
    nrf_gpio_pin_write(FALSH_SPI_CS_PIN, 0);
    APP_ERROR_CHECK(lcd_spi_write(tx_data, tx_len));
    if(rx_len > 0){
    APP_ERROR_CHECK(lcd_spi_read(rx_data, rx_len));
    }
    nrf_gpio_pin_write(FALSH_SPI_CS_PIN, 1);
}

void flash_write_write(uint8_t* tx_data, uint8_t tx_len, uint8_t* rx_data, uint32_t rx_len){
    nrf_gpio_pin_write(FALSH_SPI_CS_PIN, 0);
    APP_ERROR_CHECK(lcd_spi_write(tx_data, tx_len));
    if(rx_len > 0){
    APP_ERROR_CHECK(lcd_spi_write(rx_data, rx_len));
    }
    nrf_gpio_pin_write(FALSH_SPI_CS_PIN, 1);
}



