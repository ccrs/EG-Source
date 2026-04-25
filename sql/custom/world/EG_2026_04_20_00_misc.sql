--
UPDATE `creature_template_addon` SET `auras` = '19483' WHERE `entry` = 89;

UPDATE `creature_template` SET `BaseAttackTime` = 2400, `DamageModifier` = 1 WHERE `entry` = 31480;
UPDATE `creature_template` SET `DamageModifier` = 1 WHERE `entry` = 30625;

 -- Scourge Package smart ai
SET @ENTRY := 30887;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 29, 7, 180, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Follow Summoner by distance 7, angle 180'),
(@ENTRY, 0, 1, 0, 54, 0, 100, 0, 0, 0, 0, 0, 87, 3088700, 3088701, 3088702, 3088703, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Call random timed action list: Scourge Package #0 (3088700), Scourge Package #1 (3088701), Scourge Package #2 (3088702), Scourge Package #3 (3088703), 0, 0 (update always)'),
(@ENTRY, 0, 2, 0, 38, 0, 100, 0, 1, 1, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Despawn in 1 s');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30887 AND `SourceId` = 0;
