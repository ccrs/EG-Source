 -- Warsong Hold Warrior smart ai
SET @ENTRY := 25414;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2541401);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 37, 0, 100, 0, 0, 0, 0, 0, 80, 2541400, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On AI initialize - Self: Start timed action list id #Warsong Hold Warrior #0 (2541400) (update out of combat)'),
(@ENTRY, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 45517, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Cast spell  Commanding Shout (45517) on Self'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 0, 5000, 5000, 10000, 11, 15284, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 10 seconds (0 - 5s initially) (IC) - Self: Cast spell  Cleave (15284) on Victim'),
(@ENTRY, 0, 3, 0, 59, 0, 100, 0, 1, 0, 0, 0, 80, 2541401, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On timed event timedEvent[1] triggered - Self: Start timed action list id #Warsong Hold Warrior #1 (2541401) (update always) // -inline'),
(@ENTRY * 100 + 1, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set react state to Passive'),
(@ENTRY * 100 + 1, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Move to Self (point id 1)'),
(@ENTRY * 100 + 1, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 43014, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Cast spell  Despawn Self (43014) on Self'),
(@ENTRY * 100 + 1, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 1 to invoker'),
(@ENTRY * 100 + 1, 9, 4, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 89, 10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Move randomly in radius 10 yards'),
(@ENTRY * 100 + 1, 9, 5, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 11, 45522, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 7 seconds - Self: Cast spell  Dispel Freed Soldier Debuff (45522) on Owner/Summoner'),
(@ENTRY * 100 + 1, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 4, 0, 6, 0, 100, 0, 0, 0, 0, 0, 11, 45522, 2, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Cast spell  Dispel Freed Soldier Debuff (45522) with flags triggered on Owner/Summoner'),
(@ENTRY, 0, 5, 6, 11, 0, 100, 0, 0, 0, 0, 0, 89, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Stay in place'),
(@ENTRY, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 67, 1, 100000, 100000, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Trigger timed event timedEvent[1] in 100000 - 100000 ms with 100% chance');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 25414 AND `SourceId` = 0;

 -- Timed list 2541400 smart ai
SET @ENTRY := 2541400;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Look at Owner/Summoner'),
(@ENTRY, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Owner/Summoner: Talk 0 to invoker'),
(@ENTRY, 9, 2, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 29, 5, 1, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 4 seconds - Self: Follow Owner/Summoner by distance 5, angle 1');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 2541400 AND `SourceId` = 9;

 -- Warsong Peon smart ai
SET @ENTRY := 25270;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2527000);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 80, 2527000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Start timed action list id #Warsong Peon #0 (2527000) (update out of combat) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 89, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Stay in place'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Look at Owner/Summoner'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 33, 25270, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Owner/Summoner: Give kill credit Warsong Peon (25270)'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Owner/Summoner: Talk 0 to invoker'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 29, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Follow Owner/Summoner by distance 0, angle 0'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 41, 4000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Despawn in 4 s');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 25270 AND `SourceId` = 0;

 -- Nerub'ar Victim smart ai
SET @ENTRY := 25284;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 37, 0, 100, 0, 0, 0, 0, 0, 116, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On AI initialize - Self: Sets corpse delay to 1 s'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On AI initialize - Self: Set react state to Passive'),
(@ENTRY, 0, 2, 0, 6, 0, 100, 0, 0, 0, 0, 0, 11, 45516, 2, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Cast spell  Nerub\'ar Web Random Unit (45516) with flags triggered on Killer'),
(@ENTRY, 0, 3, 0, 6, 0, 100, 0, 0, 0, 0, 0, 11, 45534, 2, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Cast spell  Nerub\'ar Web Random Unit (45534) with flags triggered on Killer');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 25284 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 3, 25284, 0, 0, 9, 0, 11611, 0, 0, 1, 'Action invoker doesn\'t have quest Taken by the Scourge (11611) active'),
(22, 3, 25284, 0, 0, 32, 0, 16, 0, 0, 0, 'Action invoker is of type player'),
(22, 4, 25284, 0, 0, 9, 0, 11611, 0, 0, 0, 'Action invoker has quest Taken by the Scourge (11611) active'),
(22, 4, 25284, 0, 0, 32, 0, 16, 0, 0, 0, 'Action invoker is of type player');

 -- Crystalline Frayer smart ai
SET @ENTRY := 26793;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'EG_npc_crystalline_frayer' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;

DELETE FROM `spell_script_names` WHERE `spell_id` IN (605);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(605, 'EG_spell_priest_mind_control');
