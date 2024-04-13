#include "tintin.h"

#ifdef HAVE_LUA

DO_LUA(map_api_at);
DO_LUA(map_api_count);
DO_LUA(map_api_exists);
DO_LUA(map_api_find);
DO_LUA(map_api_global);
DO_LUA(map_api_inside);
DO_LUA(map_api_last_room);
DO_LUA(map_api_list);
DO_LUA(map_api_size);

/* getters */

DO_LUA(map_api_get_all);
DO_LUA(map_api_get_area);
DO_LUA(map_api_get_color);
DO_LUA(map_api_get_data);
DO_LUA(map_api_get_desc);
DO_LUA(map_api_get_flags);
DO_LUA(map_api_get_id);
DO_LUA(map_api_get_name);
DO_LUA(map_api_get_note);
DO_LUA(map_api_get_symbol);
DO_LUA(map_api_get_terrain);
DO_LUA(map_api_get_vnum);
DO_LUA(map_api_get_weight);
DO_LUA(map_api_get_exits);
DO_LUA(map_api_get_worldflags);
DO_LUA(map_api_get_dir);
DO_LUA(map_api_get_pathdir);

DO_LUA(map_api_exit);
DO_LUA(map_api_exit_color);
DO_LUA(map_api_exit_cmd);
DO_LUA(map_api_exit_data);
DO_LUA(map_api_exit_delay);
DO_LUA(map_api_exit_dir);
DO_LUA(map_api_exit_flags);
DO_LUA(map_api_exit_name);
DO_LUA(map_api_exit_vnum);
DO_LUA(map_api_exit_weight);

const luaL_Reg map_api_reg[] = {
	{ "at", map_api_at },
	{ "exists", map_api_exists },
	{ "exit", map_api_exit },
	{ "exit_color", map_api_exit_color },
	{ "exit_cmd", map_api_exit_cmd },
	{ "exit_data", map_api_exit_data },
	{ "exit_delay", map_api_exit_delay },
	{ "exit_dir", map_api_exit_dir },
	{ "exit_flags", map_api_exit_flags },
	{ "exit_name", map_api_exit_name },
	{ "exit_vnum", map_api_exit_vnum },
	{ "exit_weight", map_api_exit_weight },
	{ "find", map_api_find },
	{ "last_room", map_api_last_room },
	{ "get_all", map_api_get_all },
	{ "get_area", map_api_get_area },
	{ "get_color", map_api_get_color },
	{ "get_data", map_api_get_data },
	{ "get_desc", map_api_get_desc },
	{ "get_flags", map_api_get_flags },
	{ "get_id", map_api_get_id },
	{ "get_name", map_api_get_name },
	{ "get_note", map_api_get_note },
	{ "get_symbol", map_api_get_symbol },
	{ "get_terrain", map_api_get_terrain },
	{ "get_vnum", map_api_get_vnum },
	{ "get_weight", map_api_get_weight },
	{ "get_exits", map_api_get_exits },
	{ "get_worldflags", map_api_get_worldflags },
	{ "get_dir", map_api_get_dir },
	{ "get_pathdir", map_api_get_pathdir },
	{ "global", map_api_global },
	{ "inside", map_api_inside },
	{ "size", map_api_size },
	{ "count", map_api_count },
	{ "list", map_api_list },
	{ NULL, NULL }
};

struct map_api_int_consts
{
	char *name;
	int   value;
};

static struct map_api_int_consts map_api_int_consts[] =
{
	{ "ROOM_FLAG_AVOID",               ROOM_FLAG_AVOID },
	{ "ROOM_FLAG_HIDE",                ROOM_FLAG_HIDE },
	{ "ROOM_FLAG_LEAVE",               ROOM_FLAG_LEAVE },
	{ "ROOM_FLAG_VOID",                ROOM_FLAG_VOID },
	{ "ROOM_FLAG_STATIC",              ROOM_FLAG_STATIC },
	{ "ROOM_FLAG_CURVED",              ROOM_FLAG_CURVED },
	{ "ROOM_FLAG_PATH",                ROOM_FLAG_PATH },
	{ "ROOM_FLAG_NOGLOBAL",            ROOM_FLAG_NOGLOBAL },
	{ "ROOM_FLAG_INVIS",               ROOM_FLAG_INVIS },
	{ "ROOM_FLAG_BLOCK",               ROOM_FLAG_BLOCK },
	{ "ROOM_FLAG_TERRAIN",             ROOM_FLAG_TERRAIN },
	{ "ROOM_FLAG_FOG",                 ROOM_FLAG_FOG },

