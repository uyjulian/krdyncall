/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2023-2023 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#include "tjsdyn_common.hpp"

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

NCB_REGISTER_CLASS(TjsDynStruct)
{
	RawCallback("dynpeek", &Class::method_dynpeek, TJS_STATICMEMBER);
	RawCallback("dynpoke", &Class::method_dynpoke, TJS_STATICMEMBER);
	RawCallback("dynwriteutf16string", &Class::method_dynwriteutf16string, TJS_STATICMEMBER);
	RawCallback("dynwriteutf8string", &Class::method_dynwriteutf8string, TJS_STATICMEMBER);
	RawCallback("sizeof", &Class::method_sizeof, TJS_STATICMEMBER);
};
