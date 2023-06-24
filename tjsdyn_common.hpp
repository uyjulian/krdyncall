/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2023-2023 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#include "ncbind/ncbind.hpp"
#include "dyncall.h"
#include "dynload.h"
#include "CharacterSet.h"

typedef struct tjsW_state_
{
	tTJSVariant	*result;
	tjs_int numparams;
	tTJSVariant **param;
	iTJSDispatch2 *objthis;
} tjsW_state_t;

static inline tTJSVariant *tjsW_getvariant(const tjsW_state_t *state, int narg)
{
	if (narg == 0)
	{
		return nullptr;
	}
	else if (narg >= 1)
	{
		if (state->numparams > (narg - 1))
		{
			return state->param[narg - 1];
		}
	}
	else if (narg <= -1)
	{
		if (state->numparams - narg >= 0)
		{
			return state->param[state->numparams - narg];
		}
	}
	return nullptr;
}


static inline void tjsW_checkany(const tjsW_state_t *state, int narg)
{
	tTJSVariant *variant;

	variant = tjsW_getvariant(state, narg);
	if (variant == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Argument not found"));
	}
}

static inline tjs_int tjsW_checkint(const tjsW_state_t *state, int narg)
{
	tTJSVariant *variant;

	variant = tjsW_getvariant(state, narg);
	if (variant == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Argument not found"));
	}

	if (variant->Type() != tvtInteger)
	{
		TVPThrowExceptionMessage(TJS_W("Invalid variant type"));
	}

	return (tjs_int)(variant->AsInteger());
}

static inline void tjsW_pushint(const tjsW_state_t *state, tjs_int n)
{
	if (state->result != nullptr)
	{
		*(state->result) = n;
	}
}

static inline tTVInteger tjsW_checkinteger(const tjsW_state_t *state, int narg)
{
	tTJSVariant *variant;

	variant = tjsW_getvariant(state, narg);
	if (variant == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Argument not found"));
	}

	if (variant->Type() != tvtInteger)
	{
		TVPThrowExceptionMessage(TJS_W("Invalid variant type"));
	}

	return variant->AsInteger();
}

static inline void tjsW_pushinteger(const tjsW_state_t *state, tTVInteger n)
{
	if (state->result != nullptr)
	{
		*(state->result) = n;
	}
}

static inline tTVReal tjsW_checkreal(const tjsW_state_t *state, int narg)
{
	tTJSVariant *variant;

	variant = tjsW_getvariant(state, narg);
	if (variant == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Argument not found"));
	}

	if (variant->Type() != tvtReal)
	{
		TVPThrowExceptionMessage(TJS_W("Invalid variant type"));
	}

	return variant->AsReal();
}

static inline void tjsW_pushreal(const tjsW_state_t *state, tTVReal n)
{
	if (state->result != nullptr)
	{
		*(state->result) = n;
	}
}

static inline tjs_int tjsW_checkutf16string(const tjsW_state_t *state, int narg, tjs_char *outstr_buf, tjs_int outstr_length)
{
	tTJSVariant *variant;

	variant = tjsW_getvariant(state, narg);
	if (variant == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Argument not found"));
	}

	if (variant->Type() != tvtString)
	{
		TVPThrowExceptionMessage(TJS_W("Invalid variant type"));
	}

	tTJSVariantString *varstr;
	const tjs_char *str_contents;
	tjs_int str_length;

	str_contents = TJS_W("");
	str_length = 0;
	varstr = variant->AsStringNoAddRef();
	if (varstr != NULL)
	{
		str_length = varstr->GetLength();
		if (str_length != 0)
		{
			str_contents = varstr->operator const tjs_char *();
		}
	}

	if (outstr_buf != NULL && outstr_length + 1 >= str_length + 1)
	{
		memcpy(outstr_buf, str_contents, str_length * sizeof(tjs_char));
		outstr_buf[str_length] = 0;
	}

	return str_length + 1;
}

