/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Vehicle.h"
#include "CombatAI.h"
#include "Player.h"
#include "WorldSession.h"

/*######
## npc_frostborn_scout
######*/

#define GOSSIP_ITEM1    "Are you okay? I've come to take you back to Frosthold if you can stand."
#define GOSSIP_ITEM2    "I'm sorry that I didn't get here sooner. What happened?"
#define GOSSIP_ITEM3    "I'll go get some help. Hang in there."

enum eFrostbornScout
{
    QUEST_MISSING_SCOUTS  =  12864
};

class npc_frostborn_scout : public CreatureScript
{
public:
    npc_frostborn_scout() : CreatureScript("npc_frostborn_scout") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {

        if (player->GetQuestStatus(QUEST_MISSING_SCOUTS) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GossipOptionNpc::None, GOSSIP_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->PlayerTalkClass->SendGossipMenu(13611, creature->GetGUID());
        }

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM(GossipOptionNpc::None, GOSSIP_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->PlayerTalkClass->SendGossipMenu(13612, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(GossipOptionNpc::None, GOSSIP_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->PlayerTalkClass->SendGossipMenu(13613, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->PlayerTalkClass->SendGossipMenu(13614, creature->GetGUID());
            player->AreaExploredOrEventHappens(QUEST_MISSING_SCOUTS);
            break;
        }

        return true;
    }
};


/////////////////////
///npc_injured_goblin
/////////////////////

enum eInjuredGoblin
{
    QUEST_BITTER_DEPARTURE     = 12832,
    SAY_QUEST_ACCEPT           =  -1800042,
    SAY_END_WP_REACHED         =  -1800043
};

#define GOSSIP_ITEM_1       "I am ready, lets get you out of here"

class npc_injured_goblin : public CreatureScript
{
public:
    npc_injured_goblin() : CreatureScript("npc_injured_goblin") { }

    struct npc_injured_goblinAI : public npc_escortAI
    {
        npc_injured_goblinAI(Creature* creature) : npc_escortAI(creature) { }

        void WaypointReached(uint32 waypointId) override
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            switch (waypointId)
            {
                case 26:
                    DoScriptText(SAY_END_WP_REACHED, me, player);
                    break;
                case 27:
                    player->GroupEventHappens(QUEST_BITTER_DEPARTURE, me);
                    break;
            }
        }

        void EnterCombat(Unit* /*who*/) override {}

        void Reset() override {}

        void JustDied(Unit* /*killer*/) override
        {
            Player* player = GetPlayerForEscort();
            if (HasEscortState(STATE_ESCORT_ESCORTING) && player)
                player->FailQuest(QUEST_BITTER_DEPARTURE);
        }

       void UpdateAI(uint32 uiDiff) override
       {
            npc_escortAI::UpdateAI(uiDiff);
            if (!UpdateVictim())
                return;
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_injured_goblinAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_BITTER_DEPARTURE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GossipOptionNpc::None, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->PlayerTalkClass->SendGossipMenu(9999999, creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(999999, creature->GetGUID());
        return true;
    }

    bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_BITTER_DEPARTURE)
            DoScriptText(SAY_QUEST_ACCEPT, creature);

        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        npc_escortAI* pEscortAI = CAST_AI(npc_injured_goblin::npc_injured_goblinAI, creature->AI());

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            pEscortAI->Start(true, true, player->GetGUID());
            creature->setFaction(113);
        }
        return true;
    }
};

/*######
## npc_roxi_ramrocket
######*/

#define SPELL_MECHANO_HOG           60866
#define SPELL_MEKGINEERS_CHOPPER    60867

class npc_roxi_ramrocket : public CreatureScript
{
public:
    npc_roxi_ramrocket() : CreatureScript("npc_roxi_ramrocket") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        //Quest Menu
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        //Trainer Menu
        if ( creature->isTrainer() )
            player->ADD_GOSSIP_ITEM(GossipOptionNpc::Trainer, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

        //Vendor Menu
        if ( creature->isVendor() )
            if (player->HasSpell(SPELL_MECHANO_HOG) || player->HasSpell(SPELL_MEKGINEERS_CHOPPER))
                player->ADD_GOSSIP_ITEM(GossipOptionNpc::Vendor, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_TRAIN:
            player->GetSession()->SendTrainerList(creature->GetGUID());
            break;
        case GOSSIP_ACTION_TRADE:
            player->GetSession()->SendListInventory(creature->GetGUID());
            break;
        }
        return true;
    }
};

/*######
## npc_brunnhildar_prisoner
######*/

enum brunhildar {
    NPC_QUEST_GIVER            = 29592,

    SPELL_ICE_PRISON           = 54894,
    SPELL_KILL_CREDIT_PRISONER = 55144,
    SPELL_KILL_CREDIT_DRAKE    = 55143,
    SPELL_SUMMON_LIBERATED     = 55073,
    SPELL_ICE_LANCE            = 55046
};

class npc_brunnhildar_prisoner : public CreatureScript
{
public:
    npc_brunnhildar_prisoner() : CreatureScript("npc_brunnhildar_prisoner") { }

