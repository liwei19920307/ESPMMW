.\esptool\esptool.exe erase_flash

.\esptool\esptool.exe --before default_reset --after hard_reset --baud 115200 --chip esp32s3 write_flash -z --flash_size detect 0x10000 .\firmware.bin 0x0000 .\s3_bin\bootloader.bin 0x8000 .\s3_bin\partitions.bin 0xe000 .\s3_bin\boot_app0.bin

pause