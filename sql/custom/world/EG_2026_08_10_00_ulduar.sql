--
-- Twilight Frost Mage (33819/33829)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33819, 33829);

SET @ENTRY := 33819;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 3000, 2000, 4000, 11, 63913, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 2 - 4 seconds (1 - 3s initially) (IC) - Self: Cast spell  Frostbolt (63913) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 6000, 10000, 12000, 18000, 11, 63758, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 12 - 18 seconds (6 - 10s initially) (IC) - Self: Cast spell  Frost Bolt Volley (63758) on Self'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 12000, 16000, 20000, 28000, 11, 64663, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 20 - 28 seconds (12 - 16s initially) (IC) - Self: Cast spell  Arcane Burst (64663) on Self'),
(@ENTRY, 0, 3, 4, 9, 0, 100, 0, 0, 8, 15000, 25000, 11, 63912, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 8 yards (cooldown 15000 - 25000 ms) - Self: Cast spell  Frost Nova (63912) on Self'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 64662, 0, 0, 0, 0, 0, 28, 100, 1, 0, 0, 0, 0, 0, 'When victim in range 0 - 8 yards (cooldown 15000 - 25000 ms) - Self: Cast spell  Blink (64662) on Farthest player within 100 yards on the threat list');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33819 AND `SourceId` = 0;

-- Twilight Pyromancer (33820/33830)
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33820, 33830);

