#include "lgfx_tft.h"

LGFX_TFT::LGFX_TFT(uint16_t SCREEN_WIDTH, uint16_t SCREEN_HEIGHT, uint8_t bl_pin)
{
    {
        auto cfg = _bus_instance.config();

        // SPI bus settings
        cfg.spi_host = SPI2_HOST; // Select the SPI to use ESP32-S2,C3: SPI2_HOST or SPI3_HOST / ESP32: VSPI_HOST or HSPI_HOST
        // * With the ESP-IDF version upgrade, the description of VSPI_HOST and HSPI_HOST will be deprecated, so if an error occurs,
        // please use SPI2_HOST and SPI3_HOST instead. Set pin number (-1 = disable)
        cfg.spi_mode = 0;                  // Set SPI communication mode (0 ~ 3)
        cfg.freq_write = 80000000;         // SPI clock during transmission (up to 80MHz, rounded to the value obtained by dividing 80MHz by an integer)
        cfg.freq_read = 20000000;          // SPI clock when receiving
        cfg.spi_3wire = true;              // Set true if receiving is done using MOSI pin
        cfg.use_lock = true;               // Set to true when using transaction lock
        cfg.dma_channel = SPI_DMA_CH_AUTO; // Set the DMA channel to use (0=DMA not used / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=automatic setting)
        // * Due to the ESP-IDF version upgrade, SPI_DMA_CH_AUTO (automatic setting) is recommended for the DMA channel. Specifying 1ch or 2ch is not recommended.
        cfg.pin_sclk = 6;  // Set SPI SCLK pin number
        cfg.pin_mosi = 7;  // Set SPI MOSI pin number
        cfg.pin_miso = -1; // Set SPI MISO pin number (-1 = disable)
        cfg.pin_dc = 2;    // Set SPI D/C pin number (-1 = disable)

        _bus_instance.config(cfg);              // Reflect the settings on the bus.
        _panel_instance.setBus(&_bus_instance); // // Set the bus on the panel.
    }

    {                                        // Set display panel control.
        auto cfg = _panel_instance.config(); // Get the structure for display panel settings.

        cfg.pin_cs = 10;   // CS pin number (-1 = disable)
        cfg.pin_rst = -1;  // RST pin number (-1 = disable)
        cfg.pin_busy = -1; // BUSY pin number (-1 = disable)

        // * The following setting values are general initial values set for each panel and the pin number to which BUSY is
        //   connected (-1 = disable), so please comment out any unknown items and try again.

        cfg.memory_width = SCREEN_WIDTH;   // Maximum width supported by driver IC
        cfg.memory_height = SCREEN_HEIGHT; // Maximum height supported by driver IC
        cfg.panel_width = SCREEN_WIDTH;    // Actual displayable width
        cfg.panel_height = SCREEN_HEIGHT;  // Actual display height
        cfg.offset_x = 0;                 // Panel X direction offset amount
        cfg.offset_y = 0;                 // Panel Y direction offset amount
        cfg.offset_rotation = 0;          // The offset of the value in the direction of rotation is 0~7 (4~7 is inverted)
        cfg.dummy_read_pixel = 8;         // Number of dummy bits to read before reading the pixel
        cfg.dummy_read_bits = 1;          // Number of virtual read bits before reading data other than pixels
        cfg.readable = false;             // Set to true if data can be read
        cfg.invert = true;                // Set to true if the panel's light and dark are inverted
        cfg.rgb_order = false;            // Set to true if the red and blue colors of the panel are swapped
        cfg.dlen_16bit = false;           // Set to true for panels that send data length in 16-bit units
        cfg.bus_shared = false;           // Set to true if the bus is shared with the SD card (use drawJpgFile etc. to perform bus control)

        _panel_instance.config(cfg);
    }

    _bl_pin = bl_pin;

    if(_bl_pin >= 0) {
        pinMode(_bl_pin, OUTPUT);
    }

    BLOn();

    setPanel(&_panel_instance); // Set the panel to use.
}

void LGFX_TFT::BLOn()
{
    if(_bl_pin >= 0) {
        digitalWrite(_bl_pin, HIGH);
    }
};

void LGFX_TFT::BLOff()
{
    if(_bl_pin >= 0) {
        digitalWrite(_bl_pin, LOW);
    }
}
