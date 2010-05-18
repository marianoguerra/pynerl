#!/usr/bin/env sh

gcc -fPIC -shared -o pynerl.so pynerl.c -I $ERL_INCLUDE -I/usr/include/python3.1/ -D_THREAD_SAFE -lpthread -ldl  -lutil -lpython3.1 -lm
$ERLC pynerl.erl
