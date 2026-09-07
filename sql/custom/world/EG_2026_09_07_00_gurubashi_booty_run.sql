--
DELETE FROM `game_event` WHERE `eventEntry` = 86;
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `holidayStage`, `description`, `world_event`, `announce`) VALUES
(86, '2016-10-29 03:58:30', '2030-12-31 07:00:00', 180, 2, 0, 0, 'Gurubashi Arena Booty Run Intro', 0, 0);

DELETE FROM `game_event_local_schedule` WHERE `eventEntry` = 86;
INSERT INTO `game_event_local_schedule` (`eventEntry`, `enabled`, `comment`) VALUES
(86, 1, 'Gurubashi Arena Booty Run Intro');

--
DELETE FROM `gameobject` WHERE `guid` = 300000;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(300000, 179697, 0, 0, 0, 1, 1, -13204.1, 277.168, 21.9821, 0, 0, 0, 0, 1, 7200, 100, 1);

DELETE FROM `game_event_gameobject` WHERE `guid` = 300000;
INSERT INTO `game_event_gameobject` (`eventEntry`, `guid`) VALUES
(16, 300000);

--
UPDATE `smart_scripts` SET `event_param1` = 86, `comment` = 'Short John Mithril - On Game Event 86 Started - Start Waypoint' WHERE `entryorguid` = 14508 AND `source_type` = 0 AND `id` = 0;
UPDATE `smart_scripts` SET `link` = 0 WHERE `entryorguid` = 14508 AND `source_type` = 0 AND `id` = 2;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 14508 AND `source_type` = 0 AND `id` = 3;
