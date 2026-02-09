--
DELETE FROM `command` WHERE `name` IN ('settings accountMount', 'settings account', 'settings account mount', 'settings account riding');
INSERT INTO `command` (`name`, `help`) VALUES
('settings account', 'Holds different account related settings.'),
('settings account mount', 'Syntax: .settings account mount [0/1] - Enables/Disables account mount transfering on character login.'),
('settings account riding', 'Syntax: .settings account riding [0/1] - Enables/Disables account riding training transfering on character login.');
