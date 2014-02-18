//------------------------------------------------------------------------------
// masterclientsignalproxy.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "masterclientsignalproxy.h"

#include "masterclient.h"

MasterClientSignalProxy::MasterClientSignalProxy(MasterClient* pMaster)
{
	this->pMaster = pMaster;

	connect(pMaster, SIGNAL( listUpdated() ),
		this, SLOT( listUpdatedSlot() ) );

	connect(pMaster, SIGNAL( message(const QString&, const QString&, bool) ),
		this, SLOT( readMasterMessage(const QString&, const QString&, bool) ) );

	connect(pMaster, SIGNAL( messageImportant(const Message&) ),
		this, SLOT( readMasterMessageImportant(const Message&) ) );
}

