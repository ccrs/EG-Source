-- 
DELETE FROM `command` WHERE `name`='settings weaponSkill';
INSERT INTO `command` (`name`, `help`) VALUES
('settings weaponSkill', "Usage: .settings weaponSkill [1/0] - Enables/Disables automatic weapon skills leveling");
