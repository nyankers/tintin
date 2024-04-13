#include "tintin.h"

#define DO_LUACMD(luacmd) struct session *luacmd(struct session *ses, char *arg1)

DO_LUACMD(luacmd_check);
DO_LUACMD(luacmd_exec);
DO_LUACMD(luacmd_read);
DO_LUACMD(luacmd_restart);

typedef DO_LUACMD(LUACMD);

enum luacmd_arg
{
	LUACMD_NO_ARGS,
	LUACMD_VERBATIM,
	LUACMD_SUBSTITUTE,
};

struct luacmd_type
{
	char                  * name;
	LUACMD                * fun;
	enum luacmd_arg         args;
	char                  * desc;
};

struct luacmd_type luacmd_table[] =
{
	{    "CHECK",             luacmd_check,        LUACMD_SUBSTITUTE, "Checks if lua is enabled."                       },
	{    "EXEC",              luacmd_exec,         LUACMD_VERBATIM,   "Executes a block of lua code."                   },
	{    "READ",              luacmd_read,         LUACMD_SUBSTITUTE, "Reads a lua script file."                        },
	{    "RESTART",           luacmd_restart,      LUACMD_NO_ARGS,    "Restarts the session's lua environment."         },
	{    "",                  NULL,                0,    		  NULL                                              }
};

#ifdef HAVE_LUA
int push_lua_data(lua_State *L, struct listnode *node);

extern void tt_api_init(lua_State *L);
extern void map_api_init(lua_State *L);
#endif

DO_COMMAND(do_lua)
{
	int cnt;

	arg = sub_arg_in_braces(ses, arg, arg1, GET_ONE, SUB_VAR|SUB_FUN);

	if (*arg1 == 0)
	{
		info:

		tintin_header(ses, 80, " LUA COMMANDS ");

		for (cnt = 0 ; *luacmd_table[cnt].fun != NULL ; cnt++)
		{
			if (*luacmd_table[cnt].desc)
			{
				tintin_printf2(ses, "  [%-13s] %s", luacmd_table[cnt].name, luacmd_table[cnt].desc);
			}
		}
		tintin_header(ses, 80, "");

		return ses;
	}
	else
	{
		for (cnt = 0 ; *luacmd_table[cnt].name ; cnt++)
		{
			if (is_abbrev(arg1, luacmd_table[cnt].name))
			{
				break;
			}
		}

		if (*luacmd_table[cnt].name == 0)
		{
			goto info;
		}
		else
		{
			switch (luacmd_table[cnt].args)
			{
				case LUACMD_NO_ARGS:
					break;

				case LUACMD_SUBSTITUTE:
					arg = sub_arg_in_braces(ses, arg, arg2, GET_ALL, SUB_VAR|SUB_FUN);
					break;

				case LUACMD_VERBATIM:
					arg = get_arg_in_braces(ses, arg, arg2, GET_ALL);
					break;
			}

			return luacmd_table[cnt].fun(ses, arg2);
		}
	}
}

DO_LUACMD(luacmd_check)
{
	if (*arg1 == 0)
	{
#ifdef HAVE_LUA
		show_message(ses, LIST_COMMAND, "#LUA IS ENABLED.");
#else
		show_message(ses, LIST_COMMAND, "#LUA ISN'T ENABLED.");
#endif
	}
	else
	{
#ifdef HAVE_LUA
		set_nest_node_ses(ses, arg1, "%s", "1");
#else
		set_nest_node_ses(ses, arg1, "%s", "0");
#endif
	}

	return ses;
}

DO_LUACMD(luacmd_exec)
{
#ifndef HAVE_LUA
	show_error(ses, LIST_COMMAND, "#LUA ISN'T ENABLED.");
#else
	struct session *prev;
	const char *error;

	prev = gtd->lua_ses;
	gtd->lua_ses = ses;

	if (ses->lua == NULL)
	{
		show_error(ses, LIST_COMMAND, "#LUA ISN'T INITIALIZED");
	}
	else if (*arg1 == 0)
	{
		show_error(ses, LIST_COMMAND, "#SYNTAX: #LUA {EXEC} {<SCRIPT>}");
	}
	else if (luaL_dostring(ses->lua, arg1))
	{
		error = lua_tolstring(ses->lua, -1, NULL);

		show_error(ses, LIST_COMMAND, "#LUA ERROR: %s", error);

		lua_pop(ses->lua, 1);
	}

	gtd->lua_ses = prev;
#endif
	return ses;
}

DO_LUACMD(luacmd_read)
{
#ifndef HAVE_LUA
	show_error(ses, LIST_COMMAND, "#LUA ISN'T ENABLED.");
#else
	struct session *prev;
	const char *error;

	prev = gtd->lua_ses;
	gtd->lua_ses = ses;

	if (ses->lua == NULL)
	{
		show_error(ses, LIST_COMMAND, "#LUA ISN'T INITIALIZED");
	}
	else if (*arg1 == 0)
	{
		show_error(ses, LIST_COMMAND, "#SYNTAX: #LUA {READ} {<FILE>}");
	}
	else if (luaL_dofile(ses->lua, arg1))
	{
		error = lua_tolstring(ses->lua, -1, NULL);

		show_error(ses, LIST_COMMAND, "#LUA ERROR: %s", error);

		lua_pop(ses->lua, 1);
	}

	gtd->lua_ses = prev;
#endif
	return ses;
}

