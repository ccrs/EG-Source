-- 
DELETE FROM `creature_loot_template` WHERE `Entry` = 17102;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES 
(17102, 118, 0, 1.9928, 0, 1, 0, 1, 1, ''),
(17102, 159, 0, 3.3579, 0, 1, 0, 1, 1, ''),
(17102, 779, 0, 19.1903, 0, 1, 0, 1, 1, ''),
(17102, 6303, 0, 7.3749, 0, 1, 0, 1, 3, ''),
(17102, 11111, 11111, 0.1, 0, 1, 0, 1, 1, ''),
(17102, 14500, 14500, 100, 0, 1, 0, 1, 1, ''),
(17102, 23614, 0, 98, 0, 1, 0, 1, 1, ''),
(17102, 24073, 24073, 5, 0, 1, 1, 1, 1, ''),
(17102, 44007, 44007, 0.5, 0, 1, 0, 1, 1, '');

DELETE FROM `creature_addon` WHERE `guid` IN (300105, 300106, 300107, 300108, 300109, 300110, 300111, 300112, 300113);
DELETE FROM `creature` WHERE `guid` IN (300105, 300106, 300107, 300108, 300109, 300110, 300111, 300112, 300113);
INSERT INTO `creature` (`guid`, `id`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `MovementType`, `ScriptName`, `VerifiedBuild`, `map`, `zoneId`, `areaId`, `modelid`, `equipment_id`, `currentwaypoint`, `curhealth`, `curmana`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(300105, 30120, 1, 4, 7211.71, -3587.46, 825.161, 3.63489, 300, 5, 1, '', 0, 571, 0, 0, 0, 0, 0, 12600, 0, 0, 0, 0),
(300106, 30120, 1, 4, 7158.39, -3562.27, 828.613, 5.28815, 300, 5, 1, '', 0, 571, 0, 0, 0, 0, 0, 12600, 0, 0, 0, 0),
(300107, 30120, 1, 4, 7170.54, -3605.32, 830.165, 4.73053, 300, 5, 1, '', 0, 571, 0, 0, 0, 0, 0, 12600, 0, 0, 0, 0),
(300108, 30120, 1, 4, 7213.37, -3520.19, 830.061, 0.803538, 300, 5, 1, '', 0, 571, 0, 0, 0, 0, 0, 12600, 0, 0, 0, 0),
(300109, 30120, 1, 4, 7181.6, -3495.81, 837.267, 0.819246, 300, 5, 1, '', 0, 571, 0, 0, 0, 0, 0, 12600, 0, 0, 0, 0),
(300110, 30120, 1, 4, 7212.84, -3480.11, 833.825, 1.10984, 300, 5, 1, '', 0, 571, 0, 0, 0, 0, 0, 12600, 0, 0, 0, 0),
(300111, 30120, 1, 4, 7226.02, -3628.44, 823.281, 2.81414, 300, 5, 1, '', 0, 571, 0, 0, 0, 0, 0, 12600, 0, 0, 0, 0),
(300112, 30120, 1, 4, 7155.84, -3636.44, 835.566, 0.901691, 300, 5, 1, '', 0, 571, 0, 0, 0, 0, 0, 12600, 0, 0, 0, 0),
(300113, 30120, 1, 4, 7189.73, -3552.77, 827.87, 1.77348, 300, 5, 1, '', 0, 571, 0, 0, 0, 0, 0, 12600, 0, 0, 0, 0);

 -- Anub'ar Webspinner smart ai
SET @ENTRY := 29335;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 2, 4000, 7000, 9000, 12000, 11, 54290, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 9 - 12 seconds (4 - 7s initially) (IC) - Self: Cast spell  Web Shot (54290) on Random hostile'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 4, 4000, 7000, 9000, 12000, 11, 59362, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 9 - 12 seconds (4 - 7s initially) (IC) - Self: Cast spell  Web Shot (59362) on Random hostile'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 6, 10000, 13000, 19000, 22000, 11, 52086, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 'Every 19 - 22 seconds (10 - 13s initially) (IC) - Self: Cast spell  Web Wrap (52086) on Random hostile (not top)');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 29335 AND `SourceId` = 0;
