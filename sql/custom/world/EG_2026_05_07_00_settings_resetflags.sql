--
DELETE FROM `command` WHERE `name` IN ('settings resetflags');
INSERT INTO `command` (`name`, `help`) VALUES
('settings resetflags', 'Syntax: .settings resetflags [playerName]\r\n\r\nResets all custom flags for the selected or named player to their default values.');
