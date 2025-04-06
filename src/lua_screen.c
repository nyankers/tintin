#include "tintin.h"

#ifdef HAVE_LUA

DO_LUA(screen_api_char_height);
DO_LUA(screen_api_char_width);
DO_LUA(screen_api_cols);
DO_LUA(screen_api_cur_col);
DO_LUA(screen_api_cur_row);
DO_LUA(screen_api_focus);
DO_LUA(screen_api_height);
DO_LUA(screen_api_input_rows);
DO_LUA(screen_api_input_cols);
DO_LUA(screen_api_input_name);
DO_LUA(screen_api_input_top_row);
DO_LUA(screen_api_input_top_col);
DO_LUA(screen_api_input_bot_row);
DO_LUA(screen_api_input_bot_col);
DO_LUA(screen_api_rows);
DO_LUA(screen_api_split_top_bar);
DO_LUA(screen_api_split_left_bar);
DO_LUA(screen_api_split_bot_bar);
DO_LUA(screen_api_split_right_bar);
DO_LUA(screen_api_scroll_rows);
DO_LUA(screen_api_scroll_cols);
DO_LUA(screen_api_scroll_top_row);
DO_LUA(screen_api_scroll_top_col);
DO_LUA(screen_api_scroll_bot_row);
DO_LUA(screen_api_scroll_bot_col);
DO_LUA(screen_api_width);

const luaL_Reg screen_api_reg[] = {
	{ "char_height", screen_api_char_height },
	{ "char_width", screen_api_char_width },
	{ "cols", screen_api_cols },
	{ "cur_col", screen_api_cur_col },
	{ "cur_row", screen_api_cur_row },
	{ "focus", screen_api_focus },
	{ "height", screen_api_height },
	{ "input_rows", screen_api_input_rows },
	{ "input_cols", screen_api_input_cols },
	{ "input_name", screen_api_input_name },
	{ "input_top_row", screen_api_input_top_row },
	{ "input_top_col", screen_api_input_top_col },
	{ "input_bot_row", screen_api_input_bot_row },
	{ "input_bot_col", screen_api_input_bot_col },
	{ "rows", screen_api_rows },
	{ "split_top_bar", screen_api_split_top_bar },
	{ "split_left_bar", screen_api_split_left_bar },
	{ "split_bot_bar", screen_api_split_bot_bar },
	{ "split_right_bar", screen_api_split_right_bar },
	{ "scroll_rows", screen_api_scroll_rows },
	{ "scroll_cols", screen_api_scroll_cols },
	{ "scroll_top_row", screen_api_scroll_top_row },
	{ "scroll_top_col", screen_api_scroll_top_col },
	{ "scroll_bot_row", screen_api_scroll_bot_row },
	{ "scroll_bot_col", screen_api_scroll_bot_col },
	{ "width", screen_api_width },
	{ NULL, NULL }
};

void screen_api_init(lua_State *L)
{
	luaL_register(L, "screen", screen_api_reg);
}

DO_LUA(screen_api_char_height)
{
	lua_pushinteger(L, gtd->screen->char_height);
	return 1;
}

DO_LUA(screen_api_char_width)
{
	lua_pushinteger(L, gtd->screen->char_width);
	return 1;
}

DO_LUA(screen_api_cols)
{
	lua_pushinteger(L, gtd->screen->cols);
	return 1;
}

DO_LUA(screen_api_cur_col)
{
	lua_pushinteger(L, gtd->lua_ses->cur_col);
	return 1;
}

DO_LUA(screen_api_cur_row)
{
	lua_pushinteger(L, gtd->lua_ses->cur_row);
	return 1;
}

DO_LUA(screen_api_focus)
{
	lua_pushinteger(L, gtd->screen->focus);
	return 1;
}

DO_LUA(screen_api_height)
{
	lua_pushinteger(L, gtd->screen->height);
	return 1;
}

DO_LUA(screen_api_input_rows)
{
	lua_pushinteger(L, 1 + gtd->lua_ses->input->bot_row - gtd->lua_ses->input->top_row);
	return 1;
}

DO_LUA(screen_api_input_cols)
{
	lua_pushinteger(L, 1 + gtd->lua_ses->input->bot_col - gtd->lua_ses->input->top_col);
	return 1;
}

DO_LUA(screen_api_input_name)
{
	lua_pushstring(L, gtd->lua_ses->input->line_name);
	return 1;
}

DO_LUA(screen_api_input_top_row)
{
	lua_pushinteger(L, gtd->lua_ses->input->top_row);
	return 1;
}

DO_LUA(screen_api_input_top_col)
{
	lua_pushinteger(L, gtd->lua_ses->input->top_col);
	return 1;
}

DO_LUA(screen_api_input_bot_row)
{
	lua_pushinteger(L, gtd->lua_ses->input->bot_row);
	return 1;
}

DO_LUA(screen_api_input_bot_col)
{
	lua_pushinteger(L, gtd->lua_ses->input->bot_col);
	return 1;
}

DO_LUA(screen_api_rows)
{
	lua_pushinteger(L, gtd->screen->rows);
	return 1;
}

DO_LUA(screen_api_split_top_bar)
{
	lua_pushinteger(L, gtd->lua_ses->split->sav_top_row);
	return 1;
}

DO_LUA(screen_api_split_left_bar)
{
	lua_pushinteger(L, gtd->lua_ses->split->sav_top_col);
	return 1;
}

DO_LUA(screen_api_split_bot_bar)
{
	lua_pushinteger(L, gtd->lua_ses->split->sav_bot_row);
	return 1;
}

DO_LUA(screen_api_split_right_bar)
{
	lua_pushinteger(L, gtd->lua_ses->split->sav_bot_col);
	return 1;
}

DO_LUA(screen_api_scroll_rows)
{
	lua_pushinteger(L, get_scroll_rows(gtd->lua_ses));
	return 1;
}

DO_LUA(screen_api_scroll_cols)
{
	lua_pushinteger(L, get_scroll_cols(gtd->lua_ses));
	return 1;
}

DO_LUA(screen_api_scroll_top_row)
{
	lua_pushinteger(L, gtd->lua_ses->split->top_row);
	return 1;
}

DO_LUA(screen_api_scroll_top_col)
{
	lua_pushinteger(L, gtd->lua_ses->split->top_col);
	return 1;
}

DO_LUA(screen_api_scroll_bot_row)
{
	lua_pushinteger(L, gtd->lua_ses->split->bot_row);
	return 1;
}

DO_LUA(screen_api_scroll_bot_col)
{
	lua_pushinteger(L, gtd->lua_ses->split->bot_col);
	return 1;
}

DO_LUA(screen_api_width)
{
	lua_pushinteger(L, gtd->screen->width);
	return 1;
}

#endif
