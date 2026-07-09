--
ALTER TABLE `vehicle_seat_addon`
    ADD COLUMN `AttachOffsetX` FLOAT DEFAULT NULL AFTER `ExitParamValue`,
    ADD COLUMN `AttachOffsetY` FLOAT DEFAULT NULL AFTER `AttachOffsetX`,
    ADD COLUMN `AttachOffsetZ` FLOAT DEFAULT NULL AFTER `AttachOffsetY`;

DELETE FROM `vehicle_seat_addon` WHERE `SeatEntry` IN (381, 382, 383, 384, 482, 483, 946, 1652, 2197, 2504, 2553, 2565, 2726, 2727, 2728, 2729, 2764, 2765, 2767, 2768, 2866, 3009, 3013, 3043, 3065, 3070, 3071, 3072, 3146, 3246, 3832, 3833, 3834, 7739);
INSERT INTO `vehicle_seat_addon` (`SeatEntry`, `SeatOrientation`, `ExitParamX`, `ExitParamY`, `ExitParamZ`, `ExitParamO`, `ExitParamValue`, `AttachOffsetX`, `AttachOffsetY`, `AttachOffsetZ`) VALUES
(381, 0, 0, 0, 0, 0, 0, -4.2169, -3.7193, 1.2374), -- Vrykul Landing Ship (25762) seat 0
(382, 0, 0, 0, 0, 0, 0, -4.2169, 3.6858, 1.2374), -- Vrykul Landing Ship (25762) seat 1
(383, 0, 0, 0, 0, 0, 0, 4.9681, -3.7187, 1.2357), -- Vrykul Landing Ship (25762) seat 2
(384, 0, 0, 0, 0, 0, 0, 4.9681, 3.7183, 1.2357), -- Vrykul Landing Ship (25762) seat 3
(482, 0, 0, 0, 0, 0, 0, 0.0652, -4.0196, 1.2366), -- Vrykul Landing Ship (25762) seat 5
(483, 0, 0, 0, 0, 0, 0, 0.0652, 4.1404, 1.2366), -- Vrykul Landing Ship (25762) seat 6
(946, 0, 0, 0, 0, 0, 0, -4.2591, -0.8188, 1.5137), -- Alliance Steam Tank (27587) seat 1
(1652, 0, 0, 0, 0, 0, 0, -1.7912, -0.0168, 4.8236), -- Wintergrasp Siege Engine (28312) seat 7
(2197, 0, 0, 0, 0, 0, 0, 0.4380, -0.0245, -0.0731), -- Brann's Flying Machine (30134) seat 0
(2504, 0, 0, 0, 0, 0, 0, 0.3834, -1.1622, -0.5139), -- Frostbrood Skytalon (31583) seat 1
(2553, 0, 0, 0, 0, 0, 0, -1.0272, 0.0000, 0.3223), -- Kor'kron Suppression Turret (31884) seat 0
(2565, 0, 0, 0, 0, 0, 0, 1.0368, -0.0216, -0.3092), -- Skybreaker Recon Fighter (32189) seat 0
(2726, 0, 0, 0, 0, 0, 0, 0.0652, -4.0196, 1.2366), -- The Helmsman's Ship (27939) seat 0
(2727, 0, 0, 0, 0, 0, 0, 0.0652, 4.1404, 1.2366), -- The Helmsman's Ship (27939) seat 1
(2728, 0, 0, 0, 0, 0, 0, 4.9681, 3.7183, 1.2357), -- The Helmsman's Ship (27939) seat 2
(2729, 0, 0, 0, 0, 0, 0, 4.9681, -3.7187, 1.2357), -- The Helmsman's Ship (27939) seat 3
(2764, 0, 0, 0, 0, 0, 0, -1.2784, 0.9340, 3.1201), -- Traveler's Tundra Mammoth (32633) seat 1
(2765, 0, 0, 0, 0, 0, 0, -1.3065, -0.7816, 3.2895), -- Traveler's Tundra Mammoth (32633) seat 2
(2767, 0, 0, 0, 0, 0, 0, -1.2784, 0.9340, 3.1201), -- Traveler's Tundra Mammoth (32640) seat 1
(2768, 0, 0, 0, 0, 0, 0, -1.3065, -0.7816, 3.2895), -- Traveler's Tundra Mammoth (32640) seat 2
(2866, 0, 0, 0, 0, 0, 0, -1.7912, -0.0168, 4.8236), -- Wintergrasp Siege Engine (32627) seat 7
(3009, 0, 0, 0, 0, 0, 0, -1.7912, -0.0168, 4.8236), -- Salvaged Siege Engine (33060) seat 7
(3013, 0, 0, 0, 0, 0, 0, 1.4620, 2.4437, 3.4097), -- Salvaged Demolisher (33109) seat 2
(3043, 5.7595863, 0, 0, 0, 0, 0, -2.1573, 8.9496, 15.5097), -- Flame Leviathan (25) - Seat Front L
(3065, 0.52359915, 0, 0, 0, 0, 0, -2.1573, -9.0504, 15.5097), -- Flame Leviathan (25) - Seat Front R
(3070, 0, 0, 0, 0, 0, 0, -3.9573, -0.0504, 16.7097), -- Flame Leviathan (25) - Static Cannon
(3071, 3.1415925, 0, 0, 0, 0, 0, 8.0427, 4.7496, 16.7097), -- Flame Leviathan (25) - Seat Back L
(3072, 3.1415925, 0, 0, 0, 0, 0, 8.0427, -4.8504, 16.7097), -- Flame Leviathan (25) - Seat Back R
(3146, 0, 0, 0, 0, 0, 0, 1.5418, -2.4857, 3.4117), -- Salvaged Demolisher (33109) seat 1
(3246, 0, 0, 0, 0, 0, 0, 1.3890, -0.0150, 20.1499), -- XT-002 Deconstructor (33293) seat 0
(3832, 3.1415925, 0, 0, 0, 0, 0, 8.0427, 4.7496, 16.7097), -- Flame Leviathan (10) - Seat Back L
(3833, 3.1415925, 0, 0, 0, 0, 0, 8.0427, -4.8504, 16.7097), -- Flame Leviathan (10) - Seat Back R
(3834, 0, 0, 0, 0, 0, 0, -3.9573, -0.0504, 16.7097), -- Flame Leviathan (10) - Static Cannon
(7739, 0, 0, 0, 0, 0, 0, -1.3696, 0.0000, 1.2631); -- Tankbuster Cannon (39759) seat 0
