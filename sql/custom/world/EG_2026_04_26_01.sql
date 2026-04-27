--
DELETE FROM `gameobject` WHERE `guid` IN (2134507, 2134508);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `ScriptName`, `StringId`, `VerifiedBuild`) VALUES
(2134507, 181510, 533, 0, 0, 3, 1, 2820.0, -3683.0, 274.0,  0, 0, 0, 0.980784, -0.195094, 0, 0, 1, NULL, NULL, 15354),
(2134508, 181510, 533, 0, 0, 3, 1, 2820.0, -3692.0, 275.5,  0, 0, 0, 0.980784, -0.195094, 0, 0, 1, NULL, NULL, 15354);
