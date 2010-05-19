/* pynerl.c */
#include <stdio.h>
#include <string.h>
#include "erl_nif.h"
#include <Python.h>

#define BUFF_SIZE 1024

static ERL_NIF_TERM pynerl_make_error(ErlNifEnv* env, const char* type, const char* msg) {
	return enif_make_tuple3(env, enif_make_atom(env, "error"), enif_make_atom(env, type), enif_make_string(env, msg, ERL_NIF_LATIN1));
}

// convert an erlang term to a python object
// return None if the type can't be converted
static PyObject* pynerl_term_to_obj(ErlNifEnv* env, ERL_NIF_TERM term) {
	long int vlong;
	double vdouble;
	char buff[BUFF_SIZE];
	PyObject* obj;
	ERL_NIF_TERM list, head, tail;

	// TODO: add more types
	if (enif_get_long(env, term, &vlong)) {
		obj = PyLong_FromLong(vlong);
	}
	else if (enif_get_double(env, term, &vdouble)) {
		obj = PyFloat_FromDouble(vlong);
	}
	else if (enif_get_string(env, term, buff, BUFF_SIZE, ERL_NIF_LATIN1)) {
		obj = PyUnicode_FromString(buff);
	}
	else if (enif_get_list_cell(env, term, &head, &tail)) {
		obj = PyList_New(0);
		list = term;

		while (enif_get_list_cell(env, list, &head, &tail)) {
			PyList_Append(obj, pynerl_term_to_obj(env, head));
			list = tail;
		}
	}
	else {
		obj = Py_None;
	}

	return obj;
}

// convert a python object to an erlang term, return the atom 'unknown' if
// the type can't be converted
static ERL_NIF_TERM pynerl_obj_to_term(ErlNifEnv* env, PyObject* obj) {
	ERL_NIF_TERM term;

	if (obj == Py_False) {
		term = enif_make_atom(env, "false");
	}
	else if (obj == Py_True) {
		term = enif_make_atom(env, "true");
	}
	else if (PyLong_Check(obj)) {
		term = enif_make_long(env, PyLong_AsLong(obj));
	}
	else if (PyFloat_Check(obj)) {
		term = enif_make_double(env, PyFloat_AsDouble(obj));
	}
	else if (PyBytes_Check(obj)) {
		// XXX: the encoding must be latin1
		term = enif_make_string(env, PyBytes_AsString(obj), ERL_NIF_LATIN1);
	}
	else if (PyUnicode_Check(obj)) {
		// XXX: the encoding must be latin1
		term = enif_make_string(env, PyUnicode_AS_DATA(obj), ERL_NIF_LATIN1);
	}
	else if (obj == Py_None) {
		term = enif_make_atom(env, "none");
    }
	else {
		term = enif_make_atom(env, "unknown");
    }

	return term;
}

// eval the first parameter, return the value of the variable passed as second parameter
// for example you eval "x = 2" and want the value of x, you do pynerl:eval("x = 2", "x")
static ERL_NIF_TERM pynerl_eval(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
	Py_Initialize();
	char buff[BUFF_SIZE];

	PyObject *pResult=NULL, *pdict, *pval;
	ERL_NIF_TERM eResult;

	// TODO: error checking
	enif_get_string(env, argv[0], buff, BUFF_SIZE, ERL_NIF_LATIN1);

	pdict = PyDict_New();
    PyDict_SetItemString(pdict, "__builtins__", PyEval_GetBuiltins());

    pResult = PyRun_String(buff, Py_file_input, pdict, pdict);

	if (pResult == NULL) {
		eResult = pynerl_make_error(env, "exception", "Exception while running eval");
		PyErr_Print();
	}
	else {
		enif_get_string(env, argv[1], buff, BUFF_SIZE, ERL_NIF_LATIN1);
		pval = PyDict_GetItemString(pdict, buff);
		eResult = pynerl_obj_to_term(env, pval);
		Py_DECREF(pResult);
	}

	Py_DECREF(pdict);

	Py_Finalize();

	return eResult;
}

static ERL_NIF_TERM pynerl_call(ErlNifEnv* env, int arc, const ERL_NIF_TERM argv[]) {
	Py_Initialize();

	int size;
	char buff[BUFF_SIZE];
	PyObject *pModName, *pModule, *pFunc, *pArgs, *pArgList, *pValue;
	ERL_NIF_TERM eResult;

	pModName = pynerl_term_to_obj(env, argv[0]);

    pModule = PyImport_Import(pModName);
    Py_DECREF(pModName);

    if (pModule != NULL) {
		// TODO: error checking
		enif_get_string(env, argv[1], buff, BUFF_SIZE, ERL_NIF_LATIN1);
		pFunc = PyObject_GetAttrString(pModule, buff);

        if (pFunc && PyCallable_Check(pFunc)) {
			pArgList = pynerl_term_to_obj(env, argv[2]);

            pArgs = PyList_AsTuple(pArgList);
			Py_DECREF(pArgList);
			pValue = PyObject_CallObject(pFunc, pArgs);
			Py_DECREF(pArgs);

			if (pValue != NULL) {
				eResult = pynerl_obj_to_term(env, pValue);
				Py_DECREF(pValue);
			}
			else {
				eResult = pynerl_make_error(env, "exception", "Exception while calling function");
			}

			Py_DECREF(pFunc);
		}
		else {
			eResult = pynerl_make_error(env, "nofun", "No function in module");
		}

		Py_DECREF(pModule);
    }
	else {
		eResult = pynerl_make_error(env, "nomodule", "Module doesn't exist");
    }

	Py_Finalize();

	return eResult;
}

static ErlNifFunc nif_funcs[] = {
    {"eval", 2, pynerl_eval},
    {"call", 3, pynerl_call}
};

ERL_NIF_INIT(pynerl,nif_funcs,NULL,NULL,NULL,NULL)
