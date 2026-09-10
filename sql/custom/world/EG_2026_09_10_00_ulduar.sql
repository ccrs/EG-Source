--
UPDATE `creature_template` SET `unit_flags` = (`unit_flags` | 768) & ~65536 WHERE `entry` IN (33436, 33437, 33441, 33442, 33495, 33523, 33535, 33536, 33962);
