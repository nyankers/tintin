#include "tintin.h"

#ifdef HAVE_LUA

// specific functions

DO_LUA(tt_action);
DO_LUA(tt_alias);
DO_LUA(tt_background);
DO_LUA(tt_button);
DO_LUA(tt_class);
DO_LUA(tt_color);
DO_LUA(tt_convert);
DO_LUA(tt_debug);
DO_LUA(tt_delay);
DO_LUA(tt_echo);
DO_LUA(tt_event);
DO_LUA(tt_exec);
DO_LUA(tt_fun);
DO_LUA(tt_get);
DO_LUA(tt_get_input);
DO_LUA(tt_get_gag);
DO_LUA(tt_history);
DO_LUA(tt_ignore);
DO_LUA(tt_macro);
DO_LUA(tt_parse);
DO_LUA(tt_parse_list);
DO_LUA(tt_procedure);
DO_LUA(tt_prompt);
DO_LUA(tt_quiet);
DO_LUA(tt_read_all);
DO_LUA(tt_session);
DO_LUA(tt_session_env);
DO_LUA(tt_set);
DO_LUA(tt_show);
DO_LUA(tt_sub);
DO_LUA(tt_substitute);
DO_LUA(tt_tick);
DO_LUA(tt_var);

// closures

DO_LUA(read_lua_nodes);

const luaL_Reg tt_api_reg[] = {
	{ "action", tt_action },
	{ "alias", tt_alias },
	{ "background", tt_background },
	{ "button", tt_button },
	{ "class", tt_class },
	{ "color", tt_color },
	{ "convert", tt_convert },
	{ "debug", tt_debug },
	{ "delay", tt_delay },
	{ "echo", tt_echo },
	{ "event", tt_event },
	{ "exec", tt_exec },
	{ "fun", tt_fun },
	{ "get", tt_get },
	{ "get_input", tt_get_input },
	{ "get_gag", tt_get_gag },
	{ "history", tt_history },
	{ "ignore", tt_ignore },
	{ "macro", tt_macro },
	{ "parse", tt_parse },
	{ "parse_list", tt_parse_list },
	{ "procedure", tt_procedure },
	{ "prompt", tt_prompt },
	{ "quiet", tt_quiet },
	{ "read_all", tt_read_all },
	{ "session", tt_session },
	{ "session_env", tt_session_env },
	{ "set", tt_set },
	{ "show", tt_show },
	{ "sub", tt_sub },
	{ "substitute", tt_substitute },
	{ "tick", tt_tick },
	{ "var", tt_var },
	{ NULL, NULL }
};

char *list_names_lua[LIST_MAX];

int substitute_flag_mask = 0;

int count_class(struct session *ses, struct listnode *group);

void tt_api_init(lua_State *L)
{
	int i;

	luaL_register(L, "tt", tt_api_reg);

	lua_pushliteral(L, CLIENT_NAME);
	lua_setfield(L, -2, "NAME");

	lua_pushliteral(L, CLIENT_VERSION);
	lua_setfield(L, -2, "VERSION");

	for (i = 0 ; *substitution_table[i].name ; i++)
	{
		SET_BIT(substitute_flag_mask, substitution_table[i].bitvector);
		
		lua_pushinteger(L, substitution_table[i].bitvector);
		lua_setfield(L, -2, substitution_table[i].name);
	}

	for (i = 0 ; i < LIST_MAX ; i++)
	{
		/* lua convention is lower-case functions */
		list_names_lua[i] = strdup(list_table[i].name_multi);
		lowerstring(list_names_lua[i]);

		lua_pushstring(L, list_names_lua[i]);
		lua_pushinteger(L, i);
		lua_pushcclosure(L, read_lua_nodes, 1);
		lua_rawset(L, -3);
	}

	lua_pop(L, 1);
}

int push_lua_data(lua_State *L, struct listnode *node)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, gtd->lua_nodes);

	lua_rawgeti(L, -1, node->lua_ref);

	lua_replace(L, -2);

	return 1;
}

int set_lua_data(lua_State *L, struct listnode *node)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, gtd->lua_nodes);

	lua_pushvalue(L, -2);

	if (node->lua_ref == LUA_NOREF)
	{
		node->lua_ref = luaL_ref(L, -2);
	}
	else
	{
		lua_rawseti(L, -2, node->lua_ref);
	}

	lua_pop(L, 2);

	return 1;
}

DO_LUA(tt_show)
{
	char *arg1, *arg2, *arg3;
	char *out, *tmp;
	int prompt;

	out = str_alloc_stack(0);
	tmp = str_alloc_stack(0);

	arg1 = get_luastring(L, 1);
	substitute(gtd->lua_ses, arg1, tmp, SUB_COL);

	prompt = is_suffix(arg1, "\\") && !is_suffix(arg1, "\\\\");

	arg2 = opt_luastring(L, 2);
	arg3 = opt_luastring(L, 3);

	if (strchr(arg1, '\n'))
	{
		strip_vt102_codes(tmp, arg1);

		check_one_line_multi(gtd->lua_ses, tmp, arg1);
	}
	else
	{
		check_one_line(gtd->lua_ses, tmp);
	}

	if (gtd->lua_ses->gagline > 0)
	{
		gtd->lua_ses->gagline--;

		show_debug(gtd->lua_ses, LIST_GAG, COLOR_DEBUG "#DEBUG GAG " COLOR_BRACE "{" COLOR_STRING "%s" COLOR_BRACE "} " COLOR_COMMAND "[" COLOR_STRING "%d" COLOR_COMMAND "]", tmp, gtd->lua_ses->gagline + 1);

		return 0;
	}

	if (*arg2)
	{
		split_show(gtd->lua_ses, tmp, arg2, arg3);

		return 0;
	}

	str_cpy_printf(&out, "%s%s%s", COLOR_TEXT, tmp, COLOR_TEXT);

	tintin_puts3(gtd->lua_ses, out, prompt);

	return 0;
}

DO_LUA(tt_echo)
{
	char *arg1, *arg2, *arg3;
	char *out, *tmp;
	int prompt;

	out = str_alloc_stack(0);
	tmp = str_alloc_stack(0);

	arg1 = get_luastring(L, 1);
	substitute(gtd->lua_ses, arg1, tmp, SUB_COL);

	prompt = is_suffix(arg1, "\\") && !is_suffix(arg1, "\\\\");

	arg2 = opt_luastring(L, 2);
	arg3 = opt_luastring(L, 3);

	if (*arg2)
	{
		split_show(gtd->lua_ses, tmp, arg2, arg3);

		return 0;
	}

	str_cpy_printf(&out, "%s%s%s", COLOR_TEXT, tmp, COLOR_TEXT);

	tintin_puts3(gtd->lua_ses, out, prompt);

	return 0;
}

