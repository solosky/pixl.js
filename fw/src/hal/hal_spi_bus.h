#ifndef HAL_SPI_BUS_H
#define HAL_SPI_BUS_H


typedef struct {
    void* p_rx_buffer;
    size_t rx_length;
    void* p_tx_buffer;
    size_t tx_length;
}spi_transaction_t;

typedef struct {
    uint32_t cs_pin;
} spi_device_t;


void hal_spi_bus_init();
void hal_spi_bus_attach(spi_device_t* p_dev);
void hal_spi_bus_aquire(spi_device_t* p_dev);
void hal_spi_bus_release(spi_device_t* p_dev);
ret_code_t hal_spi_bus_xfer(spi_transaction_t* p_trans);


#endif 