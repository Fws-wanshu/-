;  STM32F103VET6 Linker Script for Keil MDK
;  Flash: 512KB (0x8000000 - 0x807FFFF)
;  RAM:   64KB  (0x20000000 - 0x2000FFFF)

LR_IROM1 0x08000000 0x00080000  {    ; load region size_region
  ER_IROM1 0x08000000 0x00080000  {  ; load address = execution address
   *.o (RESET, +First)
   *(InRoot$$Sections)
   .ANY (+RO)
  }
  RW_IRAM1 0x20000000 0x00010000  {  ; RW data
   .ANY (+RW +ZI)
  }
}
