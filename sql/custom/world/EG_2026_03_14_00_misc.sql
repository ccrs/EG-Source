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

 -- Titanium Thunderer smart ai
SET @ENTRY := 28965;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 2, 0, 100, 2, 0, 40, 12000, 18000, 11, 52879, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-40%% (cooldown 12000 - 18000 ms) - Self: Cast spell  Deflection (52879) with flags interrupt previous on Self'),
(@ENTRY, 0, 1, 0, 2, 0, 100, 4, 0, 65, 12000, 18000, 11, 59181, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-65%% (cooldown 12000 - 18000 ms) - Self: Cast spell  Deflection (59181) with flags interrupt previous on Self'),
(@ENTRY, 0, 2, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 10, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly respawn in 10 seconds'),
(@ENTRY, 0, 3, 4, 38, 0, 33, 0, 1, 1, 0, 0, 144, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Disable Immune to PC'),
(@ENTRY, 0, 4, 5, 61, 0, 100, 0, 0, 0, 0, 0, 145, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Disable Immune to NPC'),
(@ENTRY, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 28, 16245, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Remove aura due to spell  Freeze Anim (16245)'),
(@ENTRY, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 146, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Set interactable'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 80, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Attack Closest player in 80 yards'),
(@ENTRY, 0, 8, 0, 13, 0, 100, 2, 45000, 60000, 0, 0, 11, 52885, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'When victim is casting anyspell (once) - Self: Cast spell  Deadly Throw (52885) on Victim'),
(@ENTRY, 0, 9, 0, 13, 0, 100, 4, 45000, 60000, 0, 0, 11, 59180, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'When victim is casting anyspell (once) - Self: Cast spell  Deadly Throw (59180) on Victim'),
(@ENTRY, 0, 10, 0, 0, 0, 100, 2, 0, 5000, 7000, 15000, 11, 52904, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 7 - 15 seconds (0 - 5s initially) (IC) - Self: Cast spell  Throw (52904) on Random hostile'),
(@ENTRY, 0, 11, 0, 0, 0, 100, 4, 0, 5000, 7000, 15000, 11, 59179, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 7 - 15 seconds (0 - 5s initially) (IC) - Self: Cast spell  Throw (59179) on Random hostile'),
(@ENTRY, 0, 12, 0, 25, 0, 100, 0, 0, 0, 0, 0, 11, 16245, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Cast spell  Freeze Anim (16245) with flags triggered on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28965 AND `SourceId` = 0;

 -- Titanium Siegebreaker smart ai
SET @ENTRY := 28961;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 2, 0, 100, 2, 0, 40, 10000, 15000, 11, 52891, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-40%% (cooldown 10000 - 15000 ms) - Self: Cast spell  Blade Turning (52891) with flags interrupt previous on Self'),
(@ENTRY, 0, 1, 0, 2, 0, 100, 4, 0, 50, 10000, 15000, 11, 59173, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-50%% (cooldown 10000 - 15000 ms) - Self: Cast spell  Blade Turning (59173) with flags interrupt previous on Self'),
(@ENTRY, 0, 2, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 10, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly respawn in 10 seconds'),
(@ENTRY, 0, 3, 4, 38, 0, 33, 0, 1, 1, 0, 0, 144, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Disable Immune to PC'),
(@ENTRY, 0, 4, 5, 61, 0, 100, 0, 0, 0, 0, 0, 145, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Disable Immune to NPC'),
(@ENTRY, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 28, 16245, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Remove aura due to spell  Freeze Anim (16245)'),
(@ENTRY, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 146, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Set interactable'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 80, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Attack Closest player in 80 yards'),
(@ENTRY, 0, 8, 0, 2, 0, 100, 1, 0, 20, 0, 0, 11, 19134, 1, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-20%% (once) - Self: Cast spell  Frightening Shout (19134) with flags interrupt previous on Random hostile'),
(@ENTRY, 0, 9, 0, 9, 0, 100, 0, 0, 5, 10000, 15000, 11, 52890, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 5 yards (cooldown 10000 - 15000 ms) - Self: Cast spell  Penetrating Strike (52890) on Victim'),
(@ENTRY, 0, 10, 0, 25, 0, 100, 0, 0, 0, 0, 0, 11, 16245, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Cast spell  Freeze Anim (16245) with flags triggered on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28961 AND `SourceId` = 0;

 -- Invisible Stalker with guid 126866 smart ai
SET @ENTRY := -126866;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 15214;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 38, 0, 100, 257, 1, 1, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126860, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Creature Titanium Thunderer (28965) with guid 126860 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126848, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Creature Titanium Thunderer (28965) with guid 126848 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126851, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Creature Titanium Thunderer (28965) with guid 126851 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126836, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Creature Titanium Siegebreaker (28961) with guid 126836 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 4, 5, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126847, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Creature Titanium Siegebreaker (28961) with guid 126847 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126846, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Creature Titanium Siegebreaker (28961) with guid 126846 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126844, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Creature Titanium Siegebreaker (28961) with guid 126844 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126845, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Creature Titanium Siegebreaker (28961) with guid 126845 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 8, 9, 38, 0, 100, 257, 1, 2, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126837, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Creature Titanium Siegebreaker (28961) with guid 126837 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 9, 10, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126852, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Creature Titanium Thunderer (28965) with guid 126852 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 10, 11, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126861, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Creature Titanium Thunderer (28965) with guid 126861 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 11, 12, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126838, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Creature Titanium Siegebreaker (28961) with guid 126838 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 12, 13, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126843, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Creature Titanium Siegebreaker (28961) with guid 126843 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 13, 14, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126841, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Creature Titanium Siegebreaker (28961) with guid 126841 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 14, 15, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126849, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Creature Titanium Thunderer (28965) with guid 126849 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 15, 0, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126862, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Creature Titanium Thunderer (28965) with guid 126862 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 16, 17, 38, 0, 100, 257, 1, 3, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126857, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Thunderer (28965) with guid 126857 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 17, 18, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126855, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Thunderer (28965) with guid 126855 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 18, 19, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126859, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Thunderer (28965) with guid 126859 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 19, 20, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126854, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Thunderer (28965) with guid 126854 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 20, 21, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126853, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Thunderer (28965) with guid 126853 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 21, 22, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126858, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Thunderer (28965) with guid 126858 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 22, 23, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126856, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Thunderer (28965) with guid 126856 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 23, 24, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126850, 28965, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Thunderer (28965) with guid 126850 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 24, 25, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126835, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Siegebreaker (28961) with guid 126835 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 25, 26, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126840, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Siegebreaker (28961) with guid 126840 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 26, 27, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126839, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Siegebreaker (28961) with guid 126839 (fetching): Set creature data #1 to 1'),
(@ENTRY, 0, 27, 0, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 126842, 28961, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Creature Titanium Siegebreaker (28961) with guid 126842 (fetching): Set creature data #1 to 1');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = -126866 AND `SourceId` = 0;
