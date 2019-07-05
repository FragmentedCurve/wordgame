#!/bin/sh
CC=clang  # if you change this, you might have to change every command
DEBUG_CFLAGS='-g -O0 -std=c99 -Isrc/common/ -Isrc/dicts/'
CFLAGS='-O2 -std=c99 -Isrc/common/ -Isrc/dicts/'


# is_changed [object file] [source file]
is_changed() {
	[ -e ${1} ] && f1=`stat --format=%Y ${1}` || return 0
	[ -e ${2} ] && f2=`stat --format=%Y ${2}` || return 1
	[ ${f1} -lt ${f2} ]
}

# compile_all [compile command]
# compile command interface: command [object file] [source file]
# Builds files are in the env var $BUILD.
# Source files are listed in the nev var $SRC.
# Their indexes are correlated.
compile_all() {
	[ 0 -gt ${#BUILD[@]} ] && return 1
	[ 0 -gt ${#SRC[@]} ] && return 1
	[ ${#BUILD[@]} -eq ${#SRC[@]} ] || return 1
	
	for i in `seq 0 $((${#BUILD[@]} - 1))`; do
		${1} ${BUILD[$i]} ${SRC[$i]}
	done
}

# compile [output file] [source file]
compile_c() {
	is_changed ${1} ${2} && ${CC} ${CFLAGS} -o ${1} -c ${2}
}

make_dicts() {
	./tools/wl2array.py resources/en-common.wl common6 6 > src/dicts/common6.c
	#./tools/wl2array.py resources/en-common.wl common > src/dicts/common.c
	#./tools/wl2array.py resources/urbandictionary.wl urban6 6 > src/dicts/urban6.c
	#./tools/wl2array.py resources/urbandictionary.wl urban > src/dicts/urban.c
}

build() {
	BUILD=(
		build/trie.o
		build/words.o
		build/game.o
		build/common6.o
		build/main.o
	)
	SRC=(
		src/common/trie.c
		src/common/words.c
		src/common/game.c
		src/dicts/common6.c
		src/termword/main.c
	)
	
	compile_all compile_c
	${CC} -o build/termwords build/trie.o build/words.o build/game.o build/main.o build/common6.o
}

case $1 in
	'debug')
		CFLAGS=${DEBUG_CFLAGS}
		[ -e build ] || (mkdir build 2> /dev/null)
		build
		;;
	'release')
		[ -e build ] || (mkdir build 2> /dev/null)
		build
		;;
	'dicts')
		make_dicts
		;;
	'clean')
		rm -rf build/
		rm -f src/dicts/*.c
		;;
	*)
		echo "Usage: ${0} [release|debug|clean|dicts]"
		;;
esac
