#ifndef PASSTHROUGHAPP_PASSTHROUGHOBJECT_H
#define PASSTHROUGHAPP_PASSTHROUGHOBJECT_H

// {C38D254C-4C40-4192-A746-AC6FE519831E}
extern "C" const __declspec(selectany) IID IID_IPassthroughObject =
	{0xc38d254c, 0x4c40, 0x4192,
		{0xa7, 0x46, 0xac, 0x6f, 0xe5, 0x19, 0x83, 0x1e}};

struct
__declspec(uuid("{C38D254C-4C40-4192-A746-AC6FE519831E}"))
__declspec(novtable)
IPassthroughObject : public IUnknown
{
	STDMETHOD(SetTargetUnknown)(IUnknown* punkTarget) = 0;
};

#if _ATL_VER < 0x700
	#define InlineIsEqualGUID ::ATL::InlineIsEqualGUID
#else
	#define InlineIsEqualGUID ::InlineIsEqualGUID
#endif

#endif // PASSTHROUGHAPP_PASSTHROUGHOBJECT_H
