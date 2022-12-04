.\esptool\esptool.exe erase_flash

.\esptool\esptool.exe --before default_reset --after hard_reset --baud 115200 --chip esp32c3 write_flash -z --flash_size detect 0x0000 .\flash_bin\bootloader_dio_80m.bin 0x8000 .\flash_bin\partitions.bin 0xe000 .\flash_bin\boot_app0.bin 0x10000 .\firmware.bin

pause