SET @ENTRY := 33820;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 3000, 2000, 4000, 11, 63789, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 2 - 4 seconds (1 - 3s initially) (IC) - Self: Cast spell  Fireball (63789) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 8000, 12000, 15000, 25000, 11, 63775, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 25 seconds (8 - 12s initially) (IC) - Self: Cast spell  Flamestrike (63775) on Random hostile'),
(@ENTRY, 0, 2, 0, 37, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On AI initialize - Set event phase to phase 1'),
(@ENTRY, 0, 3, 0, 1, 1, 100, 0, 1000, 2000, 5000, 5000, 11, 63774, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 5 seconds (1 - 2s initially) (OOC) - Self: Cast spell  Summon Fire Elemental (63774) on Self'),
(@ENTRY, 0, 4, 0, 17, 0, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On summoned anyunit - Set event phase to phase 2'),
(@ENTRY, 0, 5, 0, 82, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature died - Set event phase to phase 1'),
(@ENTRY, 0, 6, 0, 35, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature despawn - Set event phase to phase 1'),
(@ENTRY, 0, 7, 0, 9, 0, 100, 0, 0, 8, 15000, 25000, 11, 64662, 0, 0, 0, 0, 0, 28, 100, 1, 0, 0, 0, 0, 0, 'When victim in range 0 - 8 yards (cooldown 15000 - 25000 ms) - Self: Cast spell  Blink (64662) on Farthest player within 100 yards on the threat list');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33820 AND `SourceId` = 0;

-- Enslaved Fire Elemental (33838/33839) - summoned by the Pyromancer via 63774, no longer spawned in the world
DELETE FROM `creature_addon` WHERE `guid` IN (137557, 137558, 137559);
DELETE FROM `creature` WHERE `guid` IN (137557, 137558, 137559);

UPDATE `creature_template` SET `mechanic_immune_mask` = 550189951 WHERE `entry` IN (33838, 33839);

SET @ENTRY := 33838;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 11, 63778, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Cast spell  Fire Shield (63778) on Self'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 9000, 12000, 18000, 11, 38064, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 12 - 18 seconds (5 - 9s initially) (IC) - Self: Cast spell  Blast Wave (38064) on Self'),
(@ENTRY, 0, 2, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33838 AND `SourceId` = 0;

--
UPDATE `creature` SET `position_x` = 1795.5156, `position_y` = 123.2986, `position_z` = 342.4612, `orientation` = 5.651392 WHERE `guid` = 137538;
UPDATE `creature` SET `position_x` = 1886.5798, `position_y` = 122.2934, `position_z` = 342.4619, `orientation` = 4.021912 WHERE `guid` = 137539;
UPDATE `creature` SET `position_x` = 1906.4045, `position_y` = 122.7715, `position_z` = 412.3273, `orientation` = 4.415683 WHERE `guid` = 137540;
UPDATE `creature` SET `position_x` = 1837.0955, `position_y` = 117.0365, `position_z` = 341.8062, `orientation` = 5.126954 WHERE `guid` = 137541;
UPDATE `creature` SET `position_x` = 1792.3195, `position_y` = 131.8472, `position_z` = 342.4611, `orientation` = 5.587723 WHERE `guid` = 137542;
UPDATE `creature` SET `position_x` = 1786.5139, `position_y` = 109.9358, `position_z` = 342.4649, `orientation` = 5.876401 WHERE `guid` = 137543;
UPDATE `creature` SET `position_x` = 1894.2448, `position_y` = 123.4774, `position_z` = 342.4634, `orientation` = 3.934547 WHERE `guid` = 137544;
UPDATE `creature` SET `position_x` = 1849.3091, `position_y` = 127.5451, `position_z` = 342.0558, `orientation` = 4.787535 WHERE `guid` = 137545;
UPDATE `creature` SET `position_x` = 1850.5469, `position_y` = 118.3333, `position_z` = 341.9373, `orientation` = 4.772872 WHERE `guid` = 137546;
UPDATE `creature` SET `position_x` = 1799.1442, `position_y` = 114.7396, `position_z` = 342.3671, `orientation` = 5.726883 WHERE `guid` = 137547;
UPDATE `creature` SET `position_x` = 1878.4098, `position_y` = 120.5017, `position_z` = 342.4611, `orientation` = 4.132428 WHERE `guid` = 137548;
UPDATE `creature` SET `position_x` = 1843.2673, `position_y` = 122.6806, `position_z` = 341.5329, `orientation` = 4.938898 WHERE `guid` = 137549;
UPDATE `creature` SET `position_x` = 1783.4427, `position_y` = 118.7396, `position_z` = 342.4641, `orientation` = 5.789063 WHERE `guid` = 137550;
UPDATE `creature` SET `position_x` = 1898.0885, `position_y` = 131.7517, `position_z` = 342.4644, `orientation` = 3.979852 WHERE `guid` = 137551;
UPDATE `creature` SET `position_x` = 1899.7867, `position_y` = 122.4753, `position_z` = 412.3273, `orientation` = 5.113815 WHERE `guid` = 137552;
UPDATE `creature` SET `position_x` = 1848.4879, `position_y` = 136.6319, `position_z` = 341.9086, `orientation` = 4.790001 WHERE `guid` = 137553;
UPDATE `creature` SET `position_x` = 1900.6119, `position_y` = 129.7246, `position_z` = 412.3273, `orientation` = 4.886922 WHERE `guid` = 137554;
UPDATE `creature` SET `position_x` = 1890.6997, `position_y` = 114.8837, `position_z` = 342.4624, `orientation` = 3.865214 WHERE `guid` = 137555;
UPDATE `creature` SET `position_x` = 1907.4163, `position_y` = 129.8802, `position_z` = 412.3273, `orientation` = 4.485496 WHERE `guid` = 137556;

--
DELETE FROM `creature_formations` WHERE `leaderGUID` IN (137550, 137553, 137551, 137552);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`, `point_1`, `point_2`) VALUES
(137550, 137550, 0, 0, 3, 0, 0),
(137550, 137543, 9.32, 5.5422, 3, 0, 0),
(137550, 137542, 15.83, 1.4696, 3, 0, 0),
(137550, 137538, 12.91, 0.8552, 3, 0, 0),
(137550, 137547, 16.20, 0.2447, 3, 0, 0),
(137553, 137553, 0, 0, 3, 0, 0),
(137553, 137541, 22.67, 5.6790, 3, 0, 0),
(137553, 137549, 14.90, 5.8475, 3, 0, 0),
(137553, 137545, 9.12, 0.0125, 3, 0, 0),
(137553, 137546, 18.41, 0.0344, 3, 0, 0),
(137551, 137551, 0, 0, 3, 0, 0),
(137551, 137548, 22.67, 5.9643, 3, 0, 0),
(137551, 137539, 14.90, 6.1328, 3, 0, 0),
(137551, 137555, 18.42, 0.3197, 3, 0, 0),
(137551, 137544, 9.12, 0.2977, 3, 0, 0),
(137552, 137552, 0, 0, 3, 0, 0),
(137552, 137554, 7.30, 2.6268, 3, 0, 0),
(137552, 137540, 6.62, 1.2141, 3, 0, 0),
(137552, 137556, 10.63, 1.9398, 3, 0, 0);
