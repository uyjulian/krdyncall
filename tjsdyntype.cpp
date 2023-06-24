/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2023-2023 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#include "tjsdyn_common.hpp"

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

NCB_REGISTER_CLASS(TjsDynType)
{
	RawCallback("dtSize", &Class::method_dtSize, TJS_STATICMEMBER);
	RawCallback("dtAlign", &Class::method_dtAlign, TJS_STATICMEMBER);
};
