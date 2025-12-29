#include "CTxdStore.h"
#include "../deps/selector/asnew.hpp"

void *CTxdStore::fpAddTxdSlot;
void *CTxdStore::fpLoadTxd;
void *CTxdStore::fpAddRef;
void *CTxdStore::fpPushCurrentTxd;
void *CTxdStore::fpPopCurrentTxd;
void *CTxdStore::fpSetCurrentTxd;
void *CTxdStore::fpFindTxdSlot;
void *CTxdStore::fpRemoveTxdSlot;

int CTxdStore::AddTxdSlot(const char *name)
{
	return ((int(__cdecl *)(const char *))(fpAddTxdSlot))(name);
}

bool CTxdStore::LoadTxd(int slot, const char *filename)
{
	return ((bool(__cdecl *)(int, const char *))(fpLoadTxd))(slot, filename);
}

void CTxdStore::AddRef(int slot)
{
	((void(__cdecl *)(int))(fpAddRef))(slot);
}

int CTxdStore::PushCurrentTxd()
{
	return ((int(__cdecl *)())(fpPushCurrentTxd))();
}

void CTxdStore::PopCurrentTxd()
{
	((void(__cdecl *)())(fpPopCurrentTxd))();
}

void CTxdStore::SetCurrentTxd(int slot)
{
	((void(__cdecl *)(int))(fpSetCurrentTxd))(slot);
}

int CTxdStore::FindTxdSlot(const char *name)
{
	return ((int(__cdecl *)(const char *))(fpFindTxdSlot))(name);
}

void CTxdStore::RemoveTxdSlot(int slot)
{
	((void(__cdecl *)(int))(fpRemoveTxdSlot))(slot);
}

CTxdStore::CTxdStore()
{
    fpAddTxdSlot = addr_sel::lc::select_address({ 0x5274E0, 0x527720, 0x5276B0 });
	fpLoadTxd = addr_sel::lc::select_address({0x5276B0, 0x5278F0, 0x527880});
	fpAddRef = addr_sel::lc::select_address({0x527930, 0x527B70, 0x527B00});
	fpPushCurrentTxd = addr_sel::lc::select_address({0x527900, 0x527B40, 0x527AD0});
	fpSetCurrentTxd = addr_sel::lc::select_address({0x5278C0, 0x527B00, 0x527A90});
	fpPopCurrentTxd = addr_sel::lc::select_address({0x527910, 0x527B50, 0x527AE0});
	fpFindTxdSlot = addr_sel::lc::select_address({0x5275D0, 0x527810, 0x5277A0});
	fpRemoveTxdSlot = addr_sel::lc::select_address({0x527520, 0x5277D0, 0x5276F0});
}

static CTxdStore instance;
