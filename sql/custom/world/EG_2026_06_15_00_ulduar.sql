--
-- Ironwork Cannon (33264)
UPDATE `creature_template` SET `exp` = 0, `HealthModifier` = 15 WHERE `entry` = 33264;
UPDATE `creature` SET `curhealth` = 80130 WHERE `id` = 33264;

SET @ENTRY := 33264;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Disable combat based movement'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 1000, 2000, 2000, 3000, 11, 62395, 0, 0, 0, 0, 0, 5, 200, 0, 0, 0, 0, 0, 0, 'Every 2 - 3 seconds (1 - 2s initially) (IC) - Self: Cast spell  Flame Cannon (62395) on Random hostile'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 2000, 2000, 2000, 2000, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Every 2 seconds (IC) - Self: Evade');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33264 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 3, 33264, 0, 0, 29, 0, 33109, 200, 0, 1, 'There is no creature Salvaged Demolisher (33109) within range 200 yards to Action invoker'),
(22, 3, 33264, 0, 0, 29, 0, 33060, 200, 0, 1, 'There is no creature Salvaged Siege Engine (33060) within range 200 yards to Action invoker'),
(22, 3, 33264, 0, 0, 29, 0, 33062, 200, 0, 1, 'There is no creature Salvaged Chopper (33062) within range 200 yards to Action invoker');

-- Storm Tempered Keeper (33699/33700, 33722/33723)
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` IN (33699, 33700, 33722, 33723);

-- Allow SNARE and DAZE on selected Ulduar trash
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (34085, 34186, 33121, 33191, 34267, 34268, 34269, 34270, 34271, 34272, 34273, 34274, 34190, 34229, 34196, 34245, 34198, 34236, 34199, 34237);

-- Twilight Shadowblade (33824/33831)
UPDATE `creature_template_addon` SET `auras` = '63756' WHERE `entry` IN (33824, 33831);
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33824, 33831);
UPDATE `creature_template` SET `HealthModifier` = 45 WHERE `entry` = 33831;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'EG_npc_twilight_shadowblade' WHERE `entry` = 33824;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = 33824;

-- Twilight Slayer (33823/33832)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33823, 33832);

SET @ENTRY := 33823;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 3000, 6000, 9000, 11, 35054, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 6 - 9 seconds (1 - 3s initially) (IC) - Self: Cast spell  Mortal Strike (35054) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 10000, 15000, 20000, 30000, 11, 63784, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 20 - 30 seconds (10 - 15s initially) (IC) - Self: Cast spell  Bladestorm (63784) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33823 AND `SourceId` = 0;

-- Molten Colossus (34069/34185) and Magma Rager (34086/34201)
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` IN (34069, 34185, 34086, 34201);

-- Twilight Guardian (33822/33828)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33822, 33828);

SET @ENTRY := 33822;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 2000, 4000, 6000, 11, 62317, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 4 - 6 seconds (1 - 2s initially) (IC) - Self: Cast spell  Devastate (62317) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 3000, 5000, 8000, 12000, 11, 57807, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 8 - 12 seconds (3 - 5s initially) (IC) - Self: Cast spell  Sunder Armor (57807) on Victim'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 6000, 9000, 12000, 18000, 11, 52719, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 12 - 18 seconds (6 - 9s initially) (IC) - Self: Cast spell  Concussion Blow (52719) on Victim'),
(@ENTRY, 0, 3, 0, 0, 0, 100, 0, 4000, 7000, 10000, 14000, 11, 63757, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 10 - 14 seconds (4 - 7s initially) (IC) - Self: Cast spell  Thunderclap (63757) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33822 AND `SourceId` = 0;

-- Twilight Adherent (33818/33827)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33818, 33827);
UPDATE `creature_template` SET `unit_class` = 2 WHERE `entry` IN (33818, 33827);

