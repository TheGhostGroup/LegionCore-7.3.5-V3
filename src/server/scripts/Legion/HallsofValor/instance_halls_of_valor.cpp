/*
    Dungeon : Halls of Valor 100-110
*/

#include "Group.h"
#include "halls_of_valor.h"
#include "ScriptPCH.h"
#include "WorldPacket.h"
#include "InstancePackets.h"

DoorData const doorData[] =
{
    {GO_HYMDALL_ENTER_DOOR,         DATA_HYMDALL,         DOOR_TYPE_ROOM,       BOUNDARY_NONE},
    {GO_HYMDALL_EXIT_DOOR,          DATA_HYMDALL,         DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_HYRJA_DOOR,                 DATA_HYRJA,           DOOR_TYPE_ROOM,       BOUNDARY_NW},
    {GO_GATES_OF_GLORY_DOOR,        DATA_HYRJA,           DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_GATES_OF_GLORY_DOOR,        DATA_FENRYR,          DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_ODYN_AND_SKOVALD_DOOR,      DATA_SKOVALD,         DOOR_TYPE_ROOM,       BOUNDARY_NONE},
    {GO_ODYN_AND_SKOVALD_DOOR,      DATA_ODYN,            DOOR_TYPE_ROOM,       BOUNDARY_NONE},
};