static inline void tjsW_pushutf16string(const tjsW_state_t *state, const tjs_char *s, tjs_int len)
{
	if (state->result != nullptr)
	{
		*(state->result) = ttstr(s, len);
	}
}

static inline tjs_int tjsW_checkutf8string(const tjsW_state_t *state, int narg, char *outstr_buf, size_t outstr_length)
{
	tTJSVariant *variant;

	variant = tjsW_getvariant(state, narg);
	if (variant == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Argument not found"));
	}

	if (variant->Type() != tvtString)
	{
		TVPThrowExceptionMessage(TJS_W("Invalid variant type"));
	}

	tTJSVariantString *varstr;
	const tjs_char *str_contents;
	tjs_int str_length;

	str_contents = TJS_W("");
	str_length = 0;
	varstr = variant->AsStringNoAddRef();
	if (varstr != NULL)
	{
		str_length = varstr->GetLength();
		if (str_length != 0)
		{
			str_contents = varstr->operator const tjs_char *();
		}
	}

	tjs_string str_utf16 = str_contents;
	std::string str_utf8;
	TVPUtf16ToUtf8(str_utf8, str_utf16);

	size_t str_utf8_length;
	str_utf8_length = str_utf8.length();

	if (outstr_buf != NULL && outstr_length + 1 >= str_utf8_length + 1)
	{
		memcpy(outstr_buf, str_utf8.c_str(), str_utf8_length * sizeof(char));
		outstr_buf[str_utf8_length] = 0;
	}

	return str_utf8_length + 1;
}

static inline void tjsW_pushutf8string(const tjsW_state_t *state, const char *s, tjs_int len)
{
	if (state->result != nullptr)
	{
		std::string str_utf8(s, len);
		tjs_string str_utf16;
		TVPUtf8ToUtf16(str_utf16, str_utf8);

		*(state->result) = ttstr(str_utf16.c_str(), str_utf16.length());
	}
}

static inline int tjsW_gettop(const tjsW_state_t *state)
{
	return state->numparams;
}

static inline int tjsW_error(const tjsW_state_t *state, const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	if (state->result != nullptr)
	{
		char* err;
		int err_length;
		err_length = vsprintf(NULL, fmt, va);
		err = (char *)__builtin_alloca((err_length + 1) * sizeof(char));
		if (err)
		{
			vsprintf(NULL, fmt, va);
			TVPThrowExceptionMessage(ttstr(err).c_str());
		}
		else
		{
			TVPThrowExceptionMessage(TJS_W("Exception throw could not allocate memory"));
		}
	}
	va_end(va);
	
	return TJS_S_OK;
}

static inline int tjsW_argerror(const tjsW_state_t *state, int narg, const char *fmt, ...)
{
	va_list va;

	(void)narg;
	va_start(va, fmt);
	if (state->result != nullptr)
	{
		char* err;
		int err_length;
		err_length = vsprintf(NULL, fmt, va);
		err = (char *)__builtin_alloca((err_length + 1) * sizeof(char));
		if (err)
		{
			vsprintf(NULL, fmt, va);
			TVPThrowExceptionMessage(ttstr(err).c_str());
		}
		else
		{
			TVPThrowExceptionMessage(TJS_W("Exception throw could not allocate memory"));
		}
	}
	va_end(va);
	
	return TJS_S_OK;
}

static inline int tjsW_isinteger(const tjsW_state_t *state, int narg)
{
	tTJSVariant *variant;

	variant = tjsW_getvariant(state, narg);
	if (variant == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Argument not found"));
	}
	return variant->Type() == tvtInteger;
}

static inline int tjsW_isnoneorvoid(const tjsW_state_t *state, int narg)
{
	return tjsW_getvariant(state, narg) == nullptr;
}


// The following definitions are defined in tjsdynuserdata.cpp
extern void tjsW_pushlightuserdata(const tjsW_state_t *state, void *p);
extern int tjsW_islightuserdata(const tjsW_state_t *state, int narg);
extern void *tjsW_touserdata(const tjsW_state_t *state, int narg);

// defined in tjsdyntype.cpp
extern size_t dtSize(const char* signature);