SET @ENTRY := 33818;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 3000, 7000, 3000, 7000, 11, 63760, 0, 0, 0, 0, 0, 31, 40, 0, 1, 0, 0, 0, 0, 'Every 3 - 7 seconds (3 - 7s initially) (IC) - Self: Cast spell  Greater Heal (63760) on Lowest HP friendly unit in 40 yards including self'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 10000, 10000, 18000, 11, 37978, 0, 0, 0, 0, 0, 31, 40, 0, 1, 0, 0, 0, 0, 'Every 10 - 18 seconds (5 - 10s initially) (IC) - Self: Cast spell  Renew (37978) on Lowest HP friendly unit in 40 yards including self'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 15000, 25000, 15000, 25000, 11, 13704, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 25 seconds (IC) - Self: Cast spell  Psychic Scream (13704) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33818 AND `SourceId` = 0;

-- DamageModifier
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` IN (33818, 33819, 33820, 33822, 33823, 33824, 33699, 33722);
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` IN (33827, 33829, 33830, 33828, 33832, 33831, 33700, 33723);

-- Iron Mender OOC also in 25
UPDATE `smart_scripts` SET `event_flags` = 0 WHERE `source_type` = 0 AND `entryOrGuid` = 34198 AND `id` = 0;

-- Faceless Horror (33772/33773)
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` IN (33772, 33773);
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` = 33772;
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` = 33773;

SET @ENTRY := 33772;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 4, 0, 100, 0, 0, 0, 0, 0, 11, 63703, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Cast spell  Void Wave (63703) on Self'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Set event phase to phase 1'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 5000, 8000, 8000, 12000, 11, 63722, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 8 - 12 seconds (5 - 8s initially) (IC) - Self: Cast spell  Shadow Crash (63722) on Random hostile'),
(@ENTRY, 0, 3, 0, 0, 0, 100, 0, 12000, 18000, 25000, 35000, 11, 63713, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 25 - 35 seconds (12 - 18s initially) (IC) - Self: Cast spell  Dominate Mind (63713) on Self'),
(@ENTRY, 0, 4, 0, 0, 1, 100, 0, 10000, 15000, 25000, 35000, 11, 63708, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 25 - 35 seconds (10 - 15s initially) (IC) - Self: Cast spell  Summon Void Beast (63708) on Self'),
(@ENTRY, 0, 5, 6, 35, 0, 100, 0, 33806, 0, 0, 0, 28, 63710, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Void Beast (33806) despawn - Self: Remove aura due to spell  Void Barrier (63710)'),
(@ENTRY, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Void Beast (33806) despawn - Set event phase to phase 1'),
(@ENTRY, 0, 7, 8, 82, 0, 100, 0, 33806, 0, 0, 0, 28, 63710, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Void Beast (33806) died - Self: Remove aura due to spell  Void Barrier (63710)'),
(@ENTRY, 0, 8, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Void Beast (33806) died - Set event phase to phase 1'),
(@ENTRY, 0, 9, 10, 17, 0, 100, 0, 33806, 0, 0, 0, 11, 63710, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On summoned unit Void Beast (33806) - Self: Cast spell  Void Barrier (63710) on Self'),
(@ENTRY, 0, 10, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On summoned unit Void Beast (33806) - Set event phase to phase 2');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33772 AND `SourceId` = 0;

-- Void Beast add (33806/33815)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33806, 33815);
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` = 33806;
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` = 33815;

SET @ENTRY := 33806;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 3000, 6000, 5000, 9000, 11, 63723, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 9 seconds (3 - 6s initially) (IC) - Self: Cast spell  Shadow Nova (63723) on Self'),
(@ENTRY, 0, 1, 0, 54, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 25, 100, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Attack Closest enemy creature in 100 yards'),
(@ENTRY, 0, 2, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33806 AND `SourceId` = 0;

 -- Lightning Elemental smart ai
SET @ENTRY := 32958;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 100, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Attack Closest player in 100 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 32958 AND `SourceId` = 0;

UPDATE `creature_template` SET `mechanic_immune_mask` = 4784129 WHERE `entry` IN (32958, 33689);

UPDATE `creature_template` SET `unit_class` = 4 WHERE `entry` IN (33824, 33831);
UPDATE `creature_template` SET `unit_class` = 1 WHERE `entry` IN (33822, 33828, 33823, 33832);
UPDATE `creature_template` SET `ManaModifier` = 8 WHERE `entry` IN (33772, 33773);

-- Clockwork Sapper (34193/34220)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (34193, 34220);
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` = 34193;
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` = 34220;

SET @ENTRY := 34193;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 5000, 9000, 10000, 15000, 11, 64740, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 10 - 15 seconds (5 - 9s initially) (IC) - Self: Cast spell  Energy Sap (64740) on Random hostile');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 34193 AND `SourceId` = 0;

DELETE FROM `spell_script_names` WHERE `ScriptName` = 'EG_spell_energy_sap';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(64740, 'EG_spell_energy_sap');

UPDATE `creature_template` SET `flags_extra` = 8320 WHERE `entry` = 34223;

 -- Sapper Explosion smart ai
SET @ENTRY := 34223;
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` = @ENTRY;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Set react state to Passive'),
(@ENTRY, 0, 1, 0, 25, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Disable combat based movement'),
(@ENTRY, 0, 2, 0, 60, 0, 100, 1, 5500, 5500, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Time = 5.5 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 3, 0, 60, 0, 100, 1, 500, 500, 0, 0, 11, 64875, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Time = 0.5 seconds - Self: Cast spell  Sapper Explosion (64875) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 34223 AND `SourceId` = 0;

-- Boomer XP-500 (34192/34216)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189887 WHERE `entry` IN (34192, 34216);
UPDATE `creature_template_addon` SET `auras` = '63767' WHERE `entry` IN (34192, 34216);

-- Trash (34191/34217)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (34191, 34217);

-- Forest Swarmer (33431/33731)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33431, 33731);

SET @ENTRY := 33431;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 5000, 10000, 12000, 18000, 11, 63059, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 12 - 18 seconds (5 - 10s initially) (IC) - Self: Cast spell  Pollinate (63059) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33431 AND `SourceId` = 0;

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (63059));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES
(13, 7, 63059, 0, 0, 31, 0, 3, 33430, 0, 0, 'Potential target of the spell is creature, entry is Guardian Lasher (33430)');

