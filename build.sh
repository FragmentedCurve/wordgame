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
###     uname
###
### THIS BUILD SYSTEM PROVIDES
###
###   CMD/FUNC     ARGS
###   --------------------------------------------------------------------------
###   where_am_i
###   is_changed   [object file] [source file]
###   compile_all  [compile command]

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

# compile_all [compile command] < [build file]
# compile command interface: command [object file] [source file]
compile_all() {
	while read BUILD; do
		if [ -z "${BUILD}" ]; then
			continue
		fi
		
		BUILD=(${BUILD})
		obj="${BUILD[0]}"
		src="${BUILD[1]}"
		if is_changed ${obj} ${src}; then
			echo "Compiling ${src} -> ${obj}"
			eval ${1} ${obj} ${src} || exit 1
		else
			for i in `seq 2 $((${#BUILD[@]} - 1))`; do
				if is_changed ${obj} ${BUILD[$i]}; then
					echo "Compiling ${src} -> ${obj}"
					eval ${1} ${obj} ${src} || exit 1
				fi
			done
		fi
	done
}

################################################################################

# compile [output file] [source file]
compile_c_debug() {
	clang -Wall -Werror -g -O0 -std=c99 -Isrc/common/ -Isrc/dicts/ -o ${1} -c ${2}
}

# compile [output file] [source file]
compile_c_release() {
	clang -Wall -Werror -O2 -std=c99 -Isrc/common/ -Isrc/dicts/ -o ${1} -c ${2}
}

make_dicts() {
	./tools/wl2array.py resources/en-common.wl common6 6 > src/dicts/common6.c
	#./tools/wl2array.py resources/en-common.wl common > src/dicts/common.c
	#./tools/wl2array.py resources/urbandictionary.wl urban6 6 > src/dicts/urban6.c
	#./tools/wl2array.py resources/urbandictionary.wl urban > src/dicts/urban.c
}

build_linux() {
	cc=${1}
	
	compile_all ${cc} < build_linux.txt
	clang -o build/termwords build/trie.o build/words.o build/game.o build/main.o build/common6.o
}

build_win() {
	cc=${1}
	
	compile_all ${cc} < build_win.txt
	clang -o build/mm.exe -l user32.lib -l gdi32.lib build/trie.o build/words.o build/game.o build/common6.o build/mm.o build/window_action.o build/window_input.o build/window_playboard.o
	
}

# build [compile command]
build() {
	cc=${1}
	
	[ -e build ] || (mkdir build 2> /dev/null)
	
	case `where_am_i` in
		'LINUX')
			build_linux ${cc}
		;;
		'WINDOWS')
			build_win ${cc}
		;;
	esac
}

case $1 in
	'debug')
		build compile_c_debug
		;;
	'release')
		build  compile_c_release
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
