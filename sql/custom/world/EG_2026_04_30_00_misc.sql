--
DELETE FROM `command` WHERE `name` IN ('account onlinelist countries', 'lookup player country');
INSERT INTO `command` (`name`, `help`) VALUES
('account onlinelist countries',    'Syntax: .account onlinelist countries\r\n\r\nPrints a summary of currently online players grouped by country, showing total and staff counts per country.'),
('lookup player country',           'Syntax: .lookup player country $countryCodeOrName\r\n\r\nLists all currently online players whose IP resolves to the given country. Accepts a two-letter country code (e.g. US) or a full country name (case-insensitive).');
