--
DELETE FROM `npc_vendor` WHERE `entry` = 33963 AND  `item` = 42852;
DELETE FROM `npc_vendor` WHERE `entry` = 33964 AND  `item` = 42852;
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `VerifiedBuild`) VALUES
(33963, 0, 42852, 0, 0, 2631, 0),
(33964, 0, 42852, 0, 0, 2631, 0);

UPDATE `waypoint_data` SET `position_z` = 132.0 WHERE `id` = 227985 AND `point` IN (1, 3);
UPDATE `smart_scripts` SET `target_z` = 132.0 WHERE `entryorguid` = 2801600 AND `source_type` = 9 AND `id` = 11;

--
UPDATE `creature` SET `MovementType`=0, `wander_distance`=0 WHERE `guid`=127422;