    struct npc_brunnhildar_prisonerAI : public ScriptedAI
    {
        npc_brunnhildar_prisonerAI(Creature* creature) : ScriptedAI(creature) {}

        ObjectGuid drakeGUID;
        uint16 enter_timer;
        bool hasEmptySeats;

        void Reset() override
        {
            me->CastSpell(me, SPELL_ICE_PRISON, true);
            enter_timer = 0;
            drakeGUID.Clear();
            hasEmptySeats = false;
        }

        void UpdateAI(uint32 diff) override
        {
            //TODO: not good script
            if (!drakeGUID)
                return;

            Creature* drake = Unit::GetCreature(*me, drakeGUID);
            if (!drake)
            {
                drakeGUID.Clear();
                return;
            }

            // drake unsummoned, passengers dropped
            if (!me->IsOnVehicle(drake) && !hasEmptySeats)
                me->DespawnOrUnsummon(3000);

            if (enter_timer <= 0)
                return;

            if (enter_timer < diff)
            {
                enter_timer = 0;
                if (hasEmptySeats)
                    me->JumpTo(drake, 25.0f);
                else
                    Reset();
            }
            else
                enter_timer -= diff;
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (!who || !drakeGUID)
                return;

            Creature* drake = Unit::GetCreature(*me, drakeGUID);
            if (!drake)
            {
                drakeGUID.Clear();
                return;
            }

            if (!me->IsOnVehicle(drake) && !me->HasAura(SPELL_ICE_PRISON))
            {
                if (who->IsVehicle() && me->IsWithinDist(who, 25.0f, true) && who->ToCreature() && who->ToCreature()->GetEntry() == 29709)
                {
                    SeatMap::const_iterator _Seat = who->GetVehicleKit()->GetNextEmptySeat(0, true); 
                    uint8 seat = _Seat->first;
                    if (seat <= 0)
                        return;

                    me->EnterVehicle(who, seat);
                    hasEmptySeats = false;
                }
            }

            if (who->ToCreature() && me->IsOnVehicle(drake))
            {
                if (who->ToCreature()->GetEntry() == NPC_QUEST_GIVER && me->IsWithinDist(who, 15.0f, false))
                {
                    Unit* rider = drake->GetVehicleKit()->GetPassenger(0);
                    if (!rider)
                        return;

                    rider->CastSpell(rider, SPELL_KILL_CREDIT_PRISONER, true);

                    me->ExitVehicle();
                    me->CastSpell(me, SPELL_SUMMON_LIBERATED, true);
                    me->DespawnOrUnsummon(500);

                    // drake is empty now, deliver credit for drake and despawn him
                    if (drake->GetVehicleKit()->HasEmptySeat(1) &&
                        drake->GetVehicleKit()->HasEmptySeat(2) &&
                        drake->GetVehicleKit()->HasEmptySeat(3))
                    {
                        // not working rider->CastSpell(rider, SPELL_KILL_CREDIT_DRAKE, true);
                        if (rider->ToPlayer())
                            rider->ToPlayer()->KilledMonsterCredit(29709, ObjectGuid::Empty);

                        drake->DespawnOrUnsummon(0);
                    }
                }
            }
        }

        void SpellHit(Unit* hitter, const SpellInfo* spell) override
        {
            if (!hitter || !spell)
                return;

            if (spell->Id != SPELL_ICE_LANCE)
                return;

            me->RemoveAura(SPELL_ICE_PRISON);
            enter_timer = 500;

            if (hitter->IsVehicle())
                drakeGUID = hitter->GetGUID();
            else
                return;

            SeatMap::const_iterator _Seat = hitter->GetVehicleKit()->GetNextEmptySeat(0, true);
            if (_Seat != hitter->GetVehicleKit()->Seats.end())
                hasEmptySeats = true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_brunnhildar_prisonerAI(creature);
    }
};

/*######
## npc_freed_protodrake
######*/

enum FreedProtoDrake
{
    AREA_VALLEY_OF_ANCIENT_WINTERS      = 4437,
    TEXT_EMOTE                          = 0
};

const Position FreedDrakeWaypoints[5] =
{
    { 7332.343f, -2359.76f,  814.363f, 0.f},
    { 7154.399f, -2206.755f, 821.177f, 0.f},
    { 7118.382f, -2067.392f, 822.963f, 0.f},
    { 7047.499f, -1853.361f, 849.67f, 0.f},
    { 7059.121f, -1730.347f, 825.019f, 0.f},
};


class npc_freed_protodrake : public CreatureScript
{
public:
    npc_freed_protodrake() : CreatureScript("npc_freed_protodrake") { }

    struct npc_freed_protodrakeAI : public VehicleAI
    {
        npc_freed_protodrakeAI(Creature* creature) : VehicleAI(creature) {}

        bool autoMove;
        bool wpstart;
        uint16 CheckTimer;

