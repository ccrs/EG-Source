--
DELETE FROM `command` WHERE `name` IN ('massreward');
INSERT INTO `command` (`name`, `help`) VALUES
('massreward', 'Syntax: .massreward <itemEntry> <sinceTimestamp> [count]\r\n\r\nSends <itemEntry> (optional stack <count>, default 1) via GM mail to the highest-level character on each account that has been active since the given Unix timestamp.\r\nExample: .massreward 49426 1744848000\r\nTip: use SELECT UNIX_TIMESTAMP(''YYYY-MM-DD'') to obtain the timestamp.');
