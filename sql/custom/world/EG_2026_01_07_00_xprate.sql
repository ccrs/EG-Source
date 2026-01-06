--
DELETE FROM `command` WHERE `name` IN ('settings xprate');
INSERT INTO `command` (`name`, `help`) VALUES
('settings xprate', 'Syntax: .settings xprate [1..7] - This configuration modifies the XP rate related to your current character.');