DO_LUA(tt_exec)
{
	char *cmd, *arg, *pto;
	int i, j, argc, size;

	cmd = str_alloc_stack(0);
	arg = str_alloc_stack(0);

	argc = lua_gettop(L);

	strcpy(cmd, luaL_checkstring(L, 1));
	pto = cmd + lua_objlen(L, 1);

	for (i = 2 ; i <= argc ; i++)
	{
		*(pto++) = ' ';
		*(pto++) = '{';

		if (lua_istable(L, i))
		{
			size = lua_objlen(L, i);
			for (j = 1 ; j <= size ; j++)
			{
				*(pto++) = '{';

				lua_rawgeti(L, i, j);
				if (!lua_isstring(L, -1))
				{
					return luaL_argerror(L, i, "expected string or arrays of strings");
				}

				strcpy(arg, lua_tostring(L, -1));
				lua_pop(L, 1);

				pto += substitute(gtd->lua_ses, arg, pto, SUB_SEC);

				*(pto++) = '}';
			}
		}
		else if (lua_isstring(L, i))
		{
			strcpy(arg, lua_tostring(L, i));
			pto += substitute(gtd->lua_ses, arg, pto, SUB_SEC);
		}
		else
		{
			return luaL_argerror(L, i, "expected string or arrays of strings");
		}

		*(pto++) = '}';
	}
	*pto = '\0';

	script_driver(gtd->lua_ses, LIST_COMMAND, cmd);

	return 0;
}

DO_LUA(tt_var)
{
	struct listroot *root;
	struct listnode *node;
	char *arg1, *arg2, name[BUFFER_SIZE], *str;

	root = gtd->lua_ses->list[LIST_VARIABLE];

	arg1 = get_luastring(L, 1);

	if (lua_type(L, 2) == LUA_TBOOLEAN && !lua_toboolean(L, 2))
	{
		lua_pushboolean(L, delete_nest_node(root, arg1));
		return 1;
	}

	arg2 = opt_luastring(L, 2);

	if (!*arg2)
	{
		get_arg_to_brackets(gtd->lua_ses, arg1, name);
		node = search_node_list(root, name);

		if (node)
		{
			str = str_dup("");
			get_nest_node_val(root, arg1, &str, TRUE);
			lua_pushstring(L, str);
			str_free(str);

			return 1;
		}

		lua_pushnil(L);
		return 1;
	}
	else
	{
		set_nest_node(root, arg1, "%s", arg2);
		return 0;
	}
}

int push_lua_string_or_number(lua_State *L, const char *str, int convert)
{
	lua_pushstring(L, str);
	if (convert && lua_isnumber(L, -1))
	{
		lua_pushnumber(L, lua_tonumber(L, -1));
		lua_replace(L, -2);
	}
	return 1;
}

void check_var_path(lua_State *L, int argc)
{
	int len, i;

	if (argc == 1)
	{
		if (lua_istable(L, 1))
		{
			len = lua_objlen(L, 1);

			for (i = 1 ; i <= len ; i++)
			{
				lua_rawgeti(L, 1, i);
				if (!lua_isstring(L, -1))
				{
					luaL_argerror(L, 1, "expected string(s) or array of strings");
				}
				lua_pop(L, 1);
			}
		}
		else if (!lua_isstring(L, 1))
		{
			luaL_argerror(L, 1, "expected string(s) or array of strings");
		}
	}
	else
	{
		for (i = 1 ; i < argc ; i++)
		{
			if (!lua_isstring(L, i))
			{
				luaL_argerror(L, i, "expected string(s) or array of strings");
			}
		}
	}
}

struct listnode *get_var_node(lua_State *L, int argc, int create)
{
	struct listroot *root;
	struct listnode *node = NULL;
	char *name, *arg;
	const char *str;
	int i, size;

	root = gtd->lua_ses->list[LIST_VARIABLE];
	arg = str_alloc_stack(0);
	name = str_alloc_stack(0);

	if (argc == 1)
	{
		if (lua_istable(L, 1))
		{
			size = lua_objlen(L, 1);

			for (i = 1 ; i <= size ; i++)
			{
				lua_rawgeti(L, 1, i);
				str = lua_tostring(L, -1);
				lua_pop(L, 1);

				str_cpy(&name, str);

				if (root == NULL)
				{
					if (create)
					{
						root = node->root = init_list(gtd->lua_ses, LIST_VARIABLE, LIST_SIZE);
						node = NULL;
						break;
					}
					else
					{
						return NULL;
					}
				}

				node = search_node_list(root, name);

				if (node == NULL)
				{
					break;
				}

				root = node->root;
			}

			if (node == NULL && create)
			{
				node = update_node_list(root, name, "", "", "");

				for ( i++ ; i <= size ; i++ )
				{
					root = node->root = init_list(gtd->lua_ses, LIST_VARIABLE, LIST_SIZE);

					lua_pushinteger(L, i);
					lua_gettable(L, 1);

					str = lua_tostring(L, -1);
					lua_pop(L, 1);

					str_cpy(&name, str);

					node = update_node_list(root, name, "", "", "");
				}
			}
		}
		else
		{
			str = lua_tostring(L, 1);
			str_cpy(&arg, str);

			arg = get_arg_to_brackets(gtd->lua_ses, arg, name);

			while (root && *arg)
			{
				node = search_node_list(root, name);

				if (node == NULL)
				{
					break;
				}

				root = node->root;

				arg = get_arg_in_brackets(root->ses, arg, name);
			}

			if (root == NULL)
			{
				if (create)
				{
					root = node->root = init_list(gtd->lua_ses, LIST_VARIABLE, LIST_SIZE);
					node = NULL;
				}
				else
				{
					return NULL;
				}
			}

			if (node == NULL && create)
			{
				node = update_node_list(root, name, "", "", "");

				while (*arg)
				{
					root = node->root = init_list(gtd->lua_ses, LIST_VARIABLE, LIST_SIZE);

					arg = get_arg_in_brackets(root->ses, arg, name);

					node = update_node_list(root, name, "", "", "");
				}
			}
			else
			{
				node = search_nest_node(root, name);
			}
		}
	}
	else
	{
		for (i = 1 ; i <= argc ; i++)
		{
			str = lua_tostring(L, i);
			str_cpy(&name, str);

			if (root == NULL)
			{
				if (create)
				{
					root = node->root = init_list(gtd->lua_ses, LIST_VARIABLE, LIST_SIZE);
					node = NULL;
					break;
				}
				else
				{
					return NULL;
				}
			}

			node = search_node_list(root, name);

			if (node == NULL)
			{
				break;
			}

			root = node->root;
		}

		if (node == NULL && create)
		{
			node = update_node_list(root, name, "", "", "");

			for ( i++ ; i <= argc ; i++ )
			{
				root = node->root = init_list(gtd->lua_ses, LIST_VARIABLE, LIST_SIZE);

				str = lua_tostring(L, i);
				str_cpy(&name, str);

				node = update_node_list(root, name, "", "", "");
			}
		}
	}

