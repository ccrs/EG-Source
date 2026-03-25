 -- Ironhide smart ai
SET @ENTRY := 27715;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 9, 0, 100, 0, 8, 25, 17000, 24000, 11, 32323, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 8 - 25 yards (cooldown 17000 - 24000 ms) - Self: Cast spell  Charge (32323) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 7000, 7000, 9000, 11, 34298, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 7 - 9 seconds (5 - 7s initially) (IC) - Self: Cast spell  Maul (34298) on Victim'),
(@ENTRY, 0, 2, 0, 9, 0, 100, 0, 0, 5, 7000, 11000, 11, 31279, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 5 yards (cooldown 7000 - 11000 ms) - Self: Cast spell  Swipe (31279) on Victim'),
(@ENTRY, 0, 3, 0, 6, 0, 100, 0, 0, 0, 0, 0, 26, 12427, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On death - Killer: Call group event happened from quest The Conquest Pit: Bear Wrestling! (12427) on owner'),
(@ENTRY, 0, 4, 5, 7, 0, 100, 0, 0, 0, 0, 0, 6, 12427, 0, 0, 0, 0, 0, 18, 25, 0, 0, 0, 0, 0, 0, 'On evade - Players in 25 yards: Fail quest The Conquest Pit: Bear Wrestling! (12427)'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 27715 AND `SourceId` = 0;

 -- Torgg Thundertotem smart ai
SET @ENTRY := 27716;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 3000, 6000, 8000, 11, 16033, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 6 - 8 seconds (1 - 3s initially) (IC) - Self: Cast spell  Chain Lightning (16033) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 10000, 20000, 5000, 15000, 11, 15982, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 15 seconds (10 - 20s initially) (IC) - Self: Cast spell  Healing Wave (15982) on Self'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 0, 0, 21000, 21000, 11, 31991, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 21 seconds (0s initially) (IC) - Self: Cast spell  Corrupted Nova Totem (31991) on Self'),
(@ENTRY, 0, 3, 0, 0, 0, 100, 0, 4000, 4000, 12000, 12000, 11, 15501, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 12 seconds (4s initially) (IC) - Self: Cast spell  Earth Shock (15501) on Self'),
(@ENTRY, 0, 4, 0, 6, 0, 100, 0, 0, 0, 0, 0, 26, 12428, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On death - Killer: Call group event happened from quest The Conquest Pit: Mad Furbolg Fighting (12428) on owner'),
(@ENTRY, 0, 5, 6, 7, 0, 100, 0, 0, 0, 0, 0, 6, 12428, 0, 0, 0, 0, 0, 18, 25, 0, 0, 0, 0, 0, 0, 'On evade - Players in 25 yards: Fail quest The Conquest Pit: Mad Furbolg Fighting (12428)'),
(@ENTRY, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 27716 AND `SourceId` = 0;

 -- Corrupted Nova Totem smart ai
SET @ENTRY := 18179;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 60, 0, 100, 0, 3000, 3000, 5000, 5000, 11, 30941, 0, 6, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 5 seconds (3s initially) - Self: Cast spell  Fire Nova (30941) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 18179 AND `SourceId` = 0;
