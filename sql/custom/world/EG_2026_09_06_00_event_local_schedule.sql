--
DROP TABLE IF EXISTS `game_event_local_schedule`;
CREATE TABLE `game_event_local_schedule` (
  `eventEntry` tinyint unsigned NOT NULL COMMENT 'game_event.eventEntry',
  `enabled` tinyint unsigned NOT NULL DEFAULT '1',
  `comment` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`eventEntry`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO `game_event_local_schedule` (`eventEntry`, `enabled`, `comment`) VALUES
(14, 1, 'Stranglethorn Fishing Extravaganza Announce - must stay paired with events 15 and 62'),
(16, 1, 'Gurubashi Arena Booty Run - chest spawns every 3 hours on a local clock'),
(25, 1, 'Nights - 12 hour night window, meaningless if it slides against local time'),
(32, 1, 'L70ETC Concert'),
(33, 1, 'Dalaran: Minigob'),
(46, 1, 'L70 ETC Shattrath Announce - must stay paired with event 47'),
(47, 1, 'L70 ETC Shattrath'),
(62, 1, 'Stranglethorn Fishing Extravaganza Turn-ins - must stay paired with events 14 and 15'),
(63, 1, 'Kaluak Fishing Derby Turn-ins - must stay paired with event 64'),
(65, 1, 'Perry Gatner'),
(76, 1, 'Children of Goldshire'),
(79, 1, 'Diurnal fishing event - the whole point is that it runs during local daylight'),
(80, 1, 'Nocturnal fishing event - the whole point is that it runs during local night'),
(81, 1, 'L70ETC Grim Guzzler');
