/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#ifndef DEF_A_BROKEN_ISLANDS
#define DEF_A_BROKEN_ISLANDS

#define GOSSIP_ACCEPT_DUEL      "Let''s duel"
#define EVENT_SPECIAL           20

enum Spells
{
	SPELL_STAGE_1_PORT_ALLIANCE 		= 199358,
	SPELL_STAGE_1_PORT_HORDE    		= 225152,
	SPELL_LEAVE_BROKEN_SHORE_QUEUE      = 227058,
	SPELL_BROKEN_SHORE_CLIENT_SCENE     = 216356, // 1335
	SPELL_SWIMMER_TELEPORT              = 218611,
	SPELL_INTRO_CONVERSATION_1          = 180708, // 923
	SPELL_INTRO_CONVERSATION_2          = 199353, // 924
	SPELL_CANNONBALLS_SCENE             = 183341, // 1351
	SPELL_INTRO_SCENE                   = 199357, // 486
	
	SPELL_TELEPORT_TO_STORMWIND         = 208514,

    SPELL_BLACK_FADE                    = 240188
};

enum eDuelEnums
{
    SAY_DUEL_A = -1609080,
    SAY_DUEL_B = -1609081,
    SAY_DUEL_C = -1609082,
    SAY_DUEL_D = -1609083,
    SAY_DUEL_E = -1609084,
    SAY_DUEL_F = -1609085,
    SAY_DUEL_G = -1609086,
    SAY_DUEL_H = -1609087,
    SAY_DUEL_I = -1609088,

    SPELL_DUEL = 52996,
    //SPELL_DUEL_TRIGGERED        = 52990,
    SPELL_DUEL_VICTORY = 52994,
    SPELL_DUEL_FLAG = 52991,

    QUEST_42782 = 42782,
    QUEST_44281 = 44281,
    FACTION_HOSTILE = 2068
};

enum BrokenShoreIntro
{
    SPELL_LEAVE_FOR_BROKENSHORE_CLIENT_SCENE = 225147,
    SPELL_LEAVE_FOR_BROKENSHORE_QUEUE = 227058,
};

enum Data
{
    DATA_SCENARIO_TEAM = 0,
};

enum steps
{
    SCENARIO_STEP_8 = 7,
    SCENARION_STEP_9 = 8,
    SCENARION_STEP_END
};

enum eCreatures : uint32
{
    VIEWPOINT = 93219,
    STEP6_VEHICLE = 100959,
    NPC_KROSS = 90544,

    NPC_VARIAN = 90713, //0x202090B680589640001EFF000053DF33
    NPC_JAINA = 90714,
    NPC_TIRION = 91951,
    NPC_GULDAN = 94276,

    NPC_LORD_JAR = 105179,
    NPC_BRUTAL = 91902,
    NPC_TIHONDR = 90688,

    NPC_STAGE3_HORDE_FEL_COMMANDER = 93719,

    //Horde at stage 4 (others)
    NPC_HORDE_BAINE_BLOODHOOF = 90710,
    NPC_SYLVANA = 90709,
};

enum eGameObects
{
    GO_ALLIANCE_SHIP = 251604,
    GO_HORDE_SHIP = 255203,
    TRANSPORT_ALLIANCE = 251513,
    TRANSPORT_HORDE = 254124,
    //Spires of Woe destroyed
    GO_SPIRES_OF_WOE = 240194,
};

enum etc
{
	_DT_SCENARIA_TEAM                   = 0,
    _CIMEMATIC_ID                       = 999,

    _EVENT_TeleportAlliance             = 1,
    _EVENT_TeleportHorde                = 2,


    // Stage 2     
    TowerDestroyAlliance                = 44077,   // Criteria objective ==> [Criteria.db2].Asset
    TowerDestroyHorde                   = 54114,

    // Stage 3
    DefeatCommanderAlliance             = 45131,
    DefeatCommanderHorde                = 54109,

    // Stage 5
    DestroyPortalAlliance               = 45288,
    DestroyPortalHorde                  = 54141,

    // Stage 6
    BlackCityRazed_1p                   = 44384,   // stage progress Increase by 1%
    BlackCityRazed_5p                   = 53063,
    BlackCityRazed_10p                  = 53064,
};

#endif
