--
DELETE FROM `command` WHERE `name` IN ('settings account pet');
INSERT INTO `command` (`name`, `help`) VALUES
('settings account pet', 'Syntax: .settings account pet [0/1] - Enables/Disables account pet companions transfering on character login.');