	return node;
}

void push_lua_var(lua_State *L, struct listnode *node, int convert_numbers)
{
	struct listroot *root;
	struct listnode *child = NULL;
	int i = 1, depth = 1, prev = 0;

	if (node == NULL)
	{
		lua_pushnil(L);
		return;
	}

	if (node->root == NULL)
	{
		push_lua_string_or_number(L, node->arg2, convert_numbers);
		return;
	}

	lua_newtable(L); /* stack */

	lua_newtable(L); /* result */

	while (TRUE)
	{
		root = node->root;

		for (i = prev ; i < root->used ; i++)
		{
			child = root->list[i];
			push_lua_string_or_number(L, child->arg1, convert_numbers);

			if (child->root == NULL)
			{
				push_lua_string_or_number(L, child->arg2, convert_numbers);
				lua_rawset(L, -3);
			}
			else
			{
				lua_rawseti(L, -3, depth++);
				lua_rawseti(L, -2, depth++);

				lua_pushlightuserdata(L, node);
				lua_rawseti(L, -2, depth++);

				lua_pushinteger(L, i + 1);
				lua_rawseti(L, -2, depth++);

				lua_newtable(L);
				node = child;

				prev = 0;

				break;
			}
		}

		if (node != child)
		{
			if (depth == 1) {
				break;
			}

			lua_rawgeti(L, -2, --depth);
			prev = lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_rawgeti(L, -2, --depth);
			node = lua_touserdata(L, -1);
			lua_pop(L, 1);

			lua_rawgeti(L, -2, --depth);
			lua_rawgeti(L, -3, --depth);
			lua_pushvalue(L, -3);
			lua_rawset(L, -3);

			lua_replace(L, -2);

			child = NULL;
		}
	}

	lua_replace(L, -2);
}

DO_LUA(tt_get)
{
	struct listnode *node;
	int argc, convert_numbers;

	argc = lua_gettop(L);

	if (lua_isboolean(L, argc))
	{
		convert_numbers = lua_toboolean(L, argc--);
	}
	else
	{
		convert_numbers = TRUE;
	}

	check_var_path(L, argc);

	node = get_var_node(L, argc, FALSE);

	push_lua_var(L, node, convert_numbers);

	return 1;
}

DO_LUA(tt_set)
{
	struct listnode *node;
	char *arg1, *arg2;
	int n, depth, max_depth = 0;

	n = lua_gettop(L);

	if (n < 2)
	{
		return luaL_error(L, "expected 2+ arguments");
	}

	check_var_path(L, n - 1);

	if (lua_istable(L, n))
	{

		lua_pushvalue(L, n);

		depth = n + 1;

		lua_pushnil(L);
		lua_pushnil(L);

		while (depth >= n)
		{
			check_table_loop:

			if (depth > max_depth)
			{
				max_depth = depth;
				luaL_checkstack(L, depth + 5, "table depth is too great");
			}

			while (lua_next(L, depth))
			{
				if (!lua_isstring(L, -2))
				{
					return luaL_argerror(L, n, "expected string or simple table");
				}
				else if (lua_istable(L, -1))
				{
					depth += 3;

					lua_pushnil(L);
					lua_pushnil(L);

					goto check_table_loop;
				}
				else if (!lua_isstring(L, -1))
				{
					return luaL_argerror(L, n, "expected string or simple table");
				}

				lua_pop(L, 1);
			}

			depth -= 3;
			lua_pop(L, 2);
		}
	}
	else if (!lua_isstring(L, n))
	{
		return luaL_argerror(L, n, "expected string or simple table");
	}

	node = get_var_node(L, n-1, TRUE);

	if (node->root)
	{
		free_list(node->root);

		node->root = NULL;
	}

	if (lua_isstring(L, n))
	{
		str_cpy(&node->arg2, lua_tostring(L, n));
		return 0;
	}

	node->root = init_list(gtd->lua_ses, LIST_VARIABLE, LIST_SIZE);

	arg1 = str_alloc_stack(0);
	arg2 = str_alloc_stack(0);

	depth = n;

	lua_pushlightuserdata(L, node);
	lua_pushnil(L);

	while (depth >= n)
	{
		node = lua_touserdata(L, depth + 1);

		store_table_loop:

		while (lua_next(L, depth))
		{
			if (lua_isstring(L, -1))
			{
				lua_pushvalue(L, -2);
				str_cpy(&arg1, lua_tostring(L, -1));
				str_cpy(&arg2, lua_tostring(L, -2));
				lua_pop(L, 2);

				update_node_list(node->root, arg1, arg2, "", "");
			}
			else if (lua_istable(L, -1))
			{
				lua_pushvalue(L, -2);
				str_cpy(&arg1, lua_tostring(L, -1));
				lua_pop(L, 1);

				depth += 3;

				node = update_node_list(node->root, arg1, "", "", "");
				node->root = init_list(gtd->lua_ses, LIST_VARIABLE, LIST_SIZE);

				lua_pushlightuserdata(L, node);
				lua_pushnil(L);

				goto store_table_loop;
			}
		}

		depth -= 3;
		lua_pop(L, 2);
	}

	return 0;
}

int set_standard_node_lua(lua_State *L, int list)
{
	struct listroot *root;
	struct listnode *node;
	char *arg1, *arg2, *arg3;
	int shots, n, is_function;

	root = gtd->lua_ses->list[list];

	n = lua_gettop(L);

	arg1 = get_luastring(L, 1);

	if (n > 2)
	{
		arg3 = get_luastring(L, 2);

		if (*arg3 && (atof(arg3) < 1 || atof(arg3) >= 10))
		{
			return luaL_error(L, "priority must be within [1, 10)", arg1, arg3);
		}
	}
	else
	{
		arg3 = "5";
	}

	if (n > 3)
	{
		shots = luaL_checkint(L, 3);
	}
	else
	{
		shots = 0;
	}

	if (n > 4)
	{
		n = 4;
	}


	is_function = lua_isfunction(L, n);

	if (is_function)
	{
		arg2 = "";
	}
	else
	{
		arg2 = get_luastring(L, n);
	}

	node = update_node_list(root, arg1, arg2, arg3, "");

	node->shots = shots;
	
	if (is_function)
	{
		lua_pushvalue(L, n);
		set_lua_data(L, node);
	}

	return 0;
}

DO_LUA(tt_action)
{
	return set_standard_node_lua(L, LIST_ACTION);
}

DO_LUA(tt_alias)
{
	return set_standard_node_lua(L, LIST_ALIAS);
}

