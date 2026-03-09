--
UPDATE `creature_model_info` SET `BoundingRadius` = 0.50915, `CombatReach` = 1.0625 WHERE `DisplayID` IN (4267,2568,4535);
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'EG_npc_plague_slime' WHERE `entry` = 16243;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 16243 AND `source_type` = 0;
UPDATE `creature_template` SET `ScriptName` = 'EG_npc_evolving_ectoplasm' WHERE `entry` = 3640;

-- Steel Gate Flying Machine
UPDATE `creature_template` SET `npcflag` = 0 WHERE `entry` = 24418;
UPDATE `creature` SET `unit_flags` = 33554432 WHERE `guid` = 104704;

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (38439));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 2, 38439, 0, 0, 31, 0, 3, 21648, 0, 0, 'Spell \'Placing Rocknail Flayer Carcass\' ImplicitTarget is \'Mature Netherwing Drake\''),
(13, 2, 38439, 0, 0, 1, 0, 38502, 0, 0, 1, 'Do Not cast \'Placing Rocknail Flayer Carcass\' on targets with aura \'Has Eaten Recently\'');

 -- Mature Netherwing Drake smart ai
SET @ENTRY := 21648;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2164800);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 8, 0, 100, 0, 38439, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Placing Rocknail Flayer Carcass (38439) hit - Set event phase to phase 1'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Placing Rocknail Flayer Carcass (38439) hit - Self: Pause path for 0 ms'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Placing Rocknail Flayer Carcass (38439) hit - Self: Set react state to Passive'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 1, 0, 0, 2, 0, 0, 20, 185155, 500, 0, 0, 0, 0, 0, 'On spell  Placing Rocknail Flayer Carcass (38439) hit - Self: Move to Closest gameobject Rocknail Flayer Carcass (185155) in 500 yards in 2 yd distance (point id 1)'),
(@ENTRY, 0, 4, 0, 34, 1, 100, 0, 8, 1, 0, 0, 80, 2164800, 2, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type POINT_MOTION_TYPE inform, point 1 - Self: Start timed action list id #Mature Netherwing Drake #0 (2164800) (update always) override existing // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 20, 185155, 10, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Look at Closest gameobject Rocknail Flayer Carcass (185155) in 10 yards'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 5, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Play emote ONESHOT_ATTACKUNARMED (35)'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 20, 185155, 10, 0, 0, 0, 0, 0, 'After 1 seconds - Closest gameobject Rocknail Flayer Carcass (185155) in 10 yards: Despawn instantly'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 38502, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Cast spell  Has Eaten Recently (38502) with flags triggered on Self'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Look at Last action invoker'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Talk 0 to invoker'),
(@ENTRY * 100, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 85, 38440, 2, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Last action invoker\'s party: Cast spell  Kill Credit: Feed Nether Drake (38440) with flags triggered on self'),
(@ENTRY * 100, 9, 7, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'After 4 seconds - Self: Evade'),
(@ENTRY, 0, 5, 0, 0, 2, 100, 0, 2000, 5000, 5000, 10000, 11, 38467, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 10 seconds (2 - 5s initially) (IC) - Self: Cast spell  Netherbreath (38467) on Victim'),
(@ENTRY, 0, 6, 0, 25, 0, 100, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Set event phase to default (0)'),
(@ENTRY, 0, 7, 8, 4, 0, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Set event phase to phase 2'),
(@ENTRY, 0, 8, 0, 61, 0, 100, 0, 0, 0, 0, 0, 8, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Set react state to Aggressive');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 21648 AND `SourceId` = 0;
