--
-- Winter Rumbler
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '', `mechanic_immune_mask` = 550189951 WHERE `entry` = 34135;
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` = 34142;

SET @ENTRY := 34135;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 3000, 6000, 8000, 12000, 11, 64645, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 8 - 12 seconds (3 - 6s initially) (IC) - Self: Cast spell  Cone of Cold (64645) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 1000, 4000, 10000, 15000, 11, 64647, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 'Every 10 - 15 seconds (1 - 4s initially) (IC) - Self: Cast spell  Snow Blindness (64647) on Random hostile (not top)');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 34135 AND `SourceId` = 0;

DELETE FROM `spelldifficulty_dbc` WHERE `id` IN (64645, 64647);
INSERT INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`, `spellid2`, `spellid3`) VALUES
(64645, 64645, 64655, 0, 0),
(64647, 64647, 64654, 0, 0);

-- Winter Revenant
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '', `mechanic_immune_mask` = 550189951 WHERE `entry` = 34134;
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` = 34141;

DELETE FROM `creature_template_addon` WHERE `entry` IN (34134, 34141);
INSERT INTO `creature_template_addon` (`entry`, `auras`) VALUES
(34134, '64644'),
(34141, '64644');
UPDATE `creature_addon` SET `auras` = '64644' WHERE `guid` IN (136272, 136273, 136282);

SET @ENTRY := 34134;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 3000, 6000, 12000, 18000, 11, 64642, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 12 - 18 seconds (3 - 6s initially) (IC) - Self: Cast spell  Blizzard (64642) on Random hostile'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 8000, 9000, 14000, 11, 64643, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 9 - 14 seconds (5 - 8s initially) (IC) - Self: Cast spell  Whirling Strike (64643) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 34134 AND `SourceId` = 0;

DELETE FROM `spelldifficulty_dbc` WHERE `id` = 64642;
INSERT INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`, `spellid2`, `spellid3`) VALUES
(64642, 64642, 64653, 0, 0);
