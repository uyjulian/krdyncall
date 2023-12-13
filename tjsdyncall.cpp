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

size_t dtSize(const char* signature)
{
	char ch = *signature;
	switch(ch)
	{
		case DC_SIGCHAR_BOOL: return sizeof(DCbool);
		case DC_SIGCHAR_CHAR: return sizeof(DCchar);
		case DC_SIGCHAR_UCHAR: return sizeof(DCuchar);
		case DC_SIGCHAR_SHORT: return sizeof(DCshort);
		case DC_SIGCHAR_USHORT: return sizeof(DCushort);
		case DC_SIGCHAR_INT: return sizeof(DCint);
		case DC_SIGCHAR_UINT: return sizeof(DCuint);
		case DC_SIGCHAR_LONG: return sizeof(DClong);
		case DC_SIGCHAR_ULONG: return sizeof(DCulong);
		case DC_SIGCHAR_LONGLONG: return sizeof(DClonglong);
		case DC_SIGCHAR_ULONGLONG: return sizeof(DCulonglong);
		case DC_SIGCHAR_FLOAT: return sizeof(DCfloat);
		case DC_SIGCHAR_DOUBLE: return sizeof(DCdouble);
		case DC_SIGCHAR_POINTER: return sizeof(DCpointer);
		case DC_SIGCHAR_STRING: return sizeof(DCstring);
		case DC_SIGCHAR_VOID: return 0; // sizeof(DCvoid);
		default: return 0;
	}
}

size_t dtAlign(const char* signature)
{
	return dtSize(signature);
}


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

NCB_REGISTER_SUBCLASS(TjsDynLoad)
{
	RawCallback("dlLoadLibrary", &Class::method_dlLoadLibrary, TJS_STATICMEMBER);
	RawCallback("dlFreeLibrary", &Class::method_dlFreeLibrary, TJS_STATICMEMBER);
	RawCallback("dlFindSymbol", &Class::method_dlFindSymbol, TJS_STATICMEMBER);
};


class TjsDynStruct
{
public:
	static tjs_error TJS_INTF_METHOD method_dynpeek(
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

		char *ptr = (char *)tjsW_touserdata(&state, 1);
		int offset = tjsW_checkint(&state, 2);
		char* typesig;
		int typesig_length;
		typesig_length = tjsW_checkutf8string(&state, 3, NULL, 0);
		typesig = (char *)__builtin_alloca(typesig_length * sizeof(char));
		tjsW_checkutf8string(&state, 3, typesig, typesig_length);
		const char* sig = typesig;
		char ch;
		ptr += offset;
		while ((ch = *sig++) != '\0') {
			switch(ch) {
				case DC_SIGCHAR_BOOL:   	tjsW_pushinteger(&state, (tTVInteger) *((int*)ptr)); break;
				case DC_SIGCHAR_CHAR:   	tjsW_pushinteger(&state, (tTVInteger) *((char*)ptr)); break;
				case DC_SIGCHAR_UCHAR:  	tjsW_pushinteger(&state, (tTVInteger) *((unsigned char*)ptr)); break;
				case DC_SIGCHAR_SHORT:  	tjsW_pushinteger(&state, (tTVInteger) *((short*)ptr)); break;
				case DC_SIGCHAR_USHORT: 	tjsW_pushinteger(&state, (tTVInteger) *((unsigned short*)ptr)); break;
				case DC_SIGCHAR_INT:    	tjsW_pushinteger(&state, (tTVInteger) *((int*)ptr)); break;
				case DC_SIGCHAR_UINT:   	tjsW_pushinteger(&state, (tTVInteger) *((unsigned int*)ptr)); break;
				case DC_SIGCHAR_LONG:   	tjsW_pushinteger(&state, (tTVInteger) *((long*)ptr)); break;
				case DC_SIGCHAR_ULONG:  	tjsW_pushinteger(&state, (tTVInteger) *((unsigned long*)ptr)); break;
				case DC_SIGCHAR_LONGLONG: 	tjsW_pushinteger(&state, (tTVInteger) *((DClonglong*)ptr)); break;
				case DC_SIGCHAR_ULONGLONG: 	tjsW_pushinteger(&state, (tTVInteger) *((DCulonglong*)ptr)); break;
				case DC_SIGCHAR_FLOAT: 		tjsW_pushreal(&state, (tTVInteger) *((float*)ptr)); break;
				case DC_SIGCHAR_DOUBLE: 	tjsW_pushreal(&state, (tTVInteger) *((double*)ptr)); break;
				case DC_SIGCHAR_POINTER: 	tjsW_pushlightuserdata(&state, *((void**)ptr) ); break;
				// case DC_SIGCHAR_STRING: lua_pushstring(L, (const char*)ptr); break;
				default: tjsW_error(&state, "Invalid type signature %s", typesig); break;
			}
		}
		return TJS_S_OK;
	}
	static tjs_error TJS_INTF_METHOD method_dynpoke(
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

		char *ptr = (char *)tjsW_touserdata(&state, 1);
		int offset = tjsW_checkint(&state, 2);
		char* typesig;
		int typesig_length;
		typesig_length = tjsW_checkutf8string(&state, 3, NULL, 0);
		typesig = (char *)__builtin_alloca(typesig_length * sizeof(char));
		tjsW_checkutf8string(&state, 3, typesig, typesig_length);
		const char *sig     = typesig;
		char ch;
		ptr += offset;
		while ((ch=*sig++) != '\0') {
			switch(ch) {
				case DC_SIGCHAR_BOOL:   *((int*)ptr) = (tjsW_checkinteger(&state, 4) ? 1 : 0); break;
				case DC_SIGCHAR_CHAR:   *((char*)ptr) = (char) tjsW_checkinteger(&state, 4); break;
				case DC_SIGCHAR_UCHAR:  *((unsigned char*)ptr) = (unsigned char) tjsW_checkinteger(&state, 4); break;
				case DC_SIGCHAR_SHORT:  *((short*)ptr) = (short) tjsW_checkinteger(&state, 4); break;
				case DC_SIGCHAR_USHORT: *((unsigned short*)ptr) = (unsigned short) tjsW_checkinteger(&state, 4); break;
				case DC_SIGCHAR_INT:    *((int*)ptr) = (int) tjsW_checkinteger(&state, 4); break;
				case DC_SIGCHAR_UINT:   *((unsigned int*)ptr) = (unsigned int) tjsW_checkinteger(&state, 4); break;
				case DC_SIGCHAR_LONG:   *((long*)ptr) = (long) tjsW_checkinteger(&state, 4); break;
				case DC_SIGCHAR_ULONG:  *((unsigned long*)ptr) = (unsigned long) tjsW_checkinteger(&state, 4); break;
				case DC_SIGCHAR_LONGLONG: *((DClonglong*)ptr) = (long long) tjsW_checkinteger(&state, 4); break;
				case DC_SIGCHAR_ULONGLONG: *((DCulonglong*)ptr) = (unsigned long long) tjsW_checkinteger(&state, 4); break;
				case DC_SIGCHAR_FLOAT: *((float*)ptr) = (float) tjsW_checkreal(&state, 4); break;
				case DC_SIGCHAR_DOUBLE: *((double*)ptr) = (double) tjsW_checkreal(&state, 4); break;
				case DC_SIGCHAR_POINTER: *((const void**)ptr) = tjsW_touserdata(&state, 4); break;
				case DC_SIGCHAR_STRING: *((const char**)ptr) = (const char *)tjsW_touserdata(&state, 4); break;
				default: tjsW_error(&state, "Invalid type signature"); break;
			}
		}
		return TJS_S_OK;
	}
	static tjs_error TJS_INTF_METHOD method_dynwriteutf16string(
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

		char *ptr = (char *)tjsW_touserdata(&state, 1);
		int offset = tjsW_checkint(&state, 2);
		tjs_char* str;
		int str_length;
		str_length = tjsW_checkutf16string(&state, 3, NULL, 0);
		str = (tjs_char *)__builtin_alloca(str_length * sizeof(tjs_char));
		tjsW_checkutf16string(&state, 3, str, str_length);

		memcpy((void *)(((char *)ptr) + offset), str, str_length * sizeof(tjs_char));

		return TJS_S_OK;
	}
	static tjs_error TJS_INTF_METHOD method_dynwriteutf8string(
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

		char *ptr = (char *)tjsW_touserdata(&state, 1);
		int offset = tjsW_checkint(&state, 2);
		char* str;
		int str_length;
		str_length = tjsW_checkutf8string(&state, 3, NULL, 0);
		str = (char *)__builtin_alloca(str_length * sizeof(char));
		tjsW_checkutf8string(&state, 3, str, str_length);

		memcpy((void *)(((char *)ptr) + offset), str, str_length * sizeof(char));

		return TJS_S_OK;
	}
	static tjs_error TJS_INTF_METHOD method_sizeof(
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

		char* typeinfo;
		int typeinfo_length;
		typeinfo_length = tjsW_checkutf8string(&state, 1, NULL, 0);
		typeinfo = (char *)__builtin_alloca(typeinfo_length * sizeof(char));
		tjsW_checkutf8string(&state, 1, typeinfo, typeinfo_length);
		tjsW_pushinteger(&state, dtSize(typeinfo));
		return TJS_S_OK;
	}

};

