--
DELETE FROM `npc_vendor` WHERE `entry` = 33963 AND  `item` = 42852;
DELETE FROM `npc_vendor` WHERE `entry` = 33964 AND  `item` = 42852;
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `VerifiedBuild`) VALUES
(33963, 0, 42852, 0, 0, 2631, 0),
(33964, 0, 42852, 0, 0, 2631, 0);
