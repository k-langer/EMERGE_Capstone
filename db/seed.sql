-- This file must import into database named capstone


-- create tables

DROP TABLE IF EXISTS xyz;
CREATE TABLE xyz (
    id INT NOT NULL AUTO_INCREMENT,
    value VARCHAR(100) NOT NULL,
    PRIMARY KEY (id)
);

DROP TABLE IF EXISTS command;
CREATE TABLE command (
    id INT NOT NULL AUTO_INCREMENT,
    input VARCHAR(100) NOT NULL,
    PRIMARY KEY(id)
);

DROP TABLE IF EXISTS ui_xyz;
CREATE TABLE ui_xyz (
    id INT NOT NULL AUTO_INCREMENT,
    value VARCHAR(100) NOT NULL,
    PRIMARY KEY (id)
);

DROP TABLE IF EXISTS ui_command;
CREATE TABLE ui_command (
    id INT NOT NULL AUTO_INCREMENT,
    input VARCHAR(100) NOT NULL,
    PRIMARY KEY(id)
);

-- craete procedures

DROP PROCEDURE IF EXISTS get_xyz;
delimiter /
CREATE PROCEDURE get_xyz(OUT rs CHAR(100))
BEGIN
    DECLARE val_id INT;
    IF(EXISTS(SELECT value FROM xyz LIMIT 1)) THEN
        SELECT value INTO rs FROM xyz ORDER BY id DESC LIMIT 1;
        DELETE FROM xyz;
    END IF;
END/
delimiter ;

DROP PROCEDURE IF EXISTS ui_get_xyz;
delimiter /
CREATE PROCEDURE ui_get_xyz(OUT rs CHAR(100))
BEGIN
    DECLARE val_id INT;
    IF(EXISTS(SELECT value FROM ui_xyz LIMIT 1)) THEN
        SELECT value INTO rs FROM ui_xyz ORDER BY id DESC LIMIT 1;
        DELETE FROM ui_xyz;
    END IF;
END/
delimiter ;

DROP PROCEDURE IF EXISTS get_new_data;
delimiter /
CREATE PROCEDURE get_new_data(OUT rs CHAR(100))
BEGIN
    DECLARE val_id INT;
    IF(EXISTS(SELECT input FROM command LIMIT 1)) THEN
        SELECT input INTO rs FROM command ORDER BY id ASC LIMIT 1;
        DELETE FROM command;
    END IF;
END/
delimiter ;

DROP PROCEDURE IF EXISTS ui_get_new_data;
delimiter /
CREATE PROCEDURE ui_get_new_data(OUT rs CHAR(100))
BEGIN
    DECLARE val_id INT;
    IF(EXISTS(SELECT input FROM ui_command LIMIT 1)) THEN
        SELECT input INTO rs FROM ui_command ORDER BY id ASC LIMIT 1;
        DELETE FROM ui_command;
    END IF;
END/
delimiter ;

DROP PROCEDURE IF EXISTS add_input;
delimiter /
CREATE PROCEDURE add_input(IN input CHAR(100))
BEGIN
     INSERT INTO command VALUES(DEFAULT, input);
END/
delimiter ;