	{ "EXIT_FLAG_HIDE",                EXIT_FLAG_HIDE },
	{ "EXIT_FLAG_AVOID",               EXIT_FLAG_AVOID },
	{ "EXIT_FLAG_INVIS",               EXIT_FLAG_INVIS },
	{ "EXIT_FLAG_BLOCK",               EXIT_FLAG_BLOCK },

	{ "MAP_FLAG_ASCIIGRAPHICS",        MAP_FLAG_ASCIIGRAPHICS },
	{ "MAP_FLAG_ASCIILENGTH",          MAP_FLAG_ASCIILENGTH },
	{ "MAP_FLAG_ASCIIVNUMS",           MAP_FLAG_ASCIIVNUMS },
	{ "MAP_FLAG_AUTOLINK",             MAP_FLAG_AUTOLINK },
	{ "MAP_FLAG_BLOCKGRAPHICS",        MAP_FLAG_BLOCKGRAPHICS },
	{ "MAP_FLAG_DIRECTION",            MAP_FLAG_DIRECTION },
	{ "MAP_FLAG_MUDFONT",              MAP_FLAG_MUDFONT },
	{ "MAP_FLAG_NOFOLLOW",             MAP_FLAG_NOFOLLOW },
	{ "MAP_FLAG_QUIET",                MAP_FLAG_QUIET },
	{ "MAP_FLAG_STATIC",               MAP_FLAG_STATIC },
	{ "MAP_FLAG_SYMBOLGRAPHICS",       MAP_FLAG_SYMBOLGRAPHICS },
	{ "MAP_FLAG_TERRAIN",              MAP_FLAG_TERRAIN },
	{ "MAP_FLAG_UNICODEGRAPHICS",      MAP_FLAG_UNICODEGRAPHICS },

	{ "TERRAIN_FLAG_DENSE",            TERRAIN_FLAG_DENSE },
	{ "TERRAIN_FLAG_AMPLE",            TERRAIN_FLAG_AMPLE },
	{ "TERRAIN_FLAG_SPARSE",           TERRAIN_FLAG_SPARSE },
	{ "TERRAIN_FLAG_SCANT",            TERRAIN_FLAG_SCANT },
	{ "TERRAIN_FLAG_NARROW",           TERRAIN_FLAG_NARROW },
	{ "TERRAIN_FLAG_STANDARD",         TERRAIN_FLAG_STANDARD },
	{ "TERRAIN_FLAG_WIDE",             TERRAIN_FLAG_WIDE },
	{ "TERRAIN_FLAG_VAST",             TERRAIN_FLAG_VAST },
	{ "TERRAIN_FLAG_FADEIN",           TERRAIN_FLAG_FADEIN },
	{ "TERRAIN_FLAG_FADEOUT",          TERRAIN_FLAG_FADEOUT },
	{ "TERRAIN_FLAG_DOUBLE",           TERRAIN_FLAG_DOUBLE },

	{ NULL,                            0 }
};

int find_room(struct session *ses, char *arg);
int find_path(struct session *ses, char *arg);
struct exit_data *find_exit(struct session *ses, int room, char *arg);
int searchgrid_find(struct session *ses, int from, struct search_data *search);
int match_room(struct session *ses, int room, struct search_data *search);

void map_api_init(lua_State *L)
{
	int i;

	luaL_register(L, "map", map_api_reg);

	for (i = 0 ; map_api_int_consts[i].name != NULL ; i++)
	{
		lua_pushinteger(L, map_api_int_consts[i].value);
		lua_setfield(L, -2, map_api_int_consts[i].name);
	}

	lua_pop(L, 1);
}

void check_in_map_lua(lua_State *L)
{
	if (gtd->lua_ses->map == NULL)
	{
		luaL_error(L, "the session has no map");
	}
}

int get_in_room_lua(lua_State *L, int n)
{
	int in_room;

	check_in_map_lua(L);

	if (lua_isnumber(L, n))
	{
		in_room = lua_tonumber(L, n);

		if (in_room < 1 || in_room > gtd->lua_ses->map->size)
		{
			return luaL_error(L, "invalid room: %d", in_room);
		}
	}
	else
	{
		in_room = gtd->lua_ses->map->in_room;

		if (gtd->lua_ses->map->room_list[in_room] == NULL)
		{
			luaL_error(L, "not inside the map");
		}
	}

	return in_room;
}

