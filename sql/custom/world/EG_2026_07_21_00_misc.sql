--
DELETE FROM `spell_dbc` WHERE `Id` IN (37373, 37392, 37503);
INSERT INTO `spell_dbc` (`Id`, `Attributes`, `AttributesEx`, `AttributesEx2`, `CastingTimeIndex`, `ProcChance`, `DurationIndex`, `RangeIndex`, `EquippedItemClass`, `Effect1`, `EffectImplicitTargetA1`, `EffectRadiusIndex1`, `EffectMiscValue1`, `EffectMiscValueB1`, `DmgMultiplier1`, `DmgMultiplier2`, `DmgMultiplier3`, `SchoolMask`, `SpellName`) VALUES
(37373, 256, 268435456, 0, 1, 101, 25, 1, -1, 50, 18, 0, 184926, 0, 1, 1, 1, 1, 'Summon Aura Generator 000'),
(37392, 256, 268435456, 4, 1, 101, 552, 1, -1, 28, 18, 0, 21759, 64, 1, 1, 1, 1, 'Summon Top Bunny Caster'),
(37503, 384, 0, 4, 1, 101, 21, 7, -1, 28, 44, 13, 21796, 64, 1, 1, 1, 1, 'Summon Wyrm from Beyond');

UPDATE `gameobject_template` SET `ScriptName` = 'go_aura_generator_000' WHERE `entry` = 184926;

 -- Wyrm from Beyond smart ai
SET @ENTRY := 21796;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 3, 0, 20476, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Morph to model ManaWurm.mdx (20476)'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Attack Owner/Summoner');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 21796 AND `SourceId` = 0;

UPDATE `creature_template` SET `flags_extra` = 8386 WHERE `entry` = 21760;