DO_LUACMD(luacmd_restart)
{
#ifndef HAVE_LUA
	show_error(ses, LIST_COMMAND, "#LUA ISN'T ENABLED.");
#else
	close_lua_session(ses);

	setup_lua_session(ses);
#endif
	return ses;
}

#ifdef HAVE_LUA

int call_lua(struct session *ses, int nargs, int nresults)
{
	struct session *prev;
	int result;

	prev = gtd->lua_ses;
	gtd->lua_ses = ses;

	result = lua_pcall(ses->lua, nargs, nresults, 0);

	gtd->lua_ses = prev;
	
	return result;
}

int call_lua_function(struct session *ses, struct listnode *node, char **args, int argc)
{
	const char *error;
	int i, result;

	push_call("call_lua_function(%p,%p,%p,%d)", ses, node, args, argc);

	push_lua_data(ses->lua, node);

	for (i = 0; i < argc; i++)
	{
		lua_pushstring(ses->lua, args[i]);
	}

	if (call_lua(ses, argc, 1))
	{
		error = lua_tolstring(ses->lua, -1, NULL);

		show_error(ses, LIST_COMMAND, "#LUA ERROR: %s", error);

		lua_pop(ses->lua, 1);

		pop_call();
		return TRUE;
	}

	result = !lua_isboolean(ses->lua, -1) || lua_toboolean(ses->lua, -1);

	lua_pop(ses->lua, 1);

	pop_call();
	return result;
}

void call_lua_substitute(struct session *ses, struct listnode *node, char *output, char **args, int argc)
{
	const char *error;
	int i;
	const char *result;

	push_call("call_lua_substitute(%p,%p,%p,%p,%d)", ses, node, output, args, argc);

	push_lua_data(ses->lua, node);

	for (i = 0; i < argc; i++)
	{
		lua_pushstring(ses->lua, args[i]);
	}

	if (call_lua(ses, argc, 1))
	{
		error = lua_tolstring(ses->lua, -1, NULL);

		show_error(ses, LIST_COMMAND, "#LUA ERROR: %s", error);

		lua_pop(ses->lua, 1);

		*output = 0;

		pop_call();
		return;
	}

	result = lua_tostring(ses->lua, -1);

	if (result != NULL)
	{
		str_cpy(&output, result);
	}
	else
	{
		*output = 0;
	}

	lua_pop(ses->lua, 1);

	pop_call();
	return;
}

int copy_lua_reference(int ref)
{
	int copy;

	lua_rawgeti(gtd->lua, LUA_REGISTRYINDEX, gtd->lua_nodes);
	lua_rawgeti(gtd->lua, -1, ref);

	copy = luaL_ref(gtd->lua, -2);

	lua_pop(gtd->lua, 1);

	return copy;
}

void init_lua()
{
	lua_State *L;

	L = gtd->lua = luaL_newstate();

	lua_gc(L, LUA_GCSTOP, 0);

	luaL_openlibs(L);

	lua_newtable(L);
	gtd->lua_nodes = luaL_ref(L, LUA_REGISTRYINDEX);

	tt_api_init(L);
	map_api_init(L);

	lua_gc(L, LUA_GCRESTART, -1);
}

void setup_lua_session(struct session *ses)
{
	// create a copy of the original _G
	
	ses->lua = lua_newthread(gtd->lua);                       /* t */
	
	lua_newtable(gtd->lua);                                   /* th, t */

	lua_pushnil(gtd->lua);                                    /* th, t, nil */
	while (lua_next(gtd->lua, LUA_GLOBALSINDEX))              /* th, t, k, v */
	{
		lua_pushvalue(gtd->lua, -2);                      /* th, t, k, v, k */
		lua_pushvalue(gtd->lua, -2);                      /* th, t, k, v, k, v */
		lua_rawset(gtd->lua, -5);                         /* th, t, k, v */
		lua_pop(gtd->lua, 1);                             /* th, t, k */
	}                                                         /* th, t */

	lua_setfenv(gtd->lua, -2);                                /* th */

	ses->lua_ref = luaL_ref(gtd->lua, LUA_REGISTRYINDEX);     /* */
}

void close_lua_session(struct session *ses)
{
	ses->lua = NULL;

	luaL_unref(gtd->lua, LUA_REGISTRYINDEX, ses->lua_ref);

	ses->lua_ref = LUA_NOREF;
}

int clear_lua_data(struct listnode *node)
{
	lua_rawgeti(gtd->lua, LUA_REGISTRYINDEX, gtd->lua_nodes);
	luaL_unref(gtd->lua, -1, node->lua_ref);

	node->lua_ref = LUA_NOREF;

	lua_pop(gtd->lua, 1);

	return 1;
}

char *opt_luastring(lua_State *L, int n)
{
	const char *str;
	char *arg;
	size_t size;

	str = lua_tolstring(L, n, &size);
	arg = str_alloc_stack(size);

	if (str)
	{
		str_cpy(&arg, str);
	}

	return arg;
}

char *get_luastring(lua_State *L, int n)
{
	const char *str;
	char *arg;

	str = luaL_checkstring(L, n);
	arg = str_alloc_stack(0);
	str_cpy(&arg, str);

	return arg;
}

#endif
