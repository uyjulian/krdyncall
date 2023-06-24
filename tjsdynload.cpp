/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2023-2023 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#include "tjsdyn_common.hpp"
#include <stdio.h>

class TjsDynLoad
{
public:
	static tjs_error TJS_INTF_METHOD method_dlLoadLibrary(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
		tjsW_state_t state;
		state.result = result;
		state.numparams = numparams;
		state.param = param;
		state.objthis = objthis;

		char* libpath;
		int libpath_length;

		libpath = NULL;
		if ( tjsW_isnoneorvoid(&state, 1) == 0 ) 
		{
			libpath_length = tjsW_checkutf8string(&state, 1, NULL, 0);
			libpath = (char *)__builtin_alloca(libpath_length * sizeof(char));
			tjsW_checkutf8string(&state, 1, libpath, libpath_length);
		}

		DLLib* pLib;
		pLib = dlLoadLibrary(libpath);
		if (pLib == NULL)
		{
			return TJS_S_OK; 
		}
		tjsW_pushlightuserdata(&state, pLib);
		return TJS_S_OK;
	}

	static tjs_error TJS_INTF_METHOD method_dlFreeLibrary(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
		tjsW_state_t state;
		state.result = result;
		state.numparams = numparams;
		state.param = param;
		state.objthis = objthis;

		if ( tjsW_isnoneorvoid(&state, 1) != 0 ) 
		{
			return tjsW_error(&state, "Invalid argument");
		}

		DLLib* pLib = (DLLib*) tjsW_touserdata(&state, 1);
		dlFreeLibrary(pLib);
		return TJS_S_OK;
	}

	static tjs_error TJS_INTF_METHOD method_dlFindSymbol(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
		tjsW_state_t state;
		state.result = result;
		state.numparams = numparams;
		state.param = param;
		state.objthis = objthis;

		if ( tjsW_isnoneorvoid(&state, 1) != 0 ) 
		{
			return tjsW_error(&state, "Invalid argument");
		}

		DLLib* pLib = (DLLib*) (void*) tjsW_touserdata(&state, 1);
		char* pSymbolName;
		int pSymbolName_length;
		pSymbolName_length = tjsW_checkutf8string(&state, 2, NULL, 0);
		pSymbolName = (char *)__builtin_alloca(pSymbolName_length * sizeof(char));
		tjsW_checkutf8string(&state, 2, pSymbolName, pSymbolName_length);
		void* addr = dlFindSymbol(pLib, pSymbolName);
		if (addr == NULL) return 0;
		tjsW_pushlightuserdata(&state, addr);
		return TJS_S_OK;
	}
};

NCB_REGISTER_CLASS(TjsDynLoad)
{
	RawCallback("dlLoadLibrary", &Class::method_dlLoadLibrary, TJS_STATICMEMBER);
	RawCallback("dlFreeLibrary", &Class::method_dlFreeLibrary, TJS_STATICMEMBER);
	RawCallback("dlFindSymbol", &Class::method_dlFindSymbol, TJS_STATICMEMBER);
};
