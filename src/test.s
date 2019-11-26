; 65816 Assembly file

.MEMORYMAP
	DEFAULTSLOT 0
	SLOTSIZE $8000
	SLOT 0 $8000
	.ENDME

.ROMBANKMAP
	BANKSTOTAL 1
	BANKSIZE $8000
	BANKS 1
.ENDRO

.BANK 00  SLOT 0

start:
			rep #$30
			lda.w #$1234
			ldx.w #$0000
			ldy.w #$ffff
loop:
			inx
			dey
			jmp loop