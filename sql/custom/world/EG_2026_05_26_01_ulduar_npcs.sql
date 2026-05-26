--
-- Molten Colossus
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = 34069;
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = 34185;

 -- Molten Colossus smart ai
SET @ENTRY := 34069;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 64700, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Cast spell  Magma Splash (64700) with flags triggered on Self'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 8000, 8000, 12000, 11, 64698, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 8 - 12 seconds (5 - 8s initially) (IC) - Self: Cast spell  Pyroblast (64698) on Victim'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 10000, 15000, 15000, 20000, 11, 64697, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 20 seconds (10 - 15s initially) (IC) - Self: Cast spell  Earthquake (64697) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 34069 AND `SourceId` = 0;

-- Magma Rager
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = 34086;
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = 34201;

 -- Magma Rager smart ai
SET @ENTRY := 34086;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1500, 3000, 3000, 5000, 11, 64773, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 3 - 5 seconds (1.5 - 3s initially) (IC) - Self: Cast spell  Fire Blast (64773) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 8000, 12000, 25000, 35000, 11, 64746, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 25 - 35 seconds (8 - 12s initially) (IC) - Self: Cast spell  Superheated Winds (64746) on Victim');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 34086 AND `SourceId` = 0;

-- Parts Recovery Technician
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = 34267;
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = 34268;

 -- Parts Recovery Technician smart ai
SET @ENTRY := 34267;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 17, 233, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set emote state to STATE_WORK_MINING (233)'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 10000, 22000, 30000, 11, 65070, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 22 - 30 seconds (5 - 10s initially) (IC) - Self: Cast spell  Defense Matrix (65070) on Self'),
(@ENTRY, 0, 2, 0, 4, 0, 100, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Set emote state to 0');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 34267 AND `SourceId` = 0;

-- XR-949 Salvagebot
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = 34269;
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = 34270;

 -- XR-949 Salvagebot smart ai
SET @ENTRY := 34269;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 17, 69, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set emote state to STATE_USESTANDING (69)'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 10000, 12000, 20000, 11, 65099, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 12 - 20 seconds (5 - 10s initially) (IC) - Self: Cast spell  Deploy Salvage Saws (65099) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 34269 AND `SourceId` = 0;

-- Salvagebot Sawblade
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = 34288;
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = 34291;

 -- Salvagebot Sawblade smart ai
SET @ENTRY := 34288;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 11, 65087, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  Salvage Saws Visual (65087) on Self'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 11, 65094, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  Sawblades Sound Periodic (65094) on Self'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 65089, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  Sawblades (65089) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 34288 AND `SourceId` = 0;

-- 65099 Deploy Salvage Saws
DELETE FROM `spell_script_names` WHERE `spell_id` = 65099 AND `ScriptName` = 'EG_spell_deploy_salvage_saws';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(65099, 'EG_spell_deploy_salvage_saws');

DELETE FROM `spelldifficulty_dbc` WHERE `id` = 65089;
INSERT INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`, `spellid2`, `spellid3`) VALUES
(65089, 65089, 65102, 0, 0);

UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 8256 WHERE `entry` = 34288;
UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 8256 WHERE `entry` = 34291;
