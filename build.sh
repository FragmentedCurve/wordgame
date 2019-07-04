#!/bin/sh

CC=clang  # if you change this, you might have to change every command
DEBUG_CFLAGS='-g -O0 -std=c99 -Isrc/common/ -Isrc/dicts/'
CFLAGS='-O2 -std=c99 -Isrc/common/ -Isrc/dicts/'

make_dicts() {
	./tools/wl2array.py resources/en-common.wl common6 6 > src/dicts/common6.c
	#./tools/wl2array.py resources/en-common.wl common > src/dicts/common.c
	#./tools/wl2array.py resources/urbandictionary.wl urban6 6 > src/dicts/urban6.c
	#./tools/wl2array.py resources/urbandictionary.wl urban > src/dicts/urban.c
}

compile_common() {
	${CC} ${CFLAGS} -o build/trie.o -c src/common/trie.c
	${CC} ${CFLAGS} -o build/words.o -c src/common/words.c
	${CC} ${CFLAGS} -o build/game.o -c src/common/game.c
}

compile_dicts() {
	#${CC} ${CFLAGS} -o build/common.o -c src/dicts/common.c
	${CC} ${CFLAGS} -o build/common6.o -c src/dicts/common6.c
	#${CC} ${CFLAGS} -o build/urban.o -c src/dicts/urban.c
	#${CC} ${CFLAGS} -o build/urban6.o -c src/dicts/urban6.c
}

compile_termwords() {
	${CC} ${CFLAGS} -o build/main.o -c src/termword/main.c
	${CC} -o build/termwords build/trie.o build/words.o build/game.o build/main.o build/common6.o
}

mkdir build 2> /dev/null

case $1 in
	'debug')
		CFLAGS=${DEBUG_CFLAGS}
		compile_dicts
		compile_common
		compile_termwords
		;;
	'release')
		compile_dicts
		compile_common
		compile_termwords
		;;
	'dicts')
		make_dicts
		exit 0
		;;
	'clean')
		rm -rf build/
		rm -f src/dicts/*.c
		exit 0
		;;
	*) # Fall through
		echo "Usage: ${0} [release|debug|clean|dicts]"
		exit 0
		;;
esac
