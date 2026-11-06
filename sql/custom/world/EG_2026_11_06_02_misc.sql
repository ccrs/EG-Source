--
UPDATE `creature_template` SET `unit_flags2` = `unit_flags2` | 65536 WHERE `entry` IN (33113, 34003, 33114, 33139, 34111, 33142, 33143);

-- Flame Leviathan (25)
DELETE FROM `vehicle_template_accessory` WHERE `entry` = 34003 AND `seat_id` IN (0, 1);
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(34003, 33114, 0, 1, 'Flame Leviathan (25) - Seat Front L', 6, 30000),
(34003, 33114, 1, 1, 'Flame Leviathan (25) - Seat Front R', 6, 30000);

--
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` IN (33113, 34003, 33114, 33139, 34111, 33142, 33143);

-- Flame Leviathan Seat / Overload Control Device
UPDATE `creature_template` SET `modelid1` = 11686, `modelid2` = 0, `modelid3` = 0, `modelid4` = 0 WHERE `entry` = 33114;
UPDATE `creature_template` SET `modelid1` = 28469, `modelid2` = 0, `modelid3` = 0, `modelid4` = 0 WHERE `entry` = 33143;
