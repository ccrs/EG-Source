--
DELETE FROM `vehicle_seat_addon` WHERE `SeatEntry` IN (1874, 2081, 2187, 2192, 2204, 2205, 2224, 2304, 2305);
INSERT INTO `vehicle_seat_addon` (`SeatEntry`, `SeatOrientation`, `ExitParamX`, `ExitParamY`, `ExitParamZ`, `ExitParamO`, `ExitParamValue`, `AttachOffsetX`, `AttachOffsetY`, `AttachOffsetZ`) VALUES
(1874, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2081, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2187, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2192, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2204, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2205, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2224, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2304, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL),
(2305, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
