/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2023-2023 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#include "tjsdyn_common.hpp"

DCCallVM* g_pCallVM = NULL;

class TjsDynCall
{
public:
	static tjs_error TJS_INTF_METHOD method_dodyncall(
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

		void* f;
		int top = tjsW_gettop(&state);
		if (top < 2) return tjsW_error(&state, "Missing arguments #1 'addr' and #2 'signature'");
		
		if (tjsW_islightuserdata(&state, 1) )	f = tjsW_touserdata(&state, 1);
		else if (tjsW_isinteger(&state, 1) )	f = (void*) (uintptr_t) tjsW_checkinteger(&state, 1);
		else return tjsW_argerror(&state, 1, "Expected a cfunction, userdata or number");

		char *callsignature, *s;
		int callsignature_length;
		callsignature_length = tjsW_checkutf8string(&state, 2, NULL, 0);
		callsignature = (char *)__builtin_alloca(callsignature_length * sizeof(char));
		tjsW_checkutf8string(&state, 2, callsignature, callsignature_length);
		s = callsignature;

		/* parse mode */
	 
		// dcMode( g_pCallVM, DC_CALL_C_DEFAULT );
		dcReset( g_pCallVM );

		char ch;
		int p = 3;
		int ptr = 0;
		while ( (ch = *s++) && (ch != DC_SIGCHAR_ENDARG) )
		{
			if (p > top) return tjsW_error(&state, "Need more arguments (call signature is '%s')", callsignature );
			if (ptr == 0)
			{
				switch(ch)
				{
					case '*':
						ptr += 1;
						continue;
					case DC_SIGCHAR_BOOL:
						dcArgBool(g_pCallVM, (DCbool) tjsW_checkinteger(&state, p) );
						break;
					case DC_SIGCHAR_CHAR:
					case DC_SIGCHAR_UCHAR:
						dcArgChar(g_pCallVM, (DCchar) tjsW_checkinteger(&state, p) );
						break;
					case DC_SIGCHAR_SHORT:
					case DC_SIGCHAR_USHORT:
						dcArgShort(g_pCallVM, (DCshort) tjsW_checkinteger(&state, p) );
						break;
					case DC_SIGCHAR_INT:
					case DC_SIGCHAR_UINT:
						dcArgInt(g_pCallVM, (DCint) tjsW_checkinteger(&state, p) );
						break;
					case DC_SIGCHAR_LONG:
					case DC_SIGCHAR_ULONG:
						dcArgLong(g_pCallVM, (DClong) tjsW_checkinteger(&state, p) );
						break;
					case DC_SIGCHAR_LONGLONG:
					case DC_SIGCHAR_ULONGLONG:
						dcArgLongLong(g_pCallVM, (DClonglong) tjsW_checkinteger(&state, p) );
						break;
					case DC_SIGCHAR_FLOAT:
						dcArgFloat(g_pCallVM, (DCfloat) tjsW_checkreal(&state, p) );
						break; 
					case DC_SIGCHAR_DOUBLE:
						dcArgDouble(g_pCallVM, (DCdouble) tjsW_checkreal(&state, p) );
						break;
					case DC_SIGCHAR_POINTER:
						dcArgPointer(g_pCallVM, (DCpointer) tjsW_touserdata(&state, p) );
						break;
					case DC_SIGCHAR_STRING:
						return tjsW_error(&state, "String typecode not yet implemented");
					default:
						return tjsW_error(&state, "Invalid typecode '%c' in call signature '%s'", s[0], callsignature);
				}
			}
			else /* pointer types */
			{
				switch (ch) {
					case '*':
						ptr++;
						continue;
					case '<': {
#if 0
						const char *begin = s;
						while ((ch = *s++) != '>');
						const char *end = s;
						switch (lua_type(L, p)) {
							case LUA_TNUMBER:
							case LUA_TTABLE:
							case LUA_TLIGHTUSERDATA:
							case LUA_TUSERDATA:
							default:
								tjsW_error(&state, "Pointer type mismatch at argument #%d", p);
								break;
						}
#endif
						return tjsW_error(&state, "Pointer type not yet implemented");
					}
						break;
					case DC_SIGCHAR_BOOL:
					case DC_SIGCHAR_CHAR:
						return tjsW_error(&state, "bool/char pointer type not yet implemented");
					case DC_SIGCHAR_UCHAR:
					case DC_SIGCHAR_SHORT:
					case DC_SIGCHAR_USHORT:
					case DC_SIGCHAR_INT:
					case DC_SIGCHAR_UINT:
					case DC_SIGCHAR_LONG:
					case DC_SIGCHAR_ULONG:
					case DC_SIGCHAR_LONGLONG:
					case DC_SIGCHAR_ULONGLONG:
					case DC_SIGCHAR_FLOAT:
					case DC_SIGCHAR_DOUBLE:
					case DC_SIGCHAR_POINTER:
					case DC_SIGCHAR_STRING:
					case DC_SIGCHAR_VOID:
						ptr = 0;
						return tjsW_error(&state, "Pointer type not yet implemented");
#if 0
						break;
#endif
					default:
						return tjsW_error(&state, "Invalid signature");
				}
			}

			p += 1;
		}

		if (top >= p)
		{
			return tjsW_error(&state, "too many arguments for given signature, expected %d but received %d" , p-3, top-2 );
		}
			

		switch(*s++)
		{
			case DC_SIGCHAR_VOID:
				dcCallVoid(g_pCallVM, f);
				return 0;
			case DC_SIGCHAR_BOOL:   
				tjsW_pushinteger( &state, dcCallBool(g_pCallVM, f) ? 1 : 0 ); 
				break;
			case DC_SIGCHAR_CHAR:
			case DC_SIGCHAR_UCHAR:
				tjsW_pushinteger( &state, (tTVInteger) ( dcCallChar(g_pCallVM,f) ) );
				break;
			case DC_SIGCHAR_SHORT:
			case DC_SIGCHAR_USHORT:
				tjsW_pushinteger( &state, (tTVInteger)( dcCallShort(g_pCallVM, f) ) );
				break;
			case DC_SIGCHAR_INT:
			case DC_SIGCHAR_UINT:
				tjsW_pushinteger( &state, (tTVInteger)( dcCallInt(g_pCallVM, f) ) );
				break;
			case DC_SIGCHAR_LONG:
			case DC_SIGCHAR_ULONG:
				tjsW_pushinteger( &state, (tTVInteger)( dcCallLong(g_pCallVM, f) ) );
				break;
			case DC_SIGCHAR_LONGLONG:
			case DC_SIGCHAR_ULONGLONG:
				tjsW_pushinteger( &state, (tTVInteger)( dcCallLongLong(g_pCallVM, f) ) );
				break;
			case DC_SIGCHAR_FLOAT:
				tjsW_pushreal( &state, (tTVReal) dcCallFloat(g_pCallVM, f) );
				break;
			case DC_SIGCHAR_DOUBLE:
				tjsW_pushreal( &state, (tTVReal) dcCallDouble(g_pCallVM, f) ); 
				break;
			case DC_SIGCHAR_STRING:
				return tjsW_error(&state, "String return not yet supported");
			case DC_SIGCHAR_POINTER:
				tjsW_pushlightuserdata( &state, dcCallPointer(g_pCallVM, f) );
				break;
			case '*':
				switch (*s++) {
					case DC_SIGCHAR_UCHAR:
					case DC_SIGCHAR_CHAR:
						return tjsW_error(&state, "String return not yet supported");
					default:
						tjsW_pushlightuserdata( &state, dcCallPointer(g_pCallVM, f) );
						break;
				}
				break;
			default:
				return tjsW_error(&state, "invalid signature");
		}
		return TJS_S_OK;
	}
	static tjs_error TJS_INTF_METHOD method_topointer(
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

		tjsW_pushlightuserdata(&state, (void*) (uintptr_t) tjsW_checkinteger(&state, 1) );
		return TJS_S_OK;
	}
	static tjs_error TJS_INTF_METHOD method_frompointer(
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

		tjsW_pushinteger( &state, (tTVInteger)(uintptr_t)( tjsW_touserdata(&state, 1) ) );
		return TJS_S_OK;
	}
};

NCB_REGISTER_CLASS(TjsDynCall)
{
	RawCallback("dodyncall", &Class::method_dodyncall, TJS_STATICMEMBER);
	RawCallback("topointer", &Class::method_topointer, TJS_STATICMEMBER);
	RawCallback("frompointer", &Class::method_frompointer, TJS_STATICMEMBER);
	Variant("_COPYRIGHT", TJS_W("Copyright Julian Uy"));
	Variant("_DESCRIPTION", TJS_W("TJS bindings for dyncall libraries"));
	Variant("_VERSION", TJS_W("0.1"));
};


static void TjsDyncallPreRegistCallback()
{
	g_pCallVM = dcNewCallVM(4096);
}

static void TjsDyncallPostUnregistCallback()
{
	dcFree(g_pCallVM);
}

NCB_PRE_REGIST_CALLBACK(TjsDyncallPreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(TjsDyncallPostUnregistCallback);