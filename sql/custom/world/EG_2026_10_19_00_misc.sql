--
DELETE FROM `vehicle_seat_addon` WHERE `SeatEntry` IN (2198, 2548, 2578);
INSERT INTO `vehicle_seat_addon` (`SeatEntry`, `SeatOrientation`, `ExitParamX`, `ExitParamY`, `ExitParamZ`, `ExitParamO`, `ExitParamValue`, `AttachOffsetX`, `AttachOffsetY`, `AttachOffsetZ`) VALUES
(2198, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2548, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2578, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);

UPDATE `item_template` SET `spelltrigger_1` = 0 WHERE `entry` = 44092 AND `spellid_1` = 0; -- Reforged Truesilver Champion: heirloom dropped the base item proc

DELETE FROM `spell_script_names` WHERE `spell_id` IN (51014, 51022, 51024) AND `ScriptName` IN ('EG_spell_varos_summon_arcane_beam', 'EG_spell_varos_arcane_beam_spawn_trigger', 'EG_spell_varos_arcane_beam_visual');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(51014, 'EG_spell_varos_summon_arcane_beam'),
(51022, 'EG_spell_varos_arcane_beam_spawn_trigger'),
(51024, 'EG_spell_varos_arcane_beam_visual');

UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 28239;

UPDATE `creature_template` SET `modelid2` = 0 WHERE `entry` = 28239;
UPDATE `creature_template` SET `modelid2` = 0 WHERE `entry` = 31628;

DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (28239, 31628);
INSERT INTO `creature_template_movement` (`CreatureId`, `Chase`) VALUES
(28239, 2),
(31628, 2);

UPDATE `creature_template` SET `unit_flags` = 33554432 WHERE `entry` = 29225; -- Anti-Magic Zone (Highlord Darion Mograine): uninteractible

DELETE FROM `smart_scripts` WHERE `entryorguid` = 28239 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(28239, 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Arcane Beam - On Respawn - Disable Auto Attack'),
(28239, 0, 1, 0, 11, 0, 100, 0, 0, 0, 0, 0, 0, 11, 51022, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Arcane Beam - On Respawn - Cast Arcane Beam Spawn Trigger'),
(28239, 0, 2, 0, 11, 0, 100, 0, 0, 0, 0, 0, 0, 123, 1000000, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 'Arcane Beam - On Respawn - Add Threat To Summoner');

UPDATE `creature_template` SET `modelid3` = 0 WHERE `entry` = 29174; -- Defender of the Light: display 26059 has no NPC item displays (naked human)

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 9795 AND `SourceEntry` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(15, 9795, 0, 0, 0, 9, 0, 12801, 0, 0, 0, 0, 0, '', 'Highlord Darion Mograine - Show gossip option if player has taken quest The Light of Dawn');

DELETE FROM `waypoint_data` WHERE `id` = 233386;