        void Reset() override
        {
            autoMove = false;
            wpstart = false;
            CheckTimer = 5000;
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            switch (id)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                me->AddDelayedEvent(100, [this, id] {
                    me->GetMotionMaster()->MovePoint(id + 1, FreedDrakeWaypoints[id + 1]);
                });
                break;
            case 4:
            {
                auto unit = me->GetVehicleKit()->GetPassenger(0);
                if (unit && unit->GetTypeId() == TYPEID_PLAYER)
                {
                    for (uint8 i = 1; i < 4; ++i)
                        if (Unit* prisoner = me->GetVehicleKit()->GetPassenger(i))
                        {
                            if (prisoner->GetTypeId() != TYPEID_UNIT)
                                return;
                            prisoner->CastSpell(unit, SPELL_KILL_CREDIT_PRISONER, true);
                            prisoner->CastSpell(prisoner, SPELL_SUMMON_LIBERATED, true);
                            prisoner->ExitVehicle();
                        }
                    me->CastSpell(me, SPELL_KILL_CREDIT_DRAKE, true);
                    unit->ExitVehicle();
                }
                break;
            }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!autoMove)
            {
                if (CheckTimer < diff)
                {
                    CheckTimer = 3000;
                    if (me->GetAreaId() == AREA_VALLEY_OF_ANCIENT_WINTERS)
                    {
                        Talk(TEXT_EMOTE, me->GetVehicleKit()->GetPassenger(0)->GetGUID());
                        autoMove = true;
                        if (!wpstart && autoMove)
                        {
                            wpstart = true;
                            me->GetMotionMaster()->MovePoint(0, FreedDrakeWaypoints[0]);
                        }
                    }
                }
                else
                    CheckTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_freed_protodrakeAI(creature);
    }
};

class npc_icefang : public CreatureScript
{
public:
    npc_icefang() : CreatureScript("npc_icefang") { }

    struct npc_icefangAI : public npc_escortAI
    {
        npc_icefangAI(Creature* creature) : npc_escortAI(creature) {}

        void AttackStart(Unit* /*who*/) override {}
        void EnterCombat(Unit* /*who*/) override {}
        void EnterEvadeMode() override {}

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
            {
                if (apply)
                    Start(false, true, who->GetGUID());
            }
        }

        void WaypointReached(uint32 /*waypointId*/) override
        {
        }

        void JustDied(Unit* /*killer*/) override
        {
        }

        void OnCharmed(bool /*apply*/) override
        {
        }

        void UpdateAI(uint32 diff) override
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_icefangAI (creature);
    }
};

class npc_hyldsmeet_protodrake : public CreatureScript
{
    enum NPCs
    {
        NPC_HYLDSMEET_DRAKERIDER = 29694
    };

    public:
        npc_hyldsmeet_protodrake() : CreatureScript("npc_hyldsmeet_protodrake") { }

        class npc_hyldsmeet_protodrakeAI : public CreatureAI
        {
            public:
                npc_hyldsmeet_protodrakeAI(Creature* creature) : CreatureAI(creature), _accessoryRespawnTimer(0), _vehicleKit(creature->GetVehicleKit()) {}

                void PassengerBoarded(Unit* who, int8 /*seat*/, bool apply) override
                {
                    if (apply)
                        return;

                    if (who->GetEntry() == NPC_HYLDSMEET_DRAKERIDER)
                        _accessoryRespawnTimer = 5 * MINUTE * IN_MILLISECONDS;
                }

                void UpdateAI(uint32 diff) override
                {
                    //! We need to manually reinstall accessories because the vehicle itself is friendly to players,
                    //! so EnterEvadeMode is never triggered. The accessory on the other hand is hostile and killable.
                    if (_accessoryRespawnTimer && _accessoryRespawnTimer <= diff && _vehicleKit)
                    {
                        _vehicleKit->InstallAllAccessories(true);
                        _accessoryRespawnTimer = 0;
                    }
                    else
                        _accessoryRespawnTimer -= diff;
                }

            private:
                uint32 _accessoryRespawnTimer;
                Vehicle* _vehicleKit;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hyldsmeet_protodrakeAI (creature);
        }
};

enum CloseRift
{
    SPELL_DESPAWN_RIFT          = 61665
};

class spell_close_rift : public SpellScriptLoader
{
    public:
        spell_close_rift() : SpellScriptLoader("spell_close_rift") { }

        class spell_close_rift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_close_rift_AuraScript);

            bool Load() override
            {
                _counter = 0;
                return true;
            }

            bool Validate(SpellInfo const* /*spell*/) override
            {
                return sSpellMgr->GetSpellInfo(SPELL_DESPAWN_RIFT);
            }

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                if (++_counter == 5)
                    GetTarget()->CastSpell((Unit*)NULL, SPELL_DESPAWN_RIFT, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_close_rift_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

        private:
            uint8 _counter;

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_close_rift_AuraScript();
        }
};

void AddSC_storm_peaks()
{
    new npc_frostborn_scout();
    //new npc_injured_goblin();
    new npc_roxi_ramrocket();
    new npc_brunnhildar_prisoner();
    new npc_freed_protodrake();
    new npc_icefang();
    new npc_hyldsmeet_protodrake();
    new spell_close_rift();
}
