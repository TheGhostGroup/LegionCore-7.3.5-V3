#ifndef DEF_BARADINHOLD_H
#define DEF_BARADINHOLD_H

#define DataHeader "BH"
#define BHScriptName "instance_baradin_hold"

#define MAX_ENCOUNTER 3

enum BHDataTypes
{
    // Encounters
    BOSS_ARGALOTH   = 0,
    BOSS_OCCUTHAR   = 1,
    BOSS_ALIZABAL   = 2,

    // Encounter Related
    DATA_EXTINUISH_FEL_FLAMES
};

enum Creatures
{
    NPC_ARGALOTH            = 47120,
    NPC_OCCUTHAR            = 52363,
    NPC_ALIZABAL            = 55869,
};

enum Gameobjects
{
    GO_TOLBARAD_DOOR_2  = 207619,
    GO_CELL_DOOR        = 208953,
    GO_TOLBARAD_DOOR_1  = 209849,
};

enum Data
{
    DATA_ARGALOTH   = 0,
    DATA_OCCUTHAR   = 1,
    DATA_ALIZABAL   = 2,   
};


template<class AI, class T>
inline AI* GetBaradinHoldAI(T* obj)
{
    return GetInstanceAI<AI>(obj, BHScriptName);
}

#define RegisterBaradinHoldCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetBaradinHoldAI)

#endif