DO_LUA(tt_button)
{
	struct listroot *root;
	struct listnode *node;
	const char *str;
	char *arg, *arg1, *arg2, *arg3, *arg4;
	int shots;
	short pos[4];
	int n, m, i, is_function;

	root = gtd->lua_ses->list[LIST_BUTTON];
	n = lua_gettop(L);

	arg1 = str_alloc_stack(0);
	arg2 = str_alloc_stack(0);

	if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4))
	{
		for (i = 0 ; i < 4 ; i++)
		{
			pos[i] = (short) lua_tonumber(L, i + 1);
		}

		sprintf(arg1, "%d;%d;%d;%d", pos[0], pos[1], pos[2], pos[3]);

		m = 4;
	}
	else if (lua_isstring(L, 1))
	{
		str = lua_tostring(L, 1);
		str_cpy(&arg1, str);

		arg = arg1;

		for (i = 0 ; i < 4 ; i++)
		{
			arg = get_arg_in_braces(gtd->lua_ses, arg, arg2, GET_ONE);

			pos[i] = (short) get_number(gtd->lua_ses, arg2);
		}

		m = 1;
	}
	else
	{
		return luaL_error(L, "expected four integers or a string of four numbers");
	}

	for (i = 0 ; i < 4 ; i++)
	{
		if (pos[i] == 0)
		{
			return luaL_error(L, "invalid square coordinate: %s", arg1);
		}
	}

	if ((n - m) > 1)
	{
		arg3 = opt_luastring(L, m + 1);

		if (*arg3 && (atof(arg3) < 1 || atof(arg3) >= 10))
		{
			return luaL_error(L, "priority must be within [1, 10)", arg1, arg3);
		}
	}
	else
	{
		arg3 = "5";
	}

	if ((n - m) > 2)
	{
		arg4 = opt_luastring(L, m + 2);
	}
	else
	{
		arg4 = "";
	}
		
	if (*arg4 == 0)
	{
		arg4 = "PRESSED MOUSE BUTTON ONE";
	}

	if ((n - m) > 3)
	{
		shots = luaL_optint(L, m + 3, 0);
	}
	else
	{
		shots = 0;
	}

	if ((n - m) > 4)
	{
		n = m + 4;
	}

	is_function = lua_isfunction(L, n);

	if (is_function)
	{
		arg2 = "";
	}
	else
	{
		str = luaL_checkstring(L, n);
		str_cpy(&arg2, str);
	}

	SET_BIT(gtd->event_flags, EVENT_FLAG_MOUSE);
	SET_BIT(gtd->lua_ses->event_flags, EVENT_FLAG_MOUSE);

	node = update_node_list(root, arg1, arg2, arg3, arg4);

	for (i = 0 ; i < 4 ; i++)
	{
		node->val16[i] = pos[i];
	}

	node->shots = shots;
	
	if (is_function)
	{
		lua_pushvalue(L, n);
		set_lua_data(L, node);
	}

	return 0;
}

DO_LUA(tt_event)
{
	struct listroot *root;
	struct listnode *node;
	char *arg1, *arg2;
	int n, shots, index, is_function;

	arg1 = get_luastring(L, 1);

	for (index = 0 ; *event_table[index].name != 0 ; index++)
	{
		if (!strncmp(event_table[index].name, arg1, strlen(event_table[index].name)))
		{
			root = gtd->lua_ses->list[LIST_EVENT];

			n = lua_gettop(L);

			if (n > 2)
			{
				shots = luaL_optint(L, 2, 0);
			}
			else
			{
				shots = 0;
			}

			if (n > 3)
			{
				n = 3;
			}

			is_function = lua_isfunction(L, n);

			if (is_function)
			{
				arg2 = "";
			}
			else
			{
				arg2 = get_luastring(L, n);
			}


			SET_BIT(gtd->lua_ses->event_flags, event_table[index].flags);
			SET_BIT(gtd->event_flags, event_table[index].flags);

			node = update_node_list(root, arg1, arg2, "", "");

			node->val32[0] = index;
			node->val32[1] = event_table[index].flags;

			if (node->val32[1] == 0)
			{
				tintin_printf2(gtd->ses, "\e[1;33mdo_event: event_table[index].flags == 0");
			}

			event_table[index].level++;

			node->shots = shots;

			if (is_function)
			{
				lua_pushvalue(L, n);
				set_lua_data(L, node);
			}

			return 0;
		}
	}

	return luaL_error(L, "'%s' is not an existing event", arg1);
}

DO_LUA(tt_sub)
{
	return set_standard_node_lua(L, LIST_SUBSTITUTE);
}

DO_LUA(tt_prompt)
{
	struct listroot *root;
	struct listnode *node;
	char *arg1, *arg2, *arg3, *arg4;
	int shots, n;
	int is_function;

	root = gtd->lua_ses->list[LIST_PROMPT];
	n = lua_gettop(L);

	arg1 = get_luastring(L, 1);

	if (n > 2)
	{
		if (!lua_isnumber(L, 2))
		{
			luaL_argerror(L, 2, "expected number");
		}
		arg3 = opt_luastring(L, 2);
	}
	else
	{
		arg3 = "";
	}

	if (n > 3)
	{
		if (!lua_isnumber(L, 3))
		{
			luaL_argerror(L, 3, "expected number");
		}
		arg4 = opt_luastring(L, 3);
	}
	else
	{
		arg4 = "";
	}

	if (n > 4)
	{
		if (!lua_isnumber(L, 4))
		{
			luaL_argerror(L, 4, "expected number");
		}
		shots = lua_tonumber(L, 4);
	}
	else
	{
		shots = 0;
	}

	is_function = lua_isfunction(L, n);

	if (is_function)
	{
		arg2 = "";
	}
	else
	{
		arg2 = get_luastring(L, n);
	}

	node = update_node_list(root, arg1, arg2, arg3, arg4);

	node->shots = shots;
	
	if (is_function)
	{
		lua_pushvalue(L, n);
		set_lua_data(L, node);
	}

	return 0;
}

DO_LUA(tt_macro)
{
	struct listroot *root;
	struct listnode *node;
	char *arg1, *arg2, *arg3;
	int n, shots, is_function;

	root = gtd->lua_ses->list[LIST_MACRO];

	n = lua_gettop(L);

	arg1 = get_luastring(L, 1);

	if (n > 2)
	{
		shots = luaL_optint(L, 2, 0);
	}
	else
	{
		shots = 0;
	}

	if (n > 3)
	{
		n = 3;
	}

	is_function = lua_isfunction(L, n);

	if (is_function)
	{
		arg2 = "";
	}
	else
	{
		arg2 = get_luastring(L, n);
	}


	arg3 = str_alloc_stack(0);

	tintin_macro_compile(arg1, arg3);

	node = update_node_list(root, arg1, arg2, "", arg3);

	node->shots = shots;
	
	if (is_function)
	{
		lua_pushvalue(L, n);
		set_lua_data(L, node);
	}

	return 0;
}

