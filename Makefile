all:
	gcc `xml2-config --cflags` ServerV5ProiectFinal.c -o quizserver `xml2-config --libs` -pthread
	gcc ClientV2ProiectFinal.c -o jucator
