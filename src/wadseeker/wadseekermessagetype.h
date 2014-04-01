//------------------------------------------------------------------------------
// wadseekermessagetype.h
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __WADSEEKERMESSAGETYPE_H__
#define __WADSEEKERMESSAGETYPE_H__

namespace WadseekerLib
{
	/**
	 *	Wadseeker library uses a message system that contains
	 *	a type of message together with a string representing it's
	 *	content. Types should be treated as follows:
	 *
	 *	- Notice - this is just an information of what Wadseeker
	 *			 is currently doing
	 *	- NoticeImportant - an information on an important event
	 *	- Error - something bad happened but Wadseeker is able to continue
	 *			by itself.
	 *	- CriticalError - Wadseeker stops working after such error.
	 *			Programmer should return control to the main application.
	 */
	enum MessageType
	{
		Notice = 0,
		NoticeImportant = 1,
		Error = 2,
		CriticalError = 3
	};
};

#endif
