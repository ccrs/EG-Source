-- add masquerade command info
DELETE FROM `command` WHERE `name`='settings masquerade';
INSERT INTO `command` (`name`,`permission`,`help`) VALUES
('settings masquerade', 1003, "Usage: .settings masquerade #value
Permanently masquerades the character as the specified race.
Available setting values:
   0 - Disable masquerade and HIDE other player's selection
   1 - Human - ALLIANCE
   2 - Orc - HORDE
   3 - Dwarf - ALLIANCE
   4 - Night Elf - ALLIANCE
   5 - Undead - HORDE
   6 - Tauren - HORDE
   7 - Gnome - ALLIANCE
   8 - Troll - HORDE
   9 - Blood Elf - HORDE
  10 - Draenei - ALLIANCE");

DELETE FROM `trinity_string` WHERE `entry`= 20011;
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(20011, 'Race Masquerade enabled with value "%s"');
