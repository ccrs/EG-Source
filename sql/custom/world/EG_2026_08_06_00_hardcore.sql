-- add hardcore milestone strings
DELETE FROM `trinity_string` WHERE `entry` IN (20017, 20018);
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(20017, '|cffff8000[HARDCORE]|r Milestone reached: level %u. A reward has been sent to your mailbox. Survive to level %u to claim the next one.'),
(20018, '|cffff8000[HARDCORE]|r Level %u. The one-life journey is complete. Your final reward has been sent - wear it proudly.');
