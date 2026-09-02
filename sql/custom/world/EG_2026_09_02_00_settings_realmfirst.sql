--
DELETE FROM `command` WHERE `name` IN ('settings realmfirst');
INSERT INTO `command` (`name`, `help`) VALUES
('settings realmfirst', 'Syntax: .settings realmfirst on|off [playerName]\r\n\r\nBlocks or unblocks realm first achievements for the selected or named player, exactly as if that character had used the XP rate setting. Works on offline characters.');

DELETE FROM `trinity_string` WHERE `entry` IN (20022, 20023, 20024);
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(20022, '|cffff0000[REALM FIRST]|r Your character is no longer eligible for realm first achievements.'),
(20023, '|cff00ff00[REALM FIRST]|r Your character is eligible for realm first achievements again.'),
(20024, '|cffff0000[REALM FIRST]|r %s is no longer eligible for realm first achievements.');
