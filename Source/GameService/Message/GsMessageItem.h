#pragma once


class MessageItem
{
public:
	enum ItemAction
	{
		ADDITEM = 0,
		REMOVEITEM,
		UPDATEITEM,

		USEITEM,
		SELLITEM,
		ENCHANT,
		DISENCHANT_ITEM,

		Max
	};
};


