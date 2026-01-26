--
DELETE FROM `command` WHERE `name` IN ('settings accountMount');
INSERT INTO `command` (`name`, `help`) VALUES
('settings accountMount', 'Syntax: .settings accountMount [0/1] - Enables/Disables account mount transfering on character login.');
