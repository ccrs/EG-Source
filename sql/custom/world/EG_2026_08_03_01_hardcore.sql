-- add hardcore list command info
DELETE FROM `command` WHERE `name` IN ('hardcore', 'hardcore list');
INSERT INTO `command` (`name`, `help`) VALUES
('hardcore', "Syntax: .hardcore $subcommand
Type .hardcore to see the list of possible subcommands"),
('hardcore list', "Usage: .hardcore list
Lists all characters with Hardcore mode active that are still alive.");
