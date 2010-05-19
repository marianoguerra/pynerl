-module(pynerl).
-export([init/0, eval/2, call/3]).

-on_load(init/0).

init() ->
      erlang:load_nif("./pynerl", 0).

eval(_Str, _Var) ->
      "NIF library not loaded".

call(_Module, _Function, _Args) ->
      "NIF library not loaded".

