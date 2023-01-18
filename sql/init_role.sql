USE game;

DELIMITER $$
CREATE PROCEDURE init_role() 
BEGIN
	DECLARE i INT UNSIGNED DEFAULT 1;
	WHILE i <= 20000 DO
		insert into role(`name`) values(concat('user', i));
		SELECT i;
		SET i = i+1;
	END WHILE;
END $$
DELIMITER ;

CALL init_role();