DO_LUA(map_api_at)
{
	struct session *ses;
	char *arg1, *arg2;
	int new_room;

	check_in_map_lua(L);

	ses = gtd->lua_ses;

	luaL_argcheck(L, lua_isfunction(L, 2) || lua_isstring(L, 2), 2, "expected string or function");

	if (ses->map->at_room)
	{
		return luaL_error(L, "can't nest map.at() calls");
	}

	arg1 = get_luastring(L, 1);
	new_room = find_room(ses, arg1);

	ses->map->at_room = ses->map->in_room;

	if (new_room == 0)
	{
		if (ses->map->in_room)
		{
			new_room = find_path(ses, arg1);
		}

		if (new_room == 0)
		{
			lua_pushboolean(L, FALSE);
			return 1;
		}
	}

	ses->map->in_room = new_room;

	if (lua_isfunction(L, 2))
	{
		lua_call(L, lua_gettop(L) - 2, LUA_MULTRET);
	}
	else if (lua_isstring(L, 1))
	{
		arg2 = get_luastring(L, 2);

		script_driver(ses, LIST_COMMAND, arg2);
	}

	lua_pushboolean(L, TRUE);
	lua_replace(L, 1);

	if (ses->map)
	{
		ses->map->in_room = ses->map->at_room;
		ses->map->at_room = 0;
	}

	return lua_gettop(L);
}

struct exit_data *get_lua_exit(lua_State *L)
{
	char *arg1;
	int vnum;

	arg1 = get_luastring(L, 1);

	vnum = get_in_room_lua(L, 2);

	return find_exit(gtd->lua_ses, vnum, arg1);
}

DO_LUA(map_api_exit)
{
	struct exit_data *exit;

	exit = get_lua_exit(L);

	if (exit == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_createtable(L, 0, 9);

		lua_pushliteral(L, "color");
		lua_pushstring(L, exit->color);
		lua_rawset(L, -3);

		lua_pushliteral(L, "cmd");
		lua_pushstring(L, exit->cmd);
		lua_rawset(L, -3);

		lua_pushliteral(L, "data");
		lua_pushstring(L, exit->data);
		lua_rawset(L, -3);

		lua_pushliteral(L, "delay");
		lua_pushnumber(L, exit->delay);
		lua_rawset(L, -3);

		lua_pushliteral(L, "dir");
		lua_pushinteger(L, exit->dir);
		lua_rawset(L, -3);

		lua_pushliteral(L, "flags");
		lua_pushinteger(L, exit->flags);
		lua_rawset(L, -3);

		lua_pushliteral(L, "name");
		lua_pushstring(L, exit->name);
		lua_rawset(L, -3);

		lua_pushliteral(L, "vnum");
		lua_pushinteger(L, exit->vnum);
		lua_rawset(L, -3);

		lua_pushliteral(L, "weight");
		lua_pushnumber(L, exit->weight);
		lua_rawset(L, -3);
	}

	return 1;
}

DO_LUA(map_api_exit_color)
{
	struct exit_data *exit;

	exit = get_lua_exit(L);

	if (exit == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_pushstring(L, exit->color);
	}

	return 1;
}

DO_LUA(map_api_exit_cmd)
{
	struct exit_data *exit;

	exit = get_lua_exit(L);

	if (exit == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_pushstring(L, exit->cmd);
	}

	return 1;
}

DO_LUA(map_api_exit_data)
{
	struct exit_data *exit;

	exit = get_lua_exit(L);

	if (exit == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_pushstring(L, exit->data);
	}

	return 1;
}

DO_LUA(map_api_exit_delay)
{
	struct exit_data *exit;

	exit = get_lua_exit(L);

	if (exit == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_pushnumber(L, exit->delay);
	}

	return 1;
}

DO_LUA(map_api_exit_dir)
{
	struct exit_data *exit;

	exit = get_lua_exit(L);

	if (exit == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_pushinteger(L, exit->dir);
	}

	return 1;
}

DO_LUA(map_api_exit_flags)
{
	struct exit_data *exit;

	exit = get_lua_exit(L);

	if (exit == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_pushinteger(L, exit->flags);
	}

	return 1;
}

DO_LUA(map_api_exit_name)
{
	struct exit_data *exit;

	exit = get_lua_exit(L);

	if (exit == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_pushstring(L, exit->name);
	}

	return 1;
}

