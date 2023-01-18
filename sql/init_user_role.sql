USE game;

DELIMITER $$
CREATE PROCEDURE init_user_role() 
BEGIN
	DECLARE i INT UNSIGNED DEFAULT 1;
	WHILE i <= 20000 DO
		insert into user_role(`user_id`,`role_id`) values (i,i);
		SELECT i;
		SET i = i+1;
	END WHILE;
END $$
DELIMITER ;

CALL init_user_role();

