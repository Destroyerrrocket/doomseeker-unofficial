//------------------------------------------------------------------------------
// qtmetapointer.h
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __QTMETAPOINTER_H__
#define __QTMETAPOINTER_H__

#include <Qt>

class QtMetaPointer
{
	private:
		void copy(const QtMetaPointer& copyin)
		{
			ptr = copyin.ptr;
		}

		void* ptr;

	public:
		QtMetaPointer() {}
		QtMetaPointer(void* ptr)
		{
			this->ptr = ptr;
		}

		QtMetaPointer(const QtMetaPointer& copyin)
		{
			copy(copyin);
		}

		QtMetaPointer& operator=(const QtMetaPointer& rhs)
		{
			if (this != &rhs)
			{
				copy(rhs);
			}

			return *this;
		}

		QtMetaPointer& operator=(void* rhs)
		{
			this->ptr = rhs;
			return *this;
		}

		~QtMetaPointer() {}

		friend bool operator==(const void* fPtr, const QtMetaPointer& ref)
		{
			return (fPtr == ref.ptr);
		}

		void* operator->()
		{
			return ptr;
		}

		operator void* () { return ptr; }
		operator const void* () const { return ptr; }
};

Q_DECLARE_METATYPE(QtMetaPointer)

#endif
