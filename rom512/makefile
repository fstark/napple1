test:512KA1COMPIL.BIN
	( cd .. ; ( cd src && make ) && ./napple1 rom512/rom512.cmd )

512KA1COMPIL.BIN: 32KBOOT.BIN 32KA1COMPIL.BIN WOZDLE.BIN
	cat 32KBOOT.BIN 32KA1COMPIL.BIN WOZDLE.BIN /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin /tmp/32kzero.bin > 512KA1COMPIL.BIN

32KBOOT.BIN: BOOT
	dd if=/dev/zero of=/tmp/32kzero.bin bs=1024 count=32
	dd if=/dev/zero of=/tmp/8kzero.bin bs=1024 count=8
	cat /tmp/8kzero.bin BOOT /tmp/32kzero.bin > /tmp/tmp.bin
	dd if=/tmp/tmp.bin of=32KBOOT.BIN bs=1024 count=32

BOOT: boot.asm
	xa -XCA65 -C -o BOOT boot.asm

clean:
	rm -f BOOT 32KBOOT.BIN 512KA1COMPIL.BIN
	rm -f /tmp/32kzero.bin /tmp/8kzero.bin /tmp/tmp.bin
