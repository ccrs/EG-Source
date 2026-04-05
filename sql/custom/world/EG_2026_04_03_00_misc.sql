 -- Dark Ranger Marrah smart ai
SET @ENTRY := 24137;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 11, 0, 100, 0, 0, 0, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set faction to Friendly (35)'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 83, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Remove npc flags GOSSIP, QUESTGIVER'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, 183.852, -76.5012, 15.8429, 3.45508, 'On respawn - Self: Move to position (183.852, -76.5012, 15.8429, 3.45508) (point id 1)'),
(@ENTRY, 0, 3, 4, 34, 0, 100, 0, 8, 1, 0, 0, 91, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type POINT_MOTION_TYPE inform, point 1 - Self: Remove stand state SIT'),
(@ENTRY, 0, 4, 5, 61, 0, 100, 0, 0, 0, 0, 0, 82, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type POINT_MOTION_TYPE inform, point 1 - Self: Add npc flags GOSSIP, QUESTGIVER'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 28, 34189, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type POINT_MOTION_TYPE inform, point 1 - Self: Remove aura due to spell  Stealth (34189)');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 24137 AND `SourceId` = 0;

UPDATE `creature_template` SET `unit_flags` = 32832 WHERE `entry` = 657;
UPDATE `creature_template` SET `unit_flags` = 32768 WHERE `entry` = 3450;
UPDATE `creature_template` SET `unit_flags` = 32832 WHERE `entry` = 3947;
UPDATE `creature_template` SET `unit_flags` = 32832 WHERE `entry` = 1732;
UPDATE `creature_template` SET `unit_flags` = 32832 WHERE `entry` = 636;
UPDATE `creature_template` SET `unit_flags` = 32832 WHERE `entry` = 646;
UPDATE `creature_template` SET `unit_flags` = 32832 WHERE `entry` = 639;
UPDATE `creature_template` SET `unit_flags` = 32768 WHERE `entry` = 4416;
UPDATE `creature_template` SET `unit_flags` = 32832 WHERE `entry` = 634;
UPDATE `creature_template` SET `unit_flags` = 32832 WHERE `entry` = 4417;

DELETE FROM `creature_loot_template` WHERE `Entry` = 26410;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES 
(26410, 24727, 24727, 1.5, 0, 1, 0, 1, 1, ''),
(26410, 26000, 26000, 3, 0, 1, 1, 1, 1, ''),
(26410, 26001, 26001, 3, 0, 1, 1, 1, 1, ''),
(26410, 26007, 26007, 1, 0, 1, 1, 1, 1, ''),
(26410, 26008, 26008, 1, 0, 1, 1, 1, 1, ''),
(26410, 26009, 26009, 1, 0, 1, 1, 1, 1, ''),
(26410, 26010, 26010, 1, 0, 1, 1, 1, 1, ''),
(26410, 26040, 26040, 18.3, 0, 1, 0, 1, 1, ''),
(26410, 33358, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33359, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33360, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33361, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33363, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26410, 33364, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33372, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33373, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33377, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33378, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33392, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26410, 33395, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33396, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26410, 33405, 0, 0.3, 0, 1, 0, 1, 1, ''),
(26410, 33408, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33410, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33422, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33425, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33427, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33428, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 33429, 0, 0.3, 0, 1, 0, 1, 1, ''),
(26410, 33438, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26410, 33440, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26410, 33443, 0, 3.3, 0, 1, 0, 1, 1, ''),
(26410, 33444, 0, 2.5, 0, 1, 0, 1, 1, ''),
(26410, 35988, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 35993, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 35995, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 35999, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36008, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36017, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36111, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36120, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36225, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36240, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36345, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36346, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36347, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36352, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36424, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36436, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36466, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26410, 36508, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36661, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36674, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36675, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 36858, 0, 100, 0, 1, 0, 1, 1, ''),
(26410, 37013, 0, 10, 1, 1, 0, 1, 1, ''),
(26410, 37091, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26410, 37093, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26410, 37097, 0, 0.3, 0, 1, 0, 1, 1, ''),
(26410, 42105, 0, 33, 1, 1, 0, 1, 1, ''),
(26410, 43465, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26410, 43467, 0, 0.3, 0, 1, 0, 1, 1, ''),
(26410, 43510, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26410, 43622, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26410, 43851, 0, 22.3, 0, 1, 0, 1, 1, '');

DELETE FROM `creature_loot_template` WHERE `Entry` = 26409;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES 
(26409, 24727, 24727, 1.5, 0, 1, 0, 1, 1, ''),
(26409, 26000, 26000, 3, 0, 1, 1, 1, 1, ''),
(26409, 26001, 26001, 3, 0, 1, 1, 1, 1, ''),
(26409, 26007, 26007, 1, 0, 1, 1, 1, 1, ''),
(26409, 26008, 26008, 1, 0, 1, 1, 1, 1, ''),
(26409, 26009, 26009, 1, 0, 1, 1, 1, 1, ''),
(26409, 26010, 26010, 1, 0, 1, 1, 1, 1, ''),
(26409, 26040, 26040, 17.3, 0, 1, 0, 1, 1, ''),
(26409, 33361, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33362, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26409, 33364, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26409, 33372, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33374, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33377, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33390, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33391, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33393, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33394, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33396, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26409, 33404, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33406, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33407, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26409, 33408, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33424, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33425, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26409, 33426, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33427, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26409, 33428, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33429, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26409, 33430, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33438, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33440, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33443, 0, 6.2, 0, 1, 0, 1, 1, ''),
(26409, 33444, 0, 2.1, 0, 1, 0, 1, 1, ''),
(26409, 33460, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 33461, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 35991, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 35998, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 35999, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36000, 0, 0.3, 0, 1, 0, 1, 1, ''),
(26409, 36001, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36006, 0, 0.3, 0, 1, 0, 1, 1, ''),
(26409, 36016, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36107, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36120, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36122, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36124, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36127, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36236, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36327, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36329, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36343, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36409, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36421, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26409, 36548, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36578, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36590, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36591, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36674, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36702, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 36852, 0, 1, 1, 1, 0, 1, 1, ''),
(26409, 36857, 0, 100, 0, 1, 0, 1, 1, ''),
(26409, 37013, 0, 10, 1, 1, 0, 1, 1, ''),
(26409, 37097, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 42105, 0, 33, 1, 1, 0, 1, 1, ''),
(26409, 43463, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 43465, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26409, 43467, 0, 0.2, 0, 1, 0, 1, 1, ''),
(26409, 43507, 0, 0.1, 0, 1, 0, 1, 1, ''),
(26409, 43851, 0, 23, 0, 1, 0, 1, 1, '');

 -- Charged Sentry Totem smart ai
SET @ENTRY := 28938;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 60, 0, 100, 0, 1000, 2000, 2000, 4000, 11, 52705, 0, 0, 0, 0, 0, 25, 30, 0, 0, 0, 0, 0, 0, 'Every 2 - 4 seconds (1 - 2s initially) - Self: Cast spell  Sentry Shock (52705) on Closest enemy creature in 30 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28938 AND `SourceId` = 0;

DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (28938);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`) VALUES
(28938, 1, 1, 0, 1);

 -- Depleted War Golem smart ai
SET @ENTRY := 27017;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2701700);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 38, 0, 100, 0, 0, 1, 4000, 4000, 80, 2701700, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[0] set to 1 (wait 4000 - 4000 ms before next event trigger) - Self: Start timed action list id #Depleted War Golem #0 (2701700) (update always) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 19, 26407, 25, 1, 0, 0, 0, 0, 'After 0 seconds - Self: Move to Closest dead creature Lightning Sentry (26407) in 25 yards'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 0 to invoker'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 11, 47799, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Cast spell  Charge War Golem (47799) on Self'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 5 seconds - Self: Talk 1 to invoker'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 47797, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Cast spell  War Golem Charge Credit (47797) on Owner/Summoner'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Evade'),
(@ENTRY, 0, 1, 0, 31, 0, 100, 0, 47797, 0, 0, 0, 41, 2000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell War Golem Charge Credit (47797)  hit a target - Self: Despawn in 2 s');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 27017 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 2, 27017, 0, 0, 48, 0, 12138, 0, 10, 0, 'Action invoker 0 quest ... Or Maybe We Don\'t (12138) objective == 10'),
(22, 2, 27017, 0, 1, 48, 0, 12198, 0, 10, 0, 'Action invoker 0 quest ... Or Maybe We Don\'t (12198) objective == 10');

 -- Horrified Drakkari Warrior smart ai
SET @ENTRY := 26582;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 63, 0, 100, 0, 0, 0, 0, 0, 48, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just created - Self: Set active'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 88, 2658200, 2658201, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just created - Self: Call random timed action list between range Horrified Drakkari Warrior #0 (2658200) and Horrified Drakkari Warrior #1 (2658201) (update always)'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just created - Self: Talk 0 to invoker'),
(@ENTRY, 0, 3, 4, 58, 0, 100, 0, 0, 0, 0, 0, 41, 15000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On any waypoint of any path ended - Self: Despawn in 15 s'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 89, 20, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On any waypoint of any path ended - Self: Move randomly in radius 20 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 26582 AND `SourceId` = 0;

 -- Horrified Drakkari Shaman smart ai
SET @ENTRY := 26583;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 63, 0, 100, 0, 0, 0, 0, 0, 48, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just created - Self: Set active'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 88, 2658200, 2658201, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just created - Self: Call random timed action list between range Horrified Drakkari Warrior #0 (2658200) and Horrified Drakkari Warrior #1 (2658201) (update always)'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just created - Self: Talk 0 to invoker'),
(@ENTRY, 0, 3, 4, 58, 0, 100, 0, 0, 0, 0, 0, 41, 15000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On any waypoint of any path ended - Self: Despawn in 15 s'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 89, 20, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On any waypoint of any path ended - Self: Move randomly in radius 20 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 26583 AND `SourceId` = 0;