-- Guardian Lasher (33430/33732)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33430, 33732);
UPDATE `creature_template_addon` SET `auras` = '63007' WHERE `entry` IN (33430, 33732);
UPDATE `creature_addon` SET `auras` = '63007' WHERE `guid` IN (136604, 136606, 136608);
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` = 33430;
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` = 33732;

SET @ENTRY := 33430;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 3000, 6000, 9000, 11, 63047, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 6 - 9 seconds (1 - 3s initially) (IC) - Self: Cast spell  Guardian\'s Lash (63047) on Victim');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33430 AND `SourceId` = 0;

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (63006));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES
(13, 7, 63006, 0, 0, 31, 0, 3, 33431, 0, 0, 'Aggregation Pheromones (63006) only buffs Forest Swarmer (33431)');

-- Dark Rune Ravager (33755/33758)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33755, 33758);
UPDATE `creature_template_addon` SET `auras` = '63616' WHERE `entry` IN (33755, 33758);
UPDATE `creature_addon` SET `auras` = '63616' WHERE `guid` IN (137483, 137484);
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` = 33755;
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` = 33758;

-- Dark Rune Thunderer (33754/33757)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33754, 33757);
UPDATE `creature_template_addon` SET `auras` = '63610' WHERE `entry` IN (33754, 33757);
UPDATE `creature_addon` SET `auras` = '63610' WHERE `guid` IN (137481, 137482);
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` = 33754;
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` = 33757;

UPDATE `creature_template` SET `flags_extra` = 8896 WHERE `entry` = 33378;
DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (33378);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`) VALUES
(33378, 1, 0, 1, 0, 0, 0);

-- Nature's Blade (33527/33741)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33527, 33741);

SET @ENTRY := 33527;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 2000, 4000, 5000, 8000, 11, 63247, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 8 seconds (2 - 4s initially) (IC) - Self: Cast spell  Living Tsunami (63247) on Victim');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33527 AND `SourceId` = 0;

-- Guardian of Life (33528/33733)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33528, 33733);
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` = 33528;
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` = 33733;

DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (33528, 33733);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`) VALUES
(33528, 1, 1, 1, 0, 0, 0),
(33733, 1, 1, 1, 0, 0, 0);

