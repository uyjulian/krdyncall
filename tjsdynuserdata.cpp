/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2023-2023 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#include "tjsdyn_common.hpp"

// NOTE: Do not add "static" to the following variable. Otherwise it will be duplicated and pointer comparison won't work
const tjs_char *userdata_wrapper_classname = TJS_W("__TJS2UserdataWrapper");
static tjs_uint32 userdata_wrapper_hint = 0;

class iTJSDispatch2UserdataWrapper : public tTJSDispatch
{
	public: iTJSDispatch2UserdataWrapper()
	{
	}

	public: ~iTJSDispatch2UserdataWrapper() {}

	public: tjs_error TJS_INTF_METHOD FuncCallByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		return TJS_E_NOTIMPL;
	}

	public: tjs_error TJS_INTF_METHOD PropGetByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result, iTJSDispatch2 *objthis)
	{
		return TJS_E_NOTIMPL;
	}

	public: tjs_error TJS_INTF_METHOD PropSetByNum(tjs_uint32 flag, tjs_int num, const tTJSVariant *param, iTJSDispatch2 *objthis)
	{
		return TJS_E_NOTIMPL;
	}

	public: tjs_error TJS_INTF_METHOD DeleteMemberByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis)
	{
		return TJS_E_NOTIMPL;
	}

	public: tjs_error TJS_INTF_METHOD IsInstanceOf(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, const tjs_char * classname, iTJSDispatch2 *objthis)
	{
		if (membername == nullptr)
		{
			// We only care about the pointers, not the contents
			if (hint == &userdata_wrapper_hint && classname == userdata_wrapper_classname)
			{
				return TJS_S_TRUE;
			}
		}
		return TJS_S_FALSE;
	}

	public: void *get_userdata()
	{
		return this->userdata;
	}

	public: void set_userdata(void *in_userdata)
	{
		this->userdata = in_userdata;
	}

	protected: void *userdata = nullptr;
};

void tjsW_pushlightuserdata(const tjsW_state_t *state, void *p)
{
	if (state->result != nullptr)
	{
		iTJSDispatch2UserdataWrapper *tjsobj = new iTJSDispatch2UserdataWrapper();
		if (tjsobj != nullptr)
		{
			tjsobj->set_userdata(p);
			*(state->result) = tTJSVariant((iTJSDispatch2 *)tjsobj, (iTJSDispatch2 *)tjsobj);
			tjsobj->Release(); // not like lifetime tracking of iTJSDispatch2 instances is implemented... 
		}
	}
}

int tjsW_islightuserdata(const tjsW_state_t *state, int narg)
{
	tTJSVariant *variant;

	variant = tjsW_getvariant(state, narg);
	if (variant == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Argument not found"));
	}

	if (variant->Type() != tvtObject)
	{
		return 0;
	}

	iTJSDispatch2 *obj = variant->AsObjectNoAddRef();
	if (obj == nullptr)
	{
		return 0;
	}
	if (obj->IsInstanceOf(0, nullptr, &userdata_wrapper_hint, userdata_wrapper_classname, nullptr) != TJS_S_TRUE)
	{
		return 0;
	}

	return 1;
}

void *tjsW_touserdata(const tjsW_state_t *state, int narg)
{
	tTJSVariant *variant;

	variant = tjsW_getvariant(state, narg);
	if (variant == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Argument not found"));
	}

	if (variant->Type() != tvtObject)
	{
		TVPThrowExceptionMessage(TJS_W("Invalid variant type"));
	}

	iTJSDispatch2 *obj = variant->AsObjectNoAddRef();
	if (obj == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Object type is nullptr"));
	}
	if (obj->IsInstanceOf(0, nullptr, &userdata_wrapper_hint, userdata_wrapper_classname, nullptr) != TJS_S_TRUE)
	{
		TVPThrowExceptionMessage(TJS_W("Not userdata wrapper"));
	}

	iTJSDispatch2UserdataWrapper *obj_wrap = (iTJSDispatch2UserdataWrapper *)obj;

	return obj_wrap->get_userdata();
}
