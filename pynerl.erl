-module(pynerl).
-export([init/0, pyeval/1]).

-on_load(init/0).

init() ->
      erlang:load_nif("./pynerl", 0).

pyeval(_Str) ->
      "NIF library not loaded".

