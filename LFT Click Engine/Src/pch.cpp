// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	pch.h
// Author			:	Abhijit Zala
// Creation Date	:	2021/02/20
// Purpose			:	Static header
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"

#ifdef _DEBUG
bool CheckLua(lua_State* L, int r)
{
	{
		if (r != LUA_OK)
		{
			std::string errormsg = lua_tostring(L, -1);
			std::cout << errormsg << std::endl;
			return false;
		}
		return true;
	}
}
#endif