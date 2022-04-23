# Bootloader

## Memory map

| Physical address          | Content            | Size         | Logical address    |
|---------------------------|--------------------|--------------|--------------------|
| 0x00000800                | Stage 1 bootloader | 0x0800       | 0x0000000000000800 |
| 0x00001000                | Bios Font          | 0x1000       | 0x0000000000001000 |
| 0x00002000                | Stage 2 bootloader | 0x8000       | 0x0000000000002000 |
| 0x0000A000                | Memory map         | 24 * entries | 0x000000000000A000 |
| Load address              | Kernel image       | ebss - stext | 0xFFFFFFFF80000000 |
| Previous + (ebss - stext) | Paging tables      | 0xA000       |          -         |
| Previous + 0xA000         | Kernel init stack  | Stack size   | Top - Stack size   |