DO_LUA(map_api_exit_vnum)
{
	struct exit_data *exit;

	exit = get_lua_exit(L);

	if (exit == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_pushinteger(L, exit->vnum);
	}

	return 1;
}

DO_LUA(map_api_exit_weight)
{
	struct exit_data *exit;

	exit = get_lua_exit(L);

	if (exit == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_pushnumber(L, exit->weight);
	}

	return 1;
}

void compile_lua_search(lua_State *L, int n, struct session *ses)
{
	struct search_data *search = ses->map->search;
	struct listnode *node;
	char *arg1;

	luaL_argcheck(L, lua_isnoneornil(L, n) || lua_istable(L, n) || lua_isstring(L, n), n, "expected optional string or table");

	arg1 = str_alloc_stack(0);

	search->vnum = search->min = search->max = 0;

	if (search->arg)
	{
		free(search->arg);
		search->arg = NULL;
	}

	if (lua_isstring(L, n))
	{
		search->arg = strdup(lua_tostring(L, n));
		
		node = search_node_list(ses->list[LIST_LANDMARK], search->arg);

		if (node && node->val32[0])
		{
			search->vnum = node->val32[0];
			return;
		}
	}
	else if (lua_istable(L, n) && lua_objlen(L, n) == 2)
	{
		search->vnum = 0;

		lua_rawgeti(L, n, 1);
		lua_rawgeti(L, n, 2);

		if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2))
		{
			luaL_error(L, "expected range of numbers");
		}

		search->min = lua_tointeger(L, -2);
		search->max = lua_tointeger(L, -1);

		lua_pop(L, 2);

		if (search->min < 1)
		{
			search->min = 1;
		}

		if (search->max > ses->map->size)
		{
			search->min = ses->map->size;
		}
	}

	if (search->name->regex)
	{
		free(search->name->regex);
		search->name->regex = NULL;
	}

	if (search->exit_list)
	{
		free(search->exit_list);
	}

	if (search->desc->regex)
	{
		free(search->desc->regex);
		search->desc->regex = NULL;
	}

	if (search->area->regex)
	{
		free(search->area->regex);
		search->area->regex = NULL;
	}

	if (search->note->regex)
	{
		free(search->note->regex);
		search->note->regex = NULL;
	}

	if (search->terrain->regex)
	{
		free(search->terrain->regex);
		search->terrain->regex = NULL;
	}

	search->flag = search->galf = 0;

	if (search->id)
	{
		free(search->id);
		search->id = NULL;
	}

	search->distance = 0;

	if (lua_isnoneornil(L, n))
	{
		search->name->regex = tintin_regexp_compile(ses, search->desc, "%*", PCRE_ANCHORED);

		return;
	}
	else if (lua_isstring(L, n))
	{
		strcpy(arg1, search->arg);
		strcat(arg1, "$");
		search->name->regex = tintin_regexp_compile(ses, search->desc, arg1, PCRE_ANCHORED);

		return;
	}

	lua_getfield(L, n, "name");
	if (lua_isstring(L, -1))
	{
		strcpy(arg1, lua_tostring(L, -1));
		strcat(arg1, "$");
		search->name->regex = tintin_regexp_compile(ses, search->desc, arg1, PCRE_ANCHORED);
	}
	lua_pop(L, 1);

	lua_getfield(L, n, "desc");
	if (lua_isstring(L, -1))
	{
		strcpy(arg1, lua_tostring(L, -1));
		strcat(arg1, "$");
		search->desc->regex = tintin_regexp_compile(ses, search->desc, arg1, PCRE_ANCHORED);
	}
	lua_pop(L, 1);

	lua_getfield(L, n, "area");
	if (lua_isstring(L, -1))
	{
		strcpy(arg1, lua_tostring(L, -1));
		strcat(arg1, "$");
		search->area->regex = tintin_regexp_compile(ses, search->area, arg1, PCRE_ANCHORED);
	}
	lua_pop(L, 1);

	lua_getfield(L, n, "note");
	if (lua_isstring(L, -1))
	{
		strcpy(arg1, lua_tostring(L, -1));
		strcat(arg1, "$");
		search->note->regex = tintin_regexp_compile(ses, search->note, arg1, PCRE_ANCHORED);
	}
	lua_pop(L, 1);

	lua_getfield(L, n, "terrain");
	if (lua_isstring(L, -1))
	{
		strcpy(arg1, lua_tostring(L, -1));
		strcat(arg1, "$");
		search->terrain->regex = tintin_regexp_compile(ses, search->terrain, arg1, PCRE_ANCHORED);
	}
	lua_pop(L, 1);

	lua_getfield(L, n, "flags");
	if (lua_isnumber(L, -1))
	{
		search->flag = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, n, "noflags");
	if (lua_isnumber(L, -1))
	{
		search->galf = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, n, "id");
	if (lua_isstring(L, -1))
	{
		search->id = strdup(lua_tostring(L, -1));
	}
	lua_pop(L, 1);

	lua_getfield(L, n, "distance");
	if (lua_isnumber(L, -1))
	{
		search->distance = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);
}

