install:
	gcc -Wall *.c -o netpbm.exe

test:
	./netpbm -e "img/test.ppm" -I -s "output/Invertida.ppm"
	./netpbm -e "img/test.ppm" -m -s "output/VolteadaV.ppm"
	./netpbm -e "img/test.ppm" -v -s "output/VolteadaH.ppm"
	./netpbm -e "img/test.ppm" -r i -s "output/RotadaI.ppm"
	./netpbm -e "img/test.ppm" -r d -s "output/RotadaD.ppm"
	./netpbm -e "img/test.ppm" -masc "img/masc.pgm" "img/super.ppm" -s "output/Mascara.ppm"
	./netpbm -e "img/test.pgm" -I -s "output/Invertida.pgm"
	./netpbm -e "img/test.pgm" -m -s "output/VolteadaV.pgm"
	./netpbm -e "img/test.pgm" -v -s "output/VolteadaH.pgm"
	./netpbm -e "img/test.pgm" -r i -s "output/RotadaI.pgm"
	./netpbm -e "img/test.pgm" -r d -s "output/RotadaD.pgm"
	./netpbm -e "img/test.pgm" -masc "img/masc.pgm" "img/super.ppm" -s "output/Mascara.pgm"