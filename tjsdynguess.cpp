/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2023-2023 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#include "tjsdyn_common.hpp"

class TjsDynGuess
{
public:
	static tjs_error TJS_INTF_METHOD method_dummy(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
		return TJS_S_OK;
	}
};

NCB_REGISTER_CLASS(TjsDynGuess)
{
	RawCallback("dummy", &Class::method_dummy, TJS_STATICMEMBER);
#if 0
	Variant("arch", DG_ARCH);
	Variant("os", DG_OS);
	Variant("cc", DG_CC);
#endif
};