class instance_halls_of_valor : public InstanceMapScript
{
public:
    instance_halls_of_valor() : InstanceMapScript("instance_halls_of_valor", 1477) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_halls_of_valor_InstanceMapScript(map);
    }

    struct instance_halls_of_valor_InstanceMapScript : public InstanceScript
    {
        instance_halls_of_valor_InstanceMapScript(InstanceMap* map) : InstanceScript(map) {}

        WorldLocation loc_res_pla;

        ObjectGuid HymdallChestGUID;
        ObjectGuid hyrjaGUID;
        ObjectGuid feryrGUID;
        ObjectGuid skovaldGUID;
        ObjectGuid aegisGUID;
        ObjectGuid OdynGUID;
        ObjectGuid OdynChestGUID;
        ObjectGuid HymdallGUID;

        std::map<uint32, ObjectGuid> runicBrandGUIDconteiner;

        bool StartEvent;
        uint32 fenryrEventDone;
        uint32 skovaldEventDone;
        uint32 PlayerCount;
        uint32 GoRunicColour[5];
        uint32 checkTimer = 1000;
        uint32 checkTimerAura = 1000;

        void Initialize() override
        {
            SetHeaders(DataHeader);
            SetBossNumber(MAX_ENCOUNTER);
            LoadDoorData(doorData);

            StartEvent = false;
            PlayerCount = 0;
            fenryrEventDone = 0;
            skovaldEventDone = 0;
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case NPC_HYRJA:
                    hyrjaGUID = creature->GetGUID();
                    break;
                case NPC_BOSS_FENRYR:    
                    feryrGUID = creature->GetGUID();
                    break;
                case NPC_GOD_KING_SKOVALD:
                    skovaldGUID = creature->GetGUID();
                    break;
                case NPC_AEGIS_OF_AGGRAMAR:
                    aegisGUID = creature->GetGUID();
                    break;
                case NPC_ODYN:
                    OdynGUID = creature->GetGUID();
                    break;
                case NPC_HYMDALL:
                    HymdallGUID = creature->GetGUID();
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_HYMDALL_ENTER_DOOR:
                case GO_HYMDALL_EXIT_DOOR:
                case GO_HYRJA_DOOR:
                case GO_GATES_OF_GLORY_DOOR:
                case GO_ODYN_AND_SKOVALD_DOOR:
                    AddDoor(go, true);
                    break;
                case GO_HYMDALL_CHEST:
                    HymdallChestGUID = go->GetGUID();
                    break;
                case GO_ODYN_CHEST:
                    OdynChestGUID = go->GetGUID();
                    break;
                case GO_RUNIC_BRAND_PURE:
                    GoRunicColour[0] = go->GetEntry();
                    runicBrandGUIDconteiner[go->GetEntry()] = go->GetGUID();
                    break;
                case GO_RUNIC_BRAND_RED:
                    GoRunicColour[1] = go->GetEntry();
                    runicBrandGUIDconteiner[go->GetEntry()] = go->GetGUID();
                    break;
                case GO_RUNIC_BRAND_YELLOW:
                    GoRunicColour[2] = go->GetEntry();
                    runicBrandGUIDconteiner[go->GetEntry()] = go->GetGUID();
                    break;
                case GO_RUNIC_BRAND_BLUE:
                    GoRunicColour[3] = go->GetEntry();
                    runicBrandGUIDconteiner[go->GetEntry()] = go->GetGUID();
                    break;
                case GO_RUNIC_BRAND_GREEN:
                    GoRunicColour[4] = go->GetEntry();
                    runicBrandGUIDconteiner[go->GetEntry()] = go->GetGUID();
                    break;
                default:
                    break;
            }
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            switch (type)
            {
                case DATA_HYMDALL:
                    if (state == DONE)
                    {
                        if (instance->GetDifficultyID() != DIFFICULTY_MYTHIC_KEYSTONE)
                            if (GameObject* chest = instance->GetGameObject(HymdallChestGUID))
                                chest->SetRespawnTime(86400);

                        instance->ApplyOnEveryPlayer([&](Player* player)
                        {
                            if (auto hymdall = instance->GetCreature(HymdallGUID))
                            {
                                uint16 encounterId = sObjectMgr->GetDungeonEncounterByCreature(hymdall->GetEntry());
                                instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_HYMDALL, hymdall, player);
                                hymdall->GetMap()->SendToPlayers(WorldPackets::Instance::BossKillCredit(encounterId).Write());
                            }

                            instance->ToInstanceMap()->PermBindAllPlayers(player);
                            player->getHostileRefManager().deleteReferences();
                        });
                    }
                    break;
                case DATA_SKOVALD:
                {
                    if (state != IN_PROGRESS)
                        if (Creature* aegis = instance->GetCreature(aegisGUID))
                            aegis->DespawnOrUnsummon();
                    if (state == DONE)
                        if (Creature* odyn = instance->GetCreature(OdynGUID))
                            odyn->AI()->DoAction(true);
                    break;
                }
                case DATA_ODYN:
                {
                    if (state != IN_PROGRESS)
                        for (uint8 i = 0; i < 5; i++)
                            if (GameObject* rune = instance->GetGameObject(runicBrandGUIDconteiner[GoRunicColour[i]]))
                                rune->SetGoState(GO_STATE_READY);
                    if (state == DONE)
                    {
                        if (instance->GetDifficultyID() != DIFFICULTY_MYTHIC_KEYSTONE)
                            if (GameObject* chest = instance->GetGameObject(OdynChestGUID))
                                chest->SetRespawnTime(86400);

                        instance->SummonCreature(NPC_SPOILS_CHEST_VISUAL, spoilsPos);
                        instance->ApplyOnEveryPlayer([&](Player* player)
                        {
                            if (auto odyn = instance->GetCreature(OdynGUID))
                            {
                                uint16 encounterId = sObjectMgr->GetDungeonEncounterByCreature(odyn->GetEntry());
                                instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_ODYN, odyn, player);
                                odyn->GetMap()->SendToPlayers(WorldPackets::Instance::BossKillCredit(encounterId).Write());
                            }

                            instance->ToInstanceMap()->PermBindAllPlayers(player);
                            player->getHostileRefManager().deleteReferences();
                        });
                    }
                    break;
                }
                default:
                    break;
            }

            if (type == DATA_HYRJA || type == DATA_FENRYR)
                if (GetBossState(DATA_HYRJA) == DONE && GetBossState(DATA_FENRYR) == DONE)
                    DoCastSpellOnPlayers(202160); //Odyn's Blessing - Speed buff

            return true;
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_FENRYR_EVENT:
                {
                    fenryrEventDone = data;
                    if (data == DONE)
                    {
                        if (Creature* fen = instance->GetCreature(feryrGUID))
                        {
                            fen->SetVisible(true);
                            fen->SetReactState(REACT_AGGRESSIVE);
                        }
                        SaveToDB();
                    }
                    break;
                }
                case DATA_SKOVALD_EVENT:
                    skovaldEventDone = data;
                    if (data == DONE)
                        SaveToDB();
                    break;
                case DATA_RUNES_ACTIVATED:
                    if (GameObject* rune = instance->GetGameObject(runicBrandGUIDconteiner[GoRunicColour[data]]))
                        rune->SetGoState(GO_STATE_ACTIVE);
                    break;
                case DATA_RUNES_DEACTIVATED:
                    if (GameObject* rune = instance->GetGameObject(runicBrandGUIDconteiner[GoRunicColour[data]]))
                        rune->SetGoState(GO_STATE_READY);
                    if (Creature* odyn = instance->GetCreature(OdynGUID))
                        odyn->AI()->SetData(1, data);
                    break;
                default:
                    break;
            }
        }

        ObjectGuid GetGuidData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_HYRJA:
                    return hyrjaGUID;
                case DATA_SKOVALD:
                    return skovaldGUID;
                case DATA_ODYN:   
                    return OdynGUID;
            }
            return ObjectGuid::Empty;
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_FENRYR_EVENT:
                    return fenryrEventDone;
                case DATA_SKOVALD_EVENT:
                    return skovaldEventDone;
            }
            return 0;
        }

        void OnPlayerEnter(Player* player) override
        {
            if (!StartEvent)
            {
                if (PlayerCount < 5)
                {
                    PlayerCount++;
                } 
                else
                {
                    StartEvent = true;
                    if (Group *g = player->GetGroup())
                        if (Player* leader = ObjectAccessor::GetPlayer(*player, g->GetLeaderGUID()))
                            leader->CastSpell(leader, 202036);
                }
            }
        }

        void Update(uint32 diff) override
        {
            if (checkTimer <= diff)
            {
                checkTimer = 1000;

                instance->ApplyOnEveryPlayer([&](Player* player)
                {
                    if (player->GetCurrentAreaID() == 7672)
                    {
                        if (player->GetPositionZ() <= 600.00f)
                        {
                            player->Kill(player);
                            player->RepopAtGraveyard();
                        }
                    }
                });
            }
            else
                checkTimer -= diff;

            if (checkTimerAura <= diff)
            {
                checkTimerAura = 1000;

                instance->ApplyOnEveryPlayer([&](Player* player)
                {
                    if (player->GetCurrentAreaID() == 7672 && player->HasAura(192635))
                        if (player->GetPositionX() <= 2590.00f || player->GetPositionX() >= 3175.61f)
                            player->RemoveAura(192635);
                });
            }
            else
                checkTimerAura -= diff;
        }

        void WriteSaveDataMore(std::ostringstream& data) override
        {
            data << fenryrEventDone << " " << skovaldEventDone;
        }

        void ReadSaveDataMore(std::istringstream& data) override
        {
            data >> fenryrEventDone;
            data >> skovaldEventDone;
        }

        WorldLocation* GetClosestGraveYard(float x, float y, float z) override
        {
            loc_res_pla.WorldRelocate(1477, x, y, z);

            uint32 graveyardId = 5098;

            if (instance->GetDifficultyID() != DIFFICULTY_MYTHIC_KEYSTONE)
            {
                if (GetBossState(DATA_HYRJA) == DONE && GetBossState(DATA_FENRYR) == DONE)
                    graveyardId = 5289;
                else if (GetBossState(DATA_FENRYR) != DONE && GetData(DATA_FENRYR_EVENT) == DONE)
                    graveyardId = 5226;
                else if (GetBossState(DATA_HYRJA) == DONE)
                    graveyardId = 5225;
            }

            if (graveyardId)
            {
                if (WorldSafeLocsEntry const* gy = sWorldSafeLocsStore.LookupEntry(graveyardId))
                {
                    loc_res_pla.WorldRelocate(gy->MapID, gy->Loc.X, gy->Loc.Y, gy->Loc.Z);
                }
            }
            return &loc_res_pla;
        }

        /* void Update(uint32 diff) 
        {
            InstanceScript::Update(diff);
        } */
        
    };
};

void AddSC_instance_halls_of_valor()
{
    new instance_halls_of_valor();
}