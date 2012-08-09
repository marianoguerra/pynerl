#!/usr/bin/env sh

gcc -fPIC -shared -o pynerl.so pynerl.c -I/usr/lib/erlang/erts-5.8.5/include/ -I/usr/include/python3.2/ -D_THREAD_SAFE -lpthread -ldl  -lutil -lpython3.2mu -lm
erlc pynerl.erl
