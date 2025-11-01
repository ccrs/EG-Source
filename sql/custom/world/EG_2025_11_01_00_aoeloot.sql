--
DELETE FROM `command` WHERE `name` IN ('settings aoeloot');
INSERT INTO `command` (`name`, `help`) VALUES
('settings aoeloot', 'Syntax: .settings aoeloot [0/1] - Enables/Disables AOE Loot (you will be able to get close-by creatures loot at once)');