DO_LUA(tt_procedure)
{
	struct listroot *root;
	struct listnode *node;
	char *arg1, *arg2;
	int n, shots, is_function;

	root = gtd->lua_ses->list[LIST_PROCEDURE];

	n = lua_gettop(L);

	arg1 = get_luastring(L, 1);

	if (n > 2)
	{
		shots = luaL_optint(L, 2, 0);
	}
	else
	{
		shots = 0;
	}

	if (n > 3)
	{
		n = 3;
	}

	is_function = lua_isfunction(L, n);

	if (is_function)
	{
		arg2 = "";
	}
	else
	{
		arg2 = get_luastring(L, n);
	}

	node = update_node_list(root, arg1, arg2, "", "");

	node->shots = shots;
	
	if (is_function)
	{
		lua_pushvalue(L, n);
		set_lua_data(L, node);
	}

	return 0;
}

DO_LUA(tt_fun)
{
	struct listroot *root;
	struct listnode *node;
	char *arg1, *arg2;
	int n, shots, is_function;

	root = gtd->lua_ses->list[LIST_FUNCTION];

	n = lua_gettop(L);

	arg1 = get_luastring(L, 1);

	if (n > 2)
	{
		shots = luaL_optint(L, 2, 0);
	}
	else
	{
		shots = 0;
	}

	if (n > 3)
	{
		n = 3;
	}

	is_function = lua_isfunction(L, n);

	if (is_function)
	{
		arg2 = "";
	}
	else
	{
		arg2 = get_luastring(L, n);
	}

	node = update_node_list(root, arg1, arg2, "", "");

	node->shots = shots;
	
	if (is_function)
	{
		lua_pushvalue(L, n);
		set_lua_data(L, node);
	}

	return 0;
}

DO_LUA(tt_delay)
{
	struct listroot *root;
	struct listnode *node;
	char *arg1, *arg2;
	int is_function;
	double number;
	char time[NUMBER_SIZE];

	root = gtd->lua_ses->list[LIST_DELAY];

	number = luaL_checknumber(L, 1);
	arg1 = opt_luastring(L, 1);

	sprintf(time, "%llu.%010llu", gtd->utime + (unsigned long long) (number * 1000000), (unsigned long long) (number * 10000000000) % 10000000000);

	is_function = lua_isfunction(L, 2);

	if (is_function)
	{
		arg2 = "";
	}
	else
	{
		arg2 = get_luastring(L, 2);
	}

	node = create_node_list(root, time, arg2, arg1, "");
	
	if (is_function)
	{
		lua_pushvalue(L, 2);
		set_lua_data(L, node);
	}

	return 0;
}

DO_LUA(tt_tick)
{
	struct listroot *root;
	struct listnode *node;
	char *arg1, *arg2, *arg3;
	char time[NUMBER_SIZE];
	int n, shots, is_function;

	root = gtd->lua_ses->list[LIST_TICKER];

	n = lua_gettop(L);

	arg1 = get_luastring(L, 1);

	luaL_argcheck(L, lua_isnumber(L, 2), 2, "expected number");
	arg3 = get_luastring(L, 2);
	get_number_string(gtd->lua_ses, arg3, time);

	if (n > 3)
	{
		shots = luaL_optint(L, 3, 0);
	}
	else
	{
		shots = 0;
	}

	if (n > 4)
	{
		n = 4;
	}

	is_function = lua_isfunction(L, n);

	if (is_function)
	{
		arg2 = "";
	}
	else
	{
		arg2 = get_luastring(L, n);
	}

	node = update_node_list(root, arg1, arg2, time, arg3);

	node->shots = shots;
	
	if (is_function)
	{
		lua_pushvalue(L, n);
		set_lua_data(L, node);
	}

	return 0;
}

