--
DROP TABLE IF EXISTS `game_event_holiday_rule`;
CREATE TABLE `game_event_holiday_rule` (
  `holiday` int unsigned NOT NULL COMMENT 'Holidays.dbc id',
  `ruleType` tinyint unsigned NOT NULL DEFAULT '0' COMMENT '0 fixed date, 1 nth weekday, 2 first weekday of month, 3 weekday on or after, 4 easter offset, 5 lunar new year, 6 first weekday of quarter month, 7 weekly weekday',
  `month` tinyint unsigned NOT NULL DEFAULT '0' COMMENT '1-12',
  `day` tinyint unsigned NOT NULL DEFAULT '0' COMMENT '1-31',
  `weekday` tinyint unsigned NOT NULL DEFAULT '0' COMMENT '0 Sunday to 6 Saturday',
  `dayOffset` smallint NOT NULL DEFAULT '0' COMMENT 'Days added after the base date is resolved',
  `startHour` tinyint unsigned NOT NULL DEFAULT '0' COMMENT 'Server local hour the event starts at',
  `startMinute` tinyint unsigned NOT NULL DEFAULT '0',
  `occurrence` tinyint unsigned NOT NULL DEFAULT '0' COMMENT 'nth for ruleType 1, month modulus for ruleType 6',
  `enabled` tinyint unsigned NOT NULL DEFAULT '1',
  `comment` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`holiday`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO `game_event_holiday_rule` (`holiday`, `ruleType`, `month`, `day`, `weekday`, `dayOffset`, `startHour`, `startMinute`, `occurrence`, `enabled`, `comment`) VALUES
(62,  0, 7,  4,  0,  0, 11, 0, 0, 1, 'Fireworks Spectacular - July 4, hour kept from the previous game_event start_time'),
(141, 0, 12, 15, 0,  0, 6,  0, 0, 1, 'Winter Veil - December 15 06:00 per dbc wildcard'),
(181, 4, 0,  0,  0,  0, 0,  0, 0, 1, 'Noblegarden - Easter Sunday, matches 9 of 10 dbc rows, 2009 is a Blizzard outlier'),
(201, 3, 4,  29, 0,  0, 0,  0, 0, 1, 'Childrens Week - first Sunday on or after April 29, matches 9 of 9 dbc rows'),
(301, 7, 0,  0,  0,  0, 15, 0, 0, 1, 'Stranglethorn Fishing Extravaganza - Sunday 15:00, matching game_event 62 turn-ins so the set stays together'),
(324, 0, 10, 18, 0,  0, 1,  0, 0, 1, 'Hallows End - October 18 01:00 per dbc wildcard'),
(327, 5, 0,  0,  0, -2, 0,  0, 0, 1, 'Lunar Festival - Chinese New Year minus 2 days, matches 4 of 4 dbc rows 2006-2009'),
(341, 0, 6,  21, 0,  0, 0,  0, 0, 1, 'Midsummer Fire Festival - June 21 per dbc wildcard'),
(372, 0, 9,  13, 0,  0, 0,  0, 0, 1, 'Brewfest - September 13 per dbc wildcard, stage 2 offsets to September 20'),
(374, 6, 0,  0,  5,  0, 0,  0, 0, 1, 'Darkmoon Faire Elwynn - first Friday of Mar/Jun/Sep/Dec, matches 11 of 13 dbc rows'),
(375, 6, 0,  0,  5,  0, 0,  0, 1, 1, 'Darkmoon Faire Mulgore - first Friday of Jan/Apr/Jul/Oct'),
(376, 6, 0,  0,  5,  0, 0,  0, 2, 1, 'Darkmoon Faire Terokkar - first Friday of Feb/May/Aug/Nov, matches the clean 2008 dbc block'),
(398, 0, 9,  19, 0,  0, 0,  0, 0, 1, 'Pirates Day - September 19 per dbc wildcard'),
(404, 1, 11, 0,  4, -4, 1,  0, 4, 1, 'Pilgrims Bounty - Sunday before the 4th Thursday of November, matches 4 of 4 dbc rows'),
(409, 0, 11, 1,  0,  0, 1,  0, 0, 1, 'Day of the Dead - November 1 01:00 per dbc wildcard'),
(423, 2, 2,  0,  0,  0, 0,  0, 0, 1, 'Love is in the Air - first Sunday of February, matches 4 of 4 dbc rows'),
(424, 7, 0,  0,  6,  0, 15, 0, 0, 1, 'Kaluak Fishing Derby - Saturday 15:00, matching game_event 63 turn-ins so the set stays together');

DELETE FROM `holiday_dates` WHERE `id` IN (181, 201, 327, 374, 375, 376, 404, 423);

UPDATE `game_event` SET `end_time` = '2037-12-31 00:00:00' WHERE `holiday` != 0 AND `end_time` = '2030-12-31 07:00:00';
