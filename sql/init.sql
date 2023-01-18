DROP DATABASE IF EXISTS game;
CREATE DATABASE game;

USE game;

CREATE TABLE user(
	`user_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
	`username` VARCHAR(128) NOT NULL,
	`password` VARCHAR(128) NOT NULL,
	PRIMARY KEY (`user_id`),
	UNIQUE (`username`)
);

CREATE TABLE role(
	`role_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
	`name` VARCHAR(30) NOT NULL,
	`item` MEDIUMBLOB,
	`logic` MEDIUMBLOB,
	PRIMARY KEY (`role_id`)
);

CREATE TABLE user_role(
	`user_id` INT UNSIGNED NOT NULL,
	`role_id` INT UNSIGNED NOT NULL,
	FOREIGN KEY (`user_id`) REFERENCES user (`user_id`),
	FOREIGN KEY (`role_id`) REFERENCES role (`role_id`)
);
