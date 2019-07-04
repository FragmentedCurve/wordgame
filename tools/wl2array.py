#!/usr/bin/env python3

import sys

def filter(words, word_len=None):
    result = [w.upper() for w in words]
    
    while result.count('') > 0:
        result.remove('')

    r = []
    for w in result:
        if w.isalpha():
            if word_len != None:
                if len(w) <= word_len:
                    r.append(w)
            else:
                r.append(w)
    result = r
    return result

def main():
    argc = len(sys.argv)
    if argc < 3:
        print('Usage: {0} [wordlist] [variable name] [[filter length]]'.format(sys.argv[0]))
        return

    wordlist_file = sys.argv[1]
    var_name = sys.argv[2]
    word_len = None
    
    if argc >= 4:
        word_len = int(sys.argv[3])
        
    with open(wordlist_file, 'r') as f:
        words = f.read().split('\n')

    words = filter(words, word_len)

    word_count = 0
    print('const char* const wl_{0}[] = {{'.format(var_name))
    for i in range(0, len(words), 4):
        print('\t', end='')
        l = ['"{0}"'.format(w) for w in words[i:i+4]]
        word_count += len(l)
        print(', '.join(l), end=',\n')
    print('};')

    print("unsigned int wl_{0}_len = {1};".format(var_name, word_count))
    
if __name__ == '__main__':
    main()
