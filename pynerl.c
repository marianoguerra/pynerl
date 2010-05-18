/* pynerl.c */
#include <stdio.h>
#include "erl_nif.h"
#include <Python.h>

#define STR_SIZE 1024
char code[STR_SIZE];

static ERL_NIF_TERM pyeval(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  Py_Initialize();

  if (enif_get_string(env, argv[0], code, STR_SIZE, ERL_NIF_LATIN1) < 0) {
	  return enif_make_string(env, "code too long", ERL_NIF_LATIN1);
  }
  else {
	  PyRun_SimpleString(code);
  }

  Py_Finalize();

  return argv[0];
}
static ErlNifFunc nif_funcs[] =
{
    {"pyeval", 1, pyeval}
};

ERL_NIF_INIT(pynerl,nif_funcs,NULL,NULL,NULL,NULL)
