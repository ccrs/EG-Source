--
-- revert cf08a64
UPDATE `creature_template` SET `unit_flags` = unit_flags & ~0x04000000 WHERE `entry` = 30204;
