--
UPDATE `creature_template` SET `modelid1` = 28783, `modelid2` = 0 WHERE `entry` = 33189;
UPDATE `creature_template` SET `modelid1` = 28782, `modelid2` = 0 WHERE `entry` = 33218;

UPDATE `creature_template` SET `VehicleId` = 340 WHERE `entry` = 34003;

DELETE FROM `spelldifficulty_dbc` WHERE `id` IN (61911, 62217, 62451, 63818, 64717, 64776);
INSERT INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`, `spellid2`, `spellid3`) VALUES
(61911, 61911, 63495, 0, 0), -- Static Disruption
(62217, 62217, 62922, 0, 0), -- Unstable Energy (Elder Brightleaf)
(62451, 62451, 62865, 0, 0), -- Unstable Energy (Freya)
(63818, 63818, 63978, 0, 0), -- Rumble / Stone Nova
(64717, 64717, 65241, 0, 0), -- Flame Spray
(64776, 64776, 65240, 0, 0); -- Machine Gun

 -- Rubble smart ai
SET @ENTRY := 33768;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 63818, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Cast spell  Rumble (63818) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33768 AND `SourceId` = 0;

DELETE FROM `spell_script_names` WHERE `ScriptName` IN('EG_spell_flame_leviathan_grab_crate_triggered');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(62482, 'EG_spell_flame_leviathan_grab_crate_triggered'),
(67387, 'EG_spell_flame_leviathan_grab_crate_triggered'); -- Grab Crate (Salvaged Chopper)

DELETE FROM `creature_template_spell` WHERE `CreatureID` = 33062 AND `Index` = 4;
INSERT INTO `creature_template_spell` (`CreatureID`, `Index`, `Spell`, `VerifiedBuild`) VALUES
(33062, 4, 67372, 0); -- Grab Pyrite
