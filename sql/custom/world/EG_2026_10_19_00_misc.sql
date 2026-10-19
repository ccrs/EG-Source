--
DELETE FROM `vehicle_seat_addon` WHERE `SeatEntry` IN (2198, 2548, 2578);
INSERT INTO `vehicle_seat_addon` (`SeatEntry`, `SeatOrientation`, `ExitParamX`, `ExitParamY`, `ExitParamZ`, `ExitParamO`, `ExitParamValue`, `AttachOffsetX`, `AttachOffsetY`, `AttachOffsetZ`) VALUES
(2198, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2548, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2578, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);

UPDATE `item_template` SET `spelltrigger_1` = 0 WHERE `entry` = 44092 AND `spellid_1` = 0; -- Reforged Truesilver Champion: heirloom dropped the base item proc
