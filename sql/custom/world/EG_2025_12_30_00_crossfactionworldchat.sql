--
DELETE FROM `command` WHERE `name` IN ('settings worldChat');
INSERT INTO `command` (`name`, `help`) VALUES
('settings worldChat', 'Syntax: .settings worldChat [0/1] - Enables/Disables crossfaction world chat');