void push_room_data(lua_State *L, struct map_data *map, struct room_data *room)
{
	int connected;

	connected = map->search->stamp == room->search_stamp;

	lua_createtable(L, 0, 4 + connected);

	lua_pushliteral(L, "vnum");
	lua_pushinteger(L, room->vnum);
	lua_rawset(L, -3);

	lua_pushliteral(L, "x");
	lua_pushinteger(L, room->x);
	lua_rawset(L, -3);

	lua_pushliteral(L, "y");
	lua_pushinteger(L, room->y);
	lua_rawset(L, -3);

	lua_pushliteral(L, "z");
	lua_pushinteger(L, room->z);
	lua_rawset(L, -3);

	if (connected)
	{
		lua_pushliteral(L, "distance");
		lua_pushnumber(L, room->length);
		lua_rawset(L, -3);
	}
}

DO_LUA(map_api_find)
{
	struct map_data *map = gtd->lua_ses->map;
	int n, vnum, dest;

	n = lua_gettop(L);

	if (n == 1)
	{
		check_in_map_lua(L);
		vnum = map->in_room;
		compile_lua_search(L, 1, gtd->lua_ses);
	}
	else
	{
		vnum = get_in_room_lua(L, 1);
		compile_lua_search(L, 2, gtd->lua_ses);
	}

	dest = searchgrid_find(gtd->lua_ses, vnum, map->search);

	if (dest)
	{
		push_room_data(L, map, map->room_list[dest]);
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}

DO_LUA(map_api_list)
{
	struct map_data *map = gtd->lua_ses->map;
	struct room_data *room;
	int n, vnum, cnt;

	n = lua_gettop(L);

	if (n == 1)
	{
		check_in_map_lua(L);
		vnum = map->in_room;
		compile_lua_search(L, 1, gtd->lua_ses);
	}
	else
	{
		vnum = get_in_room_lua(L, 1);
		compile_lua_search(L, 2, gtd->lua_ses);
	}

	searchgrid_find(gtd->lua_ses, vnum, map->search);

	lua_newtable(L);

	for (vnum = cnt = 0 ; vnum < map->size ; vnum++)
	{
		if (match_room(gtd->lua_ses, vnum, map->search))
		{
			room = map->room_list[vnum];

			push_room_data(L, map, room);

			lua_rawseti(L, -2, ++cnt);
		}
	}

	return 1;
}

DO_LUA(map_api_get_all)
{
	struct room_data *room;
	struct exit_data *exit;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_createtable(L, 0, 12);

	lua_pushliteral(L, "color");
	lua_pushstring(L, room->color);
	lua_rawset(L, -3);

	lua_pushliteral(L, "data");
	lua_pushstring(L, room->data);
	lua_rawset(L, -3);

	lua_pushliteral(L, "desc");
	lua_pushstring(L, room->desc);
	lua_rawset(L, -3);

	lua_pushliteral(L, "flags");
	lua_pushinteger(L, room->flags);
	lua_rawset(L, -3);

	lua_pushliteral(L, "id");
	lua_pushstring(L, room->id);
	lua_rawset(L, -3);

	lua_pushliteral(L, "name");
	lua_pushstring(L, room->name);
	lua_rawset(L, -3);

	lua_pushliteral(L, "note");
	lua_pushstring(L, room->note);
	lua_rawset(L, -3);

	lua_pushliteral(L, "symbol");
	lua_pushstring(L, room->symbol);
	lua_rawset(L, -3);

	lua_pushliteral(L, "terrain");
	lua_pushstring(L, room->terrain);
	lua_rawset(L, -3);

	lua_pushliteral(L, "vnum");
	lua_pushinteger(L, room->vnum);
	lua_rawset(L, -3);

	lua_pushliteral(L, "weight");
	lua_pushnumber(L, room->weight);
	lua_rawset(L, -3);

	lua_pushliteral(L, "exits");
	lua_newtable(L);

	for (exit = room->f_exit ; exit ; exit = exit->next)
	{
		lua_pushstring(L, exit->name);
		lua_pushinteger(L, exit->vnum);
		lua_rawset(L, -3);
	}

	lua_rawset(L, -3);

	return 1;
}

DO_LUA(map_api_get_area)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushstring(L, room->area);

	return 1;
}