SET @ENTRY := 33528;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 3000, 6000, 8000, 12000, 11, 63226, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 8 - 12 seconds (3 - 6s initially) (IC) - Self: Cast spell  Poison Breath (63226) on Victim');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33528 AND `SourceId` = 0;

-- Ironroot Lasher (33526/33734)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33526, 33734);
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` = 33526;
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` = 33734;

SET @ENTRY := 33526;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 3000, 6000, 6000, 10000, 11, 64072, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 6 - 10 seconds (3 - 6s initially) (IC) - Self: Cast spell  Thorn Swarm (64072) on Random hostile'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 8000, 12000, 20000, 11, 63240, 0, 0, 0, 0, 0, 31, 40, 0, 1, 0, 0, 0, 0, 'Every 12 - 20 seconds (5 - 8s initially) (IC) - Self: Cast spell  Ironroot Thorns (63240) on Lowest HP friendly within 40 yards including self');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33526 AND `SourceId` = 0;

-- Mangrove Ent (33525/33735)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33525, 33735);
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` = 33525;
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` = 33735;

SET @ENTRY := 33525;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 3000, 7000, 8000, 12000, 11, 63242, 0, 0, 0, 0, 0, 31, 40, 0, 1, 0, 0, 0, 0, 'Every 8 - 12 seconds (3 - 7s initially) (IC) - Self: Cast spell  Nourish (63242) on Lowest HP friendly within 40 yards including self'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 9000, 12000, 18000, 11, 63272, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 12 - 18 seconds (5 - 9s initially) (IC) - Self: Cast spell  Hurricane (63272) on Random hostile'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 15000, 20000, 25000, 35000, 11, 63241, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 25 - 35 seconds (15 - 20s initially) (IC) - Self: Cast spell  Tranquility (63241) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33525 AND `SourceId` = 0;

-- Corrupted Servitor (33354/33729)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33354, 33729);
UPDATE `creature_template` SET `DamageModifier` = 13 WHERE `entry` = 33354;
UPDATE `creature_template` SET `DamageModifier` = 22 WHERE `entry` = 33729;

SET @ENTRY := 33354;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 4000, 7000, 12000, 18000, 11, 63169, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 12 - 18 seconds (4 - 7s initially) (IC) - Self: Cast spell  Petrify Joints (63169) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 6000, 10000, 10000, 15000, 11, 63149, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 10 - 15 seconds (6 - 10s initially) (IC) - Self: Cast spell  Violent Earth (63149) on Random hostile');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33354 AND `SourceId` = 0;

-- Misguided Nymph (33355/33737)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33355, 33737);
UPDATE `creature_template` SET `unit_class` = 2 WHERE `entry` IN (33355, 33737);

SET @ENTRY := 33355;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 3000, 4000, 7000, 11, 63111, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 4 - 7 seconds (1 - 3s initially) (IC) - Self: Cast spell  Frost Spear (63111) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 4000, 8000, 9000, 14000, 11, 63082, 0, 0, 0, 0, 0, 31, 40, 0, 1, 0, 0, 0, 0, 'Every 9 - 14 seconds (4 - 8s initially) (IC) - Self: Cast spell  Bind Life (63082) on Lowest HP friendly within 40 yards including self'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 6000, 10000, 25000, 35000, 11, 63136, 0, 0, 0, 0, 0, 31, 40, 0, 1, 0, 0, 0, 0, 'Every 25 - 35 seconds (6 - 10s initially) (IC) - Self: Cast spell  Winter\'s Embrace (63136) on Lowest HP friendly within 40 yards including self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33355 AND `SourceId` = 0;

DELETE FROM `spelldifficulty_dbc` WHERE `id` IN (64740, 63610, 64072, 63557, 63169, 63149, 63082, 63111, 63136);
INSERT INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`, `spellid2`, `spellid3`) VALUES
(64740, 64740, 64876, 0, 0),
(63610, 63610, 63674, 0, 0),
(64072, 64072, 64073, 0, 0),
(63169, 63169, 63549, 0, 0),
(63149, 63149, 63547, 0, 0),
(63082, 63082, 63559, 0, 0),
(63111, 63111, 63562, 0, 0),
(63136, 63136, 63564, 0, 0);
