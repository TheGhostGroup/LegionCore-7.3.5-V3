-- The Mission - Alliance Pandaria

-- Conditions
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=25 AND `SourceEntry`=1066 AND `ConditionValue1`=29548;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(25, 0, 1066, 0, 0, 47, 0, 29548, 8, 0, 0, 0, '', 'Stormwind terrainswap if quest The Mission is in progress'),
(25, 0, 1066, 0, 0, 47, 0, 29548, 64, 0, 1, 0, '', 'Stormwind terrainswap if quest The Mission is NOT rewarded');

-- Remove Old Terrainswap Data
DELETE FROM `lc_phase_definitions` WHERE `TerrainSwapMap`=1066;