void make_lua_node(lua_State *L, int list, struct listnode *node, int convert_numbers)
{
	int has_class, has_shots;

	has_class = (*node->group != 0);
	has_shots = (node->shots != 0);

	switch (list)
	{
		case LIST_ACTION:
		case LIST_ALIAS:
			lua_createtable(L, 0, 3 + has_class + has_shots);

			lua_pushliteral(L, "match");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "cmd");
			if (IS_LUA_NODE(node))
			{
				push_lua_data(L, node);
			}
			else
			{
				lua_pushstring(L, node->arg2);
			}
			lua_rawset(L, -3);

			lua_pushliteral(L, "priority");
			push_lua_string_or_number(L, node->arg3, convert_numbers);
			lua_rawset(L, -3);

			break;

		case LIST_BUTTON:
			lua_createtable(L, 0, 8 + has_class + has_shots);

			lua_pushliteral(L, "name");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "cmd");
			if (IS_LUA_NODE(node))
			{
				push_lua_data(L, node);
			}
			else
			{
				lua_pushstring(L, node->arg2);
			}
			lua_rawset(L, -3);

			lua_pushliteral(L, "priority");
			push_lua_string_or_number(L, node->arg3, convert_numbers);
			lua_rawset(L, -3);

			lua_pushliteral(L, "event");
			lua_pushstring(L, node->arg4);
			lua_rawset(L, -3);

			lua_pushliteral(L, "row1");
			lua_pushinteger(L, node->val16[0]);
			lua_rawset(L, -3);

			lua_pushliteral(L, "col1");
			lua_pushinteger(L, node->val16[1]);
			lua_rawset(L, -3);

			lua_pushliteral(L, "row2");
			lua_pushinteger(L, node->val16[2]);
			lua_rawset(L, -3);

			lua_pushliteral(L, "col2");
			lua_pushinteger(L, node->val16[3]);
			lua_rawset(L, -3);

			break;

		case LIST_FUNCTION:
		case LIST_PROCEDURE:
			lua_createtable(L, 0, 2 + has_class + has_shots);

			lua_pushliteral(L, "name");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "cmd");
			if (IS_LUA_NODE(node))
			{
				push_lua_data(L, node);
			}
			else
			{
				lua_pushstring(L, node->arg2);
			}
			lua_rawset(L, -3);

			break;

		case LIST_CLASS:
			lua_createtable(L, 0, 3 + has_class + has_shots);

			lua_pushliteral(L, "name");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "open");
			push_lua_string_or_number(L, node->arg3, convert_numbers);
			lua_rawset(L, -3);

			lua_pushliteral(L, "memory");
			push_lua_string_or_number(L, node->arg4, convert_numbers);
			lua_rawset(L, -3);

			break;

		case LIST_COMMAND:
			lua_createtable(L, 0, 1 + has_class + has_shots);

			lua_pushliteral(L, "name");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			break;

		case LIST_CONFIG:
			lua_createtable(L, 0, 2 + has_class + has_shots);

			lua_pushliteral(L, "name");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "value");
			push_lua_string_or_number(L, node->arg2, convert_numbers);
			lua_rawset(L, -3);

			break;

		case LIST_DELAY:
			lua_createtable(L, 0, 3 + has_class + has_shots);

			lua_pushliteral(L, "name");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "cmd");
			lua_pushstring(L, node->arg2);
			lua_rawset(L, -3);

			lua_pushliteral(L, "time");
			lua_pushinteger(L, node->val64);
			lua_rawset(L, -3);

			break;

		case LIST_EVENT:
			lua_createtable(L, 0, 2 + has_class + has_shots);

			lua_pushliteral(L, "event");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "cmd");
			if (IS_LUA_NODE(node))
			{
				push_lua_data(L, node);
			}
			else
			{
				lua_pushstring(L, node->arg2);
			}
			lua_rawset(L, -3);

			break;

		case LIST_GAG:
			lua_createtable(L, 0, 1 + has_class + has_shots);

			lua_pushliteral(L, "match");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			break;

		case LIST_HIGHLIGHT:
			lua_createtable(L, 0, 3 + has_class + has_shots);

			lua_pushliteral(L, "match");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "color");
			lua_pushstring(L, node->arg2);
			lua_rawset(L, -3);

			lua_pushliteral(L, "priority");
			push_lua_string_or_number(L, node->arg3, convert_numbers);
			lua_rawset(L, -3);

			break;

		case LIST_HISTORY:
			lua_createtable(L, 0, 1 + has_class + has_shots);

			lua_pushliteral(L, "line");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			break;

		case LIST_LANDMARK:
			lua_createtable(L, 0, 4 + has_class + has_shots);

			lua_pushliteral(L, "name");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "desc");
			lua_pushstring(L, node->arg3);
			lua_rawset(L, -3);

			lua_pushliteral(L, "size");
			push_lua_string_or_number(L, node->arg4, convert_numbers);
			lua_rawset(L, -3);

			lua_pushliteral(L, "vnum");
			lua_pushinteger(L, node->val64);
			lua_rawset(L, -3);

			break;

		case LIST_MACRO:
			lua_createtable(L, 0, 3 + has_class + has_shots);

			lua_pushliteral(L, "input");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "cmd");
			if (IS_LUA_NODE(node))
			{
				push_lua_data(L, node);
			}
			else
			{
				lua_pushstring(L, node->arg2);
			}
			lua_rawset(L, -3);

			lua_pushliteral(L, "compiled");
			lua_pushstring(L, node->arg4);
			lua_rawset(L, -3);

			break;

		case LIST_PATH:
			lua_createtable(L, 0, 3 + has_class + has_shots);

			lua_pushliteral(L, "forward");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "backward");
			lua_pushstring(L, node->arg2);
			lua_rawset(L, -3);

			lua_pushliteral(L, "delay");
			if (*node->arg4 == 0 && convert_numbers)
			{
				lua_pushnumber(L, 0.0);
			}
			else
			{
				push_lua_string_or_number(L, node->arg4, convert_numbers);
			}
			lua_rawset(L, -3);

			break;

		case LIST_PATHDIR:
			lua_createtable(L, 0, 3 + has_class + has_shots);

			lua_pushliteral(L, "dir");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "reverse");
			lua_pushstring(L, node->arg2);
			lua_rawset(L, -3);

			lua_pushliteral(L, "coord");
			lua_pushinteger(L, node->val32[0]);
			lua_rawset(L, -3);

			break;

		case LIST_PROMPT:
			lua_createtable(L, 0, 4 + has_class + has_shots);

			lua_pushliteral(L, "match");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "sub");
			if (IS_LUA_NODE(node))
			{
				push_lua_data(L, node);
			}
			else
			{
				lua_pushstring(L, node->arg2);
			}
			lua_rawset(L, -3);

			lua_pushliteral(L, "row");
			push_lua_string_or_number(L, node->arg3, convert_numbers);
			lua_rawset(L, -3);

			lua_pushliteral(L, "col");
			push_lua_string_or_number(L, node->arg4, convert_numbers);
			lua_rawset(L, -3);

			break;

		case LIST_SUBSTITUTE:
			lua_createtable(L, 0, 3 + has_class + has_shots);

			lua_pushliteral(L, "match");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "sub");
			if (IS_LUA_NODE(node))
			{
				push_lua_data(L, node);
			}
			else
			{
				lua_pushstring(L, node->arg2);
			}
			lua_rawset(L, -3);

			lua_pushliteral(L, "priority");
			push_lua_string_or_number(L, node->arg3, convert_numbers);
			lua_rawset(L, -3);

			break;

		case LIST_TAB:
			lua_createtable(L, 0, 1 + has_class + has_shots);

			lua_pushliteral(L, "tab");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			break;

		case LIST_TERRAIN:
			lua_createtable(L, 0, 3 + has_class + has_shots);

			lua_pushliteral(L, "name");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "symbol");
			lua_pushstring(L, node->arg2);
			lua_rawset(L, -3);

			lua_pushliteral(L, "flags");
			lua_pushstring(L, node->arg3);
			lua_rawset(L, -3);

			break;

		case LIST_TICKER:
			lua_createtable(L, 0, 4 + has_class + has_shots);

			lua_pushliteral(L, "name");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "cmd");
			lua_pushstring(L, node->arg2);
			lua_rawset(L, -3);

			lua_pushliteral(L, "delay");
			push_lua_string_or_number(L, node->arg4, convert_numbers);
			lua_rawset(L, -3);

			lua_pushliteral(L, "time");
			lua_pushinteger(L, node->val64);
			lua_rawset(L, -3);

			break;

		case LIST_VARIABLE:
			lua_createtable(L, 0, 2 + has_class + has_shots);

			lua_pushliteral(L, "name");
			lua_pushstring(L, node->arg1);
			lua_rawset(L, -3);

			lua_pushliteral(L, "value");
			push_lua_var(L, node, convert_numbers);
			lua_rawset(L, -3);

			break;

	}

	if (has_class)
	{
		lua_pushliteral(L, "class");
		lua_pushstring(L, node->group);
		lua_rawset(L, -3);
	}

	if (has_shots)
	{
		lua_pushliteral(L, "shots");
		lua_pushinteger(L, node->shots);
		lua_rawset(L, -3);
	}
}

DO_LUA(tt_read_all)
{
	struct listroot *root;
	int list, i, convert_numbers;

	convert_numbers = !lua_isboolean(L, 1) || lua_toboolean(L, 1);

	lua_createtable(L, 0, LIST_MAX);
	for (list = 0 ; list < LIST_MAX ; list++)
	{
		lua_pushstring(L, list_names_lua[list]);

		root = gtd->lua_ses->list[list];

		lua_createtable(L, root->used, 0);

		for (i = 0 ; i < root->used ; i++)
		{
			make_lua_node(L, list, root->list[i], convert_numbers);
			lua_rawseti(L, -2, i + 1);
		}

		lua_rawset(L, -3);
	}

	return 1;
}

