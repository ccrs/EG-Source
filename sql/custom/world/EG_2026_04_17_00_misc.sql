--
DELETE FROM `command` WHERE `name` IN ('settings visuals');
INSERT INTO `command` (`name`, `help`) VALUES
('settings visuals', 'Syntax: .settings visuals [0/1] - Enables/Disables alternative character visuals.');
