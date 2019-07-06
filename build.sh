#!/usr/bin/env bash

################################################################################
### REQUIREMENTS TO USE THIS BUILD SYSTEM
###
### Shells: bash
### Commands:
###     seq
###     [ (test)
###     echo
###     stat

where_am_i() {
	[ -e "C:\\" ] && echo "WINDOWS"
	[ `uname` = "Linux" ] && echo "LINUX"
}

# is_changed [object file] [source file]
is_changed() {
	[ -e ${1} ] && f1=`stat --format=%Y ${1}` || return 0
	[ -e ${2} ] && f2=`stat --format=%Y ${2}` || return 1
	[ ${f1} -lt ${f2} ]
}

# compile_all [compile command]
# compile command interface: command [object file] [source file]
compile_all() {
	if [ $((${#BUILD[@]} % 2)) -ne 0 ]; then
		echo "BUILD SCRIPT ERROR: Your BUILD variable doesn't have "
		echo "                    an even number of entries. Every "
		echo "                    source file should have an object "
		echo "                    target following it."
		exit 1
	fi
	
	for i in `seq 0 2 $((${#BUILD[@]} - 1))`; do
		# Source files are even indexes.
		# Output files are the following odd index.
		src=${BUILD[$i]}
		obj=${BUILD[$((i + 1))]}
		if is_changed ${obj} ${src}; then
			echo "Compiling ${src} -> ${obj}"
			eval ${1} ${obj} ${src}
		fi
	done
}

################################################################################

# compile [output file] [source file]
compile_c_release() {
	clang -O2 -std=c99 -Isrc/common/ -Isrc/dicts/ -o ${1} -c ${2}
}

# compile [output file] [source file]
compile_c_debug() {
	clang -g -O0 -std=c99 -Isrc/common/ -Isrc/dicts/ -o ${1} -c ${2}
}

make_dicts() {
	./tools/wl2array.py resources/en-common.wl common6 6 > src/dicts/common6.c
	#./tools/wl2array.py resources/en-common.wl common > src/dicts/common.c
	#./tools/wl2array.py resources/urbandictionary.wl urban6 6 > src/dicts/urban6.c
	#./tools/wl2array.py resources/urbandictionary.wl urban > src/dicts/urban.c
}

# build [compile command]
build() {
	LINUX_BUILD=(
		src/common/trie.c build/trie.o
		src/common/words.c build/words.o
		src/common/game.c build/game.o
		src/dicts/common6.c build/common6.o
		src/termword/main.c build/main.o
	)

	WIN_BUILD=(
		src/common/trie.c build/trie.o
		src/common/words.c build/words.o
		src/common/game.c build/game.o
		src/dicts/common6.c build/common6.o
		src/win/mm.c build/mm.o
		src/win/window_action.c build/window_action.o
		src/win/window_input.c build/window_input.o
	)

	cc=${1}

	case `where_am_i` in
		'LINUX')
			BUILD=(${LINUX_BUILD[@]})
			link='clang -o build/termwords build/trie.o build/words.o build/game.o build/main.o build/common6.o'
			;;
		'WINDOWS')
			BUILD=(${WIN_BUILD[@]})
			link='clang -o build/termwords -l user32.lib -l gdi32.lib build/trie.o build/words.o build/game.o build/main.o build/common6.o'
			;;
	esac
	
	[ -e build ] || (mkdir build 2> /dev/null)
	compile_all ${cc}
	eval ${link}
}

case $1 in
	'debug')
		build compile_c_debug
		;;
	'release')
		;;
	'dicts')
		make_dicts
		;;
	'clean')
		rm -rf build/
		;;
	'dist-clean')
		rm -rf build/
		rm -f src/dicts/*.c
		;;
	*)
		echo "Usage: ${0} [release|debug|clean|dist-clean|dicts]"
		;;
esac