DO_LUA(tt_class)
{
	struct listnode *node;
	struct listroot *root;
	char *arg1;
	const char *str;
	int list, i, j, count, size, convert_numbers;

	arg1 = get_luastring(L, 1);

	if (lua_isboolean(L, 2))
	{
		str = "";
		convert_numbers = lua_toboolean(L, 2);
	}
	else
	{
		str = luaL_optstring(L, 2, "");
		convert_numbers = !lua_isboolean(L, 3) || lua_toboolean(L, 3);
	}

	node = search_node_list(gtd->lua_ses->list[LIST_CLASS], arg1);

	if (node == NULL)
	{
		lua_pushnil(L);
	}
	else if (!strcmp(str, "size"))
	{
		lua_pushinteger(L, count_class(gtd->lua_ses, node));
	}
	else if (*str)
	{
		for (list = 0 ; list < LIST_MAX ; list++)
		{
			if (strcmp(str, list_names_lua[list]))
			{
				continue;
			}

			lua_newtable(L);

			root = gtd->lua_ses->list[list];

			if (HAS_BIT(root->flags, LIST_FLAG_CLASS))
			{
				for (i = 0, j = 0 ; i < root->used ; i++)
				{
					if (!strcmp(root->list[i]->group, arg1))
					{
						make_lua_node(L, list, root->list[i], convert_numbers);
						lua_rawseti(L, -2, ++j);
					}
				}
			}

			break;
		}

		if (list == LIST_MAX)
		{
			luaL_error(L, "invalid class lookup: %s", str);
		}
	}
	else
	{
		count = 0;

		for (list = 0 ; list < LIST_MAX ; list++)
		{
			if (!HAS_BIT(gtd->lua_ses->list[list]->flags, LIST_FLAG_CLASS))
			{
				continue;
			}
			count++;
		}

		lua_createtable(L, 0, count+1);

		size = 0;

		for (list = 0 ; list < LIST_MAX ; list++)
		{
			root = gtd->lua_ses->list[list];

			if (!HAS_BIT(root->flags, LIST_FLAG_CLASS))
			{
				continue;
			}

			lua_pushstring(L, list_names_lua[list]);

			lua_newtable(L);

			for (i = 0, j = 0 ; i < root->used ; i++)
			{
				if (!strcmp(root->list[i]->group, arg1))
				{
					make_lua_node(L, list, root->list[i], convert_numbers);
					lua_rawseti(L, -2, ++j);
				}
			}

			lua_rawset(L, -3);

			size += j;
		}

		lua_pushinteger(L, size);
		lua_setfield(L, -2, "size");
	}

	return 1;
}

DO_LUA(read_lua_nodes)
{
	struct listroot *root;
	char *arg1;
	int list, i, index, convert_numbers;

	list = lua_tointeger(L, lua_upvalueindex(1));
	root = gtd->lua_ses->list[list];

	if (lua_isboolean(L, 1))
	{
		arg1 = "";
		convert_numbers = lua_toboolean(L, 1);
	}
	else
	{
		arg1 = opt_luastring(L, 1);

		if (lua_isboolean(L, 2))
		{
			convert_numbers = lua_toboolean(L, 2);
		}
		else
		{
			convert_numbers = TRUE;
		}
	}

	if (*arg1 == 0)
	{
		lua_createtable(L, root->used, 0);
		for (i = 0 ; i < root->used ; i++)
		{
			make_lua_node(L, list, root->list[i], convert_numbers);
			lua_rawseti(L, -2, i + 1);
		}

		return 1;
	}
	else
	{
		lua_createtable(L, root->used, 0);

		switch (list_table[root->type].mode)
		{
			case SORT_ALPHA:
			case SORT_STABLE:
				index = bsearch_alpha_list(root, arg1, 0);
				break;

			case SORT_ALNUM:
				index = bsearch_alnum_list(root, arg1, 0);
				break;

			default:
				index = nsearch_list(root, arg1);
				break;
		}

		if (index != -1)
		{
			make_lua_node(L, list, root->list[index], convert_numbers);
			lua_rawseti(L, -2, 1);
		}
		else
		{
			i = 1;
			for (index = 0 ; index < root->used ; index++)
			{
				if (tintin_regexp(gtd->lua_ses, NULL, root->list[index]->arg1, arg1, 0, 0))
				{
					make_lua_node(L, list, root->list[index], convert_numbers);
					lua_rawseti(L, -2, i++);
				}
			}
		}

		return 1;
	}
}

int limit_index(struct listroot *root, int index)
{
	if (index < 0)
	{
		index = root->used + index;

		if (index < 0)
		{
			index = 0;
		}
	}
	else if (index > 0)
	{
		if ((--index) >= root->used)
		{
			index = root->used;
		}
	}

	return index;
}

DO_LUA(tt_history)
{
	struct listroot *root = gtd->lua_ses->list[LIST_HISTORY];
	int i, j, n, min, max;

	if (lua_type(L, 1) == LUA_TSTRING && !strcmp(lua_tostring(L, 1), "size"))
	{
		lua_pushinteger(L, root->used);

		return 1;
	}

	if (lua_isnumber(L, 2))
	{
		if (!lua_isnumber(L, 1))
		{
			return luaL_argerror(L, 1, "expected a range of numbers");
		}

		min = limit_index(root, lua_tointeger(L, 1));
		max = limit_index(root, lua_tointeger(L, 2));
		
		if (max < min)
		{
			n = max;
			max = min;
			min = n;
		}

		lua_createtable(L, max + 1 - min, 0);

		for (i = min, j = 0 ; i <= max ; i++)
		{
			lua_pushstring(L, root->list[i]->arg1);
			lua_rawseti(L, -2, ++j);
		}

		return 1;
	}
	else if (lua_isnumber(L, 1))
	{
		n = lua_tointeger(L, 1);

		if (n < 0)
		{
			n = root->used + n;
			
			if (n < 0)
			{
				lua_pushnil(L);
				return 1;
			}
		}
		else if (n > 0)
		{
			if ((--n) > root->used)
			{
				lua_pushnil(L);
				return 1;
			}
		}

		lua_pushstring(L, root->list[n]->arg1);

		return 1;
	}
	else if (lua_isnoneornil(L, 1))
	{
		lua_createtable(L, root->used, 0);

		for (i = 0 ; i < root->used ; i++)
		{
			lua_pushstring(L, root->list[i]->arg1);
			lua_rawseti(L, -2, i + 1);
		}
		
		return 1;
	}
	else
	{
		return luaL_error(L, "expected a number, number range, or no arguments");
	}
}

