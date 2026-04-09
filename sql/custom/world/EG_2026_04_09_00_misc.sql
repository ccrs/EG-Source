--
UPDATE `item_template` SET `duration` = 86400 WHERE `entry` = 39878;

 -- Jotunheim Proto-Drake smart ai
SET @ENTRY := 30330;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 28, 0, 100, 0, 0, 0, 0, 0, 146, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On passenger removed (vehicle) - Passenger: Set interactable');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30330 AND `SourceId` = 0;

DELETE FROM `disables` WHERE entry IN (7326, 7327) AND `sourceType` = 4;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (7326, 7327) AND `type` = 18;
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
(7326, 18, 0, 0, ''),
(7327, 18, 0, 0, '');

DELETE FROM `spell_script_names` WHERE `ScriptName` = 'EG_spell_lava_strike';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(57591, 'EG_spell_lava_strike');

UPDATE `areatrigger_teleport` SET `target_position_z`=66.549 WHERE `ID`=5243;
UPDATE `areatrigger_teleport` SET `target_position_z`=-109.163 WHERE `ID`=5241;

UPDATE `creature_template` SET `ScriptName`='npc_twilight_eggs' WHERE `entry`=31204;
