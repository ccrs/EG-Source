--
UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 192, `modelid1` = 27401 WHERE `entry` = 30898;
UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 192, `modelid1` = 16925 WHERE `entry` = 30899;
UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 192, `modelid1` = 27401 WHERE `entry` = 30897;

DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (30898, 30899, 30897);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`) VALUES
(30898, 1, 0, 1, 1),
(30899, 1, 0, 1, 1),
(30897, 1, 0, 1, 1);

UPDATE `creature_template` SET `minlevel` = 77, `maxlevel` = 77, `exp` = 2 WHERE `entry` = 28237;
UPDATE `creature_template` SET `minlevel` = 81, `exp` = 2 WHERE `entry` = 31875;

 -- Dark Matter smart ai
SET @ENTRY := 28235;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 72, 0, 100, 0, 5, 0, 0, 0, 11, 51003, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On action[5] - Self: Cast spell  Summon Dark Matter Target (51003) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28235 AND `SourceId` = 0;

 -- Dark Matter Target smart ai
SET @ENTRY := 28237;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 11, 51012, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  Dark Matter (51012) on Self'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 51001, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  Dark Matter (51001) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28237 AND `SourceId` = 0;

UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 192, `modelid1` = 11686 WHERE `entry` = 28237;
UPDATE `creature_template_movement` SET `Ground` = 1, `Swim` = 0 WHERE `CreatureId` = 28237;
UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 192, `modelid1` = 11686 WHERE `entry` = 31875;
UPDATE `creature_template_movement` SET `Ground` = 1, `Swim` = 0 WHERE `CreatureId` = 31875;

 -- Searing Gaze smart ai
SET @ENTRY := 28265;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 25, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set react state to Passive'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 117, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Disable evade'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set walk'),
(@ENTRY, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 89, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Move randomly in radius 5 yards'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 51136, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Cast spell  Searing Gaze (51136) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28265 AND `SourceId` = 0;

UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 8384, `modelid1` = 11686 WHERE `entry` = 28265;
UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 8384, `modelid1` = 11686 WHERE `entry` = 31878;
DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (28265, 31878);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`) VALUES
(28265, 1, 0, 0, 0),
(31878, 1, 0, 0, 0);

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (50988));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 50988, 0, 0, 31, 0, 4, 0, 0, 0, 'Potential target of the spell is player'),
(13, 1, 50988, 0, 0, 36, 0, 0, 0, 0, 0, 'Potential target of the spell is alive');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (51003));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 51003, 0, 0, 31, 0, 4, 0, 0, 0, 'Potential target of the spell is player'),
(13, 1, 51003, 0, 0, 36, 0, 0, 0, 0, 0, 'Potential target of the spell is alive');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (51146));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 51146, 0, 0, 31, 0, 4, 0, 0, 0, 'Potential target of the spell is player'),
(13, 1, 51146, 0, 0, 36, 0, 0, 0, 0, 0, 'Potential target of the spell is alive');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (59870));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 59870, 0, 0, 31, 0, 4, 0, 0, 0, 'Potential target of the spell is player'),
(13, 1, 59870, 0, 0, 36, 0, 0, 0, 0, 0, 'Potential target of the spell is alive');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (51001));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 51001, 0, 0, 31, 0, 3, 28235, 0, 0, 'Potential target of the spell is creature, entry is Dark Matter (28235)');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (51288));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 51288, 0, 0, 31, 0, 3, 28234, 0, 0, 'Potential target of the spell is creature, entry is Tribunal of the Ages (28234)'),
(13, 1, 51288, 0, 1, 31, 0, 3, 28237, 0, 0, 'Potential target of the spell is creature, entry is Dark Matter Target (28237)'),
(13, 1, 51288, 0, 2, 31, 0, 3, 28265, 0, 0, 'Potential target of the spell is creature, entry is Searing Gaze (28265)');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (51136));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 51136, 0, 0, 31, 0, 3, 30899, 0, 0, 'Potential target of the spell is creature, entry is Abedneum (30899)');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (59867));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 59867, 0, 0, 31, 0, 3, 30899, 0, 0, 'Potential target of the spell is creature, entry is Abedneum (30899)');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (50810));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 50810, 0, 0, 1, 0, 50812, 0, 0, 0, 'Potential target of the spell has aura of spell Stoned (50812), effect EFFECT_0');
