add_executable( sd_card 
                sd_card.c
                hw_config.c)


target_link_libraries(sd_card
        pico_stdlib              # for core functionality
        pico_cyw43_arch_none     # we need Wifi to access the GPIO, but we don't need anything else
        hardware_spi
        FatFs_SPI
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(sd_card)


#Enable use output, UART output
pico_enable_stdio_usb(sd_card 1)
pico_enable_stdio_uart(sd_card 1)

# add url via pico_set_program_url
example_auto_set_url(sd_card)
