--
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 34192;

DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryorguid` IN (-136578, -136579, -136580, -136581, -136582, -136583, -136584, -136585);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(-136578, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 136, 1, 1, 14286, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set movement MOVE_RUN speed to 1.14286'),
(-136579, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 136, 1, 1, 14286, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set movement MOVE_RUN speed to 1.14286'),
(-136580, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 136, 1, 1, 14286, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set movement MOVE_RUN speed to 1.14286'),
(-136581, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 136, 1, 1, 14286, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set movement MOVE_RUN speed to 1.14286'),
(-136582, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 136, 1, 1, 14286, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set movement MOVE_RUN speed to 1.14286'),
(-136583, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 136, 1, 1, 14286, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set movement MOVE_RUN speed to 1.14286'),
(-136584, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 136, 1, 1, 14286, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set movement MOVE_RUN speed to 1.14286'),
(-136585, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 136, 1, 1, 14286, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set movement MOVE_RUN speed to 1.14286');

 -- Kirin Tor Battle-Mage with guid 136525 smart ai
SET @ENTRY := -136525;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 33662;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 1, 0, 100, 1, 1000, 1000, 5000, 5000, 11, 48310, 0, 0, 0, 0, 0, 10, 137507, 33779, 0, 0, 0, 0, 0, 'Every 5 seconds (1s initially) (OOC) - Self: Cast spell  Transitus Shield Beam (48310) on Creature Ulduar Shield Bunny (33779) with guid 137507 (fetching)');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = -136525 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 1, -136525, 0, 0, 13, 1, 89, 0, 0, 0, 'instance data 89 equals 0');

 -- Kirin Tor Battle-Mage with guid 136528 smart ai
SET @ENTRY := -136528;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 33662;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 1, 0, 100, 0, 1000, 1000, 5000, 5000, 11, 48310, 0, 0, 0, 0, 0, 10, 137508, 33779, 0, 0, 0, 0, 0, 'Every 5 seconds (1s initially) (OOC) - Self: Cast spell  Transitus Shield Beam (48310) on Creature Ulduar Shield Bunny (33779) with guid 137508 (fetching)');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = -136528 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 1, -136528, 0, 0, 13, 1, 89, 0, 0, 0, 'instance data 89 equals 0');

--
DELETE FROM `creature_loot_template` WHERE `Entry` = 34003 AND `Item` IN (45086, 45110);
DELETE FROM `reference_loot_template` WHERE `Entry` = 34352 AND `Item` IN (45086, 45110);
INSERT INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(34352, 45086, 0, 0, 0, 8, 1, 1, 1, 'Flame Leviathan (25) - Rising Sun (3+ towers only)'),
(34352, 45110, 0, 0, 0, 8, 1, 1, 1, 'Flame Leviathan (25) - Titanguard (3+ towers only)');