DO_LUA(tt_parse)
{
	struct session *ses = gtd->lua_ses;
	char *arg, *arg1, *tmp;
	int first, depth, max_depth, convert_numbers;

	arg = get_luastring(L, 1);
	arg1 = str_alloc_stack(0);

	convert_numbers = !lua_isboolean(L, 2) || lua_toboolean(L, 2);

	if (*arg == 0)
	{
		lua_pushnil(L);
		return 1;
	}
	else if (*arg != DEFAULT_OPEN)
	{
		substitute(ses, arg, arg1, SUB_ESC);
		push_lua_string_or_number(L, arg1, convert_numbers);
		return 1;
	}

	tmp  = str_alloc_stack(0);

	lua_newtable(L);

	first = depth = max_depth = lua_gettop(L);

	lua_pushnil(L);

	while (TRUE)
	{
		if (*arg == 0)
		{
			if (depth > first)
			{
				depth -= 3;

				arg = lua_touserdata(L, -1);

				lua_pop(L, 1);

				lua_rawset(L, depth);

				continue;
			}
			else
			{
				lua_pop(L, 1);
				break;
			}
		}

		arg = get_arg_in_braces(ses, arg, tmp, GET_ONE);

		substitute(ses, tmp, arg1, SUB_ESC);
		push_lua_string_or_number(L, arg1, convert_numbers);

		arg = get_arg_in_braces(ses, arg, tmp, GET_ONE);

		if (*tmp != DEFAULT_OPEN)
		{
			substitute(ses, tmp, arg1, SUB_ESC);
			push_lua_string_or_number(L, arg1, convert_numbers);
			lua_rawset(L, depth);
		}
		else
		{
			lua_newtable(L);

			lua_pushlightuserdata(L, arg);

			arg = arg1;

			depth += 3;

			if (depth > max_depth)
			{
				max_depth = depth;
				luaL_checkstack(L, max_depth, "data too deep to parse");
			}

			continue;
		}
	}

	return 1;
}

DO_LUA(tt_parse_list)
{
	struct session *ses = gtd->lua_ses;
	char *arg, *arg1;
	int n = 0, convert_numbers;

	arg = get_luastring(L, 1);
	arg1 = str_alloc_stack(0);

	convert_numbers = !lua_isboolean(L, 2) || lua_toboolean(L, 2);

	lua_newtable(L);
	
	while (*arg)
	{
		arg = get_arg_in_braces(ses, arg, arg1, GET_ONE);
		push_lua_string_or_number(L, arg, convert_numbers);
		lua_rawseti(L, -2, ++n);
	}

	return 1;
}

int call_lua_function_adjust_flag(lua_State *L, int *flag)
{
	luaL_argcheck(L, lua_isfunction(L, 1) || lua_isstring(L, 1), 1, "expected function or string");

	(*flag)++;

	if (lua_isfunction(L, 1))
	{
		if (lua_pcall(L, lua_gettop(L) - 1, LUA_MULTRET, 0))
		{
			(*flag)--;

			return 1;
		}
	}
	else if (lua_isstring(L, 1))
	{
		tt_exec(L);
	}

	(*flag)--;

	return lua_gettop(L);
}

DO_LUA(tt_quiet)
{
	return call_lua_function_adjust_flag(L, &gtd->level->quiet);
}

DO_LUA(tt_convert)
{
	return call_lua_function_adjust_flag(L, &gtd->level->convert);
}

DO_LUA(tt_debug)
{
	return call_lua_function_adjust_flag(L, &gtd->level->debug);
}

DO_LUA(tt_ignore)
{
	return call_lua_function_adjust_flag(L, &gtd->level->ignore);
}

DO_LUA(tt_background)
{
	return call_lua_function_adjust_flag(L, &gtd->level->background);
}

DO_LUA(tt_get_gag)
{
	lua_pushinteger(L, gtd->lua_ses->gagline);

	return 1;
}

DO_LUA(tt_session)
{
	struct session *prev, *next;
	char *arg1;

	arg1 = get_luastring(L, 1);

	prev = gtd->lua_ses;

	next = find_session(arg1);

	if (next == NULL)
	{
		luaL_error(L, "no such session '%s'", arg1);
	}

	if (lua_pcall(L, lua_gettop(L) - 2, LUA_MULTRET, 0))
	{
		gtd->lua_ses = prev;

		return 1;
	}

	return lua_gettop(L) - 1;
}

DO_LUA(tt_session_env)
{
	struct session *ses;
	char *arg1;

	arg1 = get_luastring(L, 1);

	ses = find_session(arg1);

	if (ses == NULL)
	{
		luaL_error(L, "no such session '%s'", arg1);
	}

	lua_pushvalue(ses->lua, LUA_GLOBALSINDEX);
	lua_xmove(ses->lua, L, 1);

	return 1;
}

DO_LUA(tt_substitute)
{
	char *arg, *buf;
	int flags = 0;

	arg = get_luastring(L, 1);
	buf = str_alloc_stack(0);

	flags = luaL_optint(L, 2, SUB_SEC) & substitute_flag_mask;

	substitute(gtd->lua_ses, arg, buf, flags);

	lua_pushstring(L, buf);

	return 1;
}

DO_LUA(tt_color)
{
	char *arg, *result;

	arg = get_luastring(L, 1);

	result = str_alloc_stack(0);

	substitute(gtd->lua_ses, arg, result, SUB_COL);

	lua_pushstring(L, result);

	return 1;
}

DO_LUA(tt_get_input)
{
	lua_pushstring(L, gtd->ses->input->buf);
	return 1;
}

void prepare_lua_vars(lua_State *L, int n, int argc, char **vars, int *varc)
{
	int i, len;

	// check arguments first
	if (lua_istable(L, n))
	{
		if (argc > n)
		{
			luaL_error(L, "expected a table or list of string/numbers, not both");
		}

		len = lua_objlen(L, n);
		for (i = 1 ; i < len ; i++)
		{
			lua_rawgeti(L, n, i);
			if (!lua_isstring(L, -1))
			{
				luaL_argerror(L, n, "expected table or list of string/numbers");
			}
			lua_pop(L, 1);
		}
	}
	else
	{
		for (i = n ; i <= argc ; i++)
		{
			if (!lua_isstring(L, i))
			{
				luaL_argerror(L, i, "expected table or list of string/numbers");
			}
		}
	}

	// now apply them
	if (lua_istable(L, n))
	{
		len = lua_objlen(L, n);
		for (i = 1 ; i < len && i <= 100 ; i++)
		{
			lua_rawgeti(L, n, i);
			RESTRING(vars[i-1], lua_tostring(L, -1));
			lua_pop(L, 1);
		}

		for (*varc = i = i - 1 ; i < 100; i++)
		{
			*vars[i] = 0;
		}
	}
	else
	{
		for (i = n ; i <= argc && (i-n) < 100 ; i++)
		{
			RESTRING(vars[i-n], lua_tostring(L, i));
		}

		for (*varc = i = i - n ; i < 100; i++)
		{
			*vars[i] = 0;
		}
	}
}

#endif
