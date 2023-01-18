USE game;

DELIMITER $$
CREATE PROCEDURE init_user() 
BEGIN
	DECLARE i INT UNSIGNED DEFAULT 1;
	WHILE i <= 20000 DO
		insert into user(`username`, `password`) values(concat('user', i), 'password');
		SELECT i;
		SET i = i+1;
	END WHILE;
END $$
DELIMITER ;

CALL init_user();
