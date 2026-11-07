--
UPDATE `creature_template` SET `unit_flags` = `unit_flags` | 32768 WHERE `entry` IN (3654, 5762, 5763);