DO_LUA(map_api_get_color)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushstring(L, room->color);

	return 1;
}

DO_LUA(map_api_get_data)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushstring(L, room->data);

	return 1;
}

DO_LUA(map_api_get_desc)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushstring(L, room->desc);

	return 1;
}

DO_LUA(map_api_get_flags)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushinteger(L, room->flags);

	return 1;
}

DO_LUA(map_api_get_id)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushstring(L, room->id);

	return 1;
}

DO_LUA(map_api_get_name)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushstring(L, room->name);

	return 1;
}

DO_LUA(map_api_get_note)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushstring(L, room->note);

	return 1;
}

DO_LUA(map_api_get_symbol)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushstring(L, room->symbol);

	return 1;
}

DO_LUA(map_api_get_terrain)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushstring(L, room->terrain);

	return 1;
}

DO_LUA(map_api_get_vnum)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushinteger(L, room->vnum);

	return 1;
}

DO_LUA(map_api_get_weight)
{
	struct room_data *room;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_pushnumber(L, room->weight);

	return 1;
}

DO_LUA(map_api_get_exits)
{
	struct room_data *room;
	struct exit_data *exit;
	int vnum;

	vnum = get_in_room_lua(L, 1);

	room = gtd->lua_ses->map->room_list[vnum];

	lua_newtable(L);

	for (exit = room->f_exit ; exit ; exit = exit->next)
	{
		lua_pushstring(L, exit->name);
		lua_pushinteger(L, exit->vnum);
		lua_rawset(L, -3);
	}

	return 1;
}

DO_LUA(map_api_get_worldflags)
{
	check_in_map_lua(L);

	lua_pushinteger(L, gtd->lua_ses->map->flags);

	return 1;
}

DO_LUA(map_api_get_dir)
{
	check_in_map_lua(L);

	lua_pushinteger(L, gtd->lua_ses->map->dir);

	return 1;
}

DO_LUA(map_api_get_pathdir)
{
	check_in_map_lua(L);

	lua_pushstring(L, dir_to_exit(gtd->lua_ses, gtd->lua_ses->map->dir));

	return 1;
}

void push_room_or_nil(lua_State *L, int vnum)
{
	if (vnum == 0)
	{
		lua_pushnil(L);
	}
	else
	{
		lua_pushinteger(L, vnum);
	}
}

DO_LUA(map_api_exists)
{
	lua_pushboolean(L, gtd->lua_ses->map != NULL);
	return 1;
}

DO_LUA(map_api_inside)
{
	struct map_data *map = gtd->lua_ses->map;

	if (map != NULL)
	{
		lua_pushboolean(L, map->room_list[map->in_room] != NULL);
	}
	else
	{
		lua_pushboolean(L, FALSE);
	}
	return 1;
}

DO_LUA(map_api_global)
{
	check_in_map_lua(L);

	push_room_or_nil(L, gtd->lua_ses->map->global_vnum);

	return 1;
}

DO_LUA(map_api_last_room)
{
	check_in_map_lua(L);

	push_room_or_nil(L, gtd->lua_ses->map->last_room);

	return 1;
}

DO_LUA(map_api_count)
{
	struct map_data *map;
	int room, cnt, exits;

	check_in_map_lua(L);

	map = gtd->lua_ses->map;

	for (room = cnt = exits = 0 ; room < map->size ; room++)
	{
		if (map->room_list[room])
		{
			cnt++;

			exits += map->room_list[room]->exit_size;
		}
	}

	lua_pushinteger(L, cnt);
	lua_pushinteger(L, exits);

	return 2;
}

DO_LUA(map_api_size)
{
	check_in_map_lua(L);

	lua_pushinteger(L, gtd->lua_ses->map->size);

	return 1;
}



#endif