NCB_REGISTER_SUBCLASS(TjsDynStruct)
{
	RawCallback("dynpeek", &Class::method_dynpeek, TJS_STATICMEMBER);
	RawCallback("dynpoke", &Class::method_dynpoke, TJS_STATICMEMBER);
	RawCallback("dynwriteutf16string", &Class::method_dynwriteutf16string, TJS_STATICMEMBER);
	RawCallback("dynwriteutf8string", &Class::method_dynwriteutf8string, TJS_STATICMEMBER);
	RawCallback("sizeof", &Class::method_sizeof, TJS_STATICMEMBER);
};

class TjsDynType
{
public:
	static tjs_error TJS_INTF_METHOD method_dtSize(
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

		char* signature;
		int signature_length;
		signature_length = tjsW_checkutf8string(&state, 1, NULL, 0);
		signature = (char *)__builtin_alloca(signature_length * sizeof(char));
		tjsW_checkutf8string(&state, 1, signature, signature_length);
		tjsW_pushinteger(&state, dtSize(signature));
		return TJS_S_OK;
	}
	static tjs_error TJS_INTF_METHOD method_dtAlign(
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

		char* signature;
		int signature_length;
		signature_length = tjsW_checkutf8string(&state, 1, NULL, 0);
		signature = (char *)__builtin_alloca(signature_length * sizeof(char));
		tjsW_checkutf8string(&state, 1, signature, signature_length);
		tjsW_pushinteger(&state, dtAlign(signature));
		return TJS_S_OK;
	}
};

NCB_REGISTER_SUBCLASS(TjsDynType)
{
	RawCallback("dtSize", &Class::method_dtSize, TJS_STATICMEMBER);
	RawCallback("dtAlign", &Class::method_dtAlign, TJS_STATICMEMBER);
};

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
	NCB_SUBCLASS(Load, TjsDynLoad);
	NCB_SUBCLASS(Struct, TjsDynStruct);
	NCB_SUBCLASS(Type, TjsDynType);
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
