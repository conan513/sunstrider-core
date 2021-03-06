
#include "OutdoorPvPMgr.h"
#include "OutdoorPvPHP.h"
#include "OutdoorPvPNA.h"
#include "OutdoorPvPTF.h"
#include "OutdoorPvPZM.h"
#include "OutdoorPvPSI.h"
#include "OutdoorPvPEP.h"
#include "Player.h"

OutdoorPvPMgr::OutdoorPvPMgr()
{
    m_UpdateTimer = 0;
}

void OutdoorPvPMgr::Die()
{
    //TC_LOG_DEBUG("outdoorpvp", "Deleting OutdoorPvPMgr");
    for (auto & itr : m_OutdoorPvPSet)
        delete itr;

    m_OutdoorPvPSet.clear();

    /*
    for (OutdoorPvPDataMap::iterator itr = m_OutdoorPvPDatas.begin(); itr != m_OutdoorPvPDatas.end(); ++itr)
        delete itr->second;

    m_OutdoorPvPDatas.clear();*/

    m_OutdoorPvPMap.clear();
}

void OutdoorPvPMgr::InitOutdoorPvP()
{
    // create new opvp
    OutdoorPvP * pOP = new OutdoorPvPHP;
    // respawn, init variables
    if(!pOP->SetupOutdoorPvP())
    {
        TC_LOG_ERROR("misc", "OutdoorPvP : HP init failed.");
        delete pOP;
    }
    else
    {
        m_OutdoorPvPSet.push_back(pOP);
    }

    pOP = new OutdoorPvPNA;
    // respawn, init variables
    if(!pOP->SetupOutdoorPvP())
    {
        TC_LOG_ERROR("misc", "OutdoorPvP : NA init failed.");
        delete pOP;
    }
    else
    {
        m_OutdoorPvPSet.push_back(pOP);
    }

    pOP = new OutdoorPvPTF;
    // respawn, init variables
    if(!pOP->SetupOutdoorPvP())
    {
        TC_LOG_ERROR("misc", "OutdoorPvP : TF init failed.");
        delete pOP;
    }
    else
    {
        m_OutdoorPvPSet.push_back(pOP);
    }

    pOP = new OutdoorPvPZM;
    // respawn, init variables
    if(!pOP->SetupOutdoorPvP())
    {
        TC_LOG_ERROR("misc", "OutdoorPvP : ZM init failed.");
        delete pOP;
    }
    else
    {
        m_OutdoorPvPSet.push_back(pOP);
    }

    pOP = new OutdoorPvPSI;
    // respawn, init variables
    if(!pOP->SetupOutdoorPvP())
    {
        TC_LOG_ERROR("misc", "OutdoorPvP : SI init failed.");
        delete pOP;
    }
    else
    {
        m_OutdoorPvPSet.push_back(pOP);
    }

    pOP = new OutdoorPvPEP;
    // respawn, init variables
    if(!pOP->SetupOutdoorPvP())
    {
        TC_LOG_ERROR("misc", "OutdoorPvP : EP init failed.");
        delete pOP;
    }
    else
    {
        m_OutdoorPvPSet.push_back(pOP);
    }
}

void OutdoorPvPMgr::AddZone(uint32 zoneid, OutdoorPvP *handle)
{
    m_OutdoorPvPMap[zoneid] = handle;
}

ZoneScript* OutdoorPvPMgr::GetZoneScript(uint32 zoneId)
{
    OutdoorPvPMap::iterator itr = m_OutdoorPvPMap.find(zoneId);
    if (itr != m_OutdoorPvPMap.end())
        return itr->second;
    else
        return nullptr;
}

void OutdoorPvPMgr::HandlePlayerEnterZone(Player *plr, uint32 zoneid)
{
    auto itr = m_OutdoorPvPMap.find(zoneid);
    if(itr == m_OutdoorPvPMap.end())
    {
        // no handle for this zone, return
        return;
    }
    // add possibly beneficial buffs to plr for zone
    itr->second->HandlePlayerEnterZone(plr, zoneid);
}

void OutdoorPvPMgr::HandlePlayerLeaveZone(Player *plr, uint32 zoneid)
{
    auto itr = m_OutdoorPvPMap.find(zoneid);
    if(itr == m_OutdoorPvPMap.end())
    {
        // no handle for this zone, return
        return;
    }
    // inform the OutdoorPvP class of the leaving, it should remove the player from all objectives
    itr->second->HandlePlayerLeaveZone(plr, zoneid);
}

void OutdoorPvPMgr::HandlePlayerResurrects(Player* player, uint32 zoneid)
{
    auto itr = m_OutdoorPvPMap.find(zoneid);
    if (itr == m_OutdoorPvPMap.end())
        return;

    if (itr->second->HasPlayer(player))
        itr->second->HandlePlayerResurrects(player, zoneid);
}

OutdoorPvP * OutdoorPvPMgr::GetOutdoorPvPToZoneId(uint32 zoneid)
{
    auto itr = m_OutdoorPvPMap.find(zoneid);
    if(itr == m_OutdoorPvPMap.end())
    {
        // no handle for this zone, return
        return nullptr;
    }
    return itr->second;
}

void OutdoorPvPMgr::Update(uint32 diff)
{
    if(m_UpdateTimer < diff)
    {
        for(auto & itr : m_OutdoorPvPSet)
        {
            itr->Update(diff);
        }
        m_UpdateTimer = OUTDOORPVP_OBJECTIVE_UPDATE_INTERVAL;
    } else m_UpdateTimer -= diff;
}

bool OutdoorPvPMgr::HandleCustomSpell(Player *plr, uint32 spellId, GameObject * go)
{
    for(auto & itr : m_OutdoorPvPSet)
    {
        if(itr->HandleCustomSpell(plr,spellId,go))
            return true;
    }
    return false;
}

bool OutdoorPvPMgr::HandleOpenGo(Player *plr, ObjectGuid guid)
{
    for(auto & itr : m_OutdoorPvPSet)
    {
        if(itr->HandleOpenGo(plr,guid))
            return true;
    }
    return false;
}

void OutdoorPvPMgr::HandleGossipOption(Player *plr, ObjectGuid guid, uint32 gossipid)
{
    for(auto & itr : m_OutdoorPvPSet)
    {
        if(itr->HandleGossipOption(plr,guid,gossipid))
            return;
    }
}

bool OutdoorPvPMgr::CanTalkTo(Player * plr, Creature * c, GossipMenuItems const& gso)
{
    for(auto & itr : m_OutdoorPvPSet)
    {
        if(itr->CanTalkTo(plr,c,gso))
            return true;
    }
    return false;
}

void OutdoorPvPMgr::HandleDropFlag(Player *plr, uint32 spellId)
{
    for(auto & itr : m_OutdoorPvPSet)
    {
        if(itr->HandleDropFlag(plr,spellId))
            return;
    }
}

