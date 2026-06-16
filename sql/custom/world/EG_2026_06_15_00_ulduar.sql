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
UPDATE `creature_template` SET `unit_class` = 2 WHERE `entry` IN (33772, 33773);

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

UPDATE `creature_template` SET `unit_class` = 4 WHERE `entry` IN (33824, 33831);
UPDATE `creature_template` SET `unit_class` = 1 WHERE `entry` IN (33822, 33828, 33823, 33832);
UPDATE `creature_template` SET `ManaModifier` = 8 WHERE `entry` IN (33772, 33773);
