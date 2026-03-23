--
DELETE FROM `command` WHERE `name` IN ('settings account flightPath');
INSERT INTO `command` (`name`, `help`) VALUES
('settings account flightPath', 'Syntax: .settings account flightPath [0/1] - Enables/Disables account flight path  (same faction) transfering on character login.');
