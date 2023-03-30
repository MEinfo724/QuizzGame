all:
	gcc `xml2-config --cflags` ServerV5ProiectFinal.c -o vedm `xml2-config --libs` -pthread
	gcc ClientV2ProiectFinal.c -o damn1