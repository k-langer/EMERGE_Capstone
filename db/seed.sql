-- This file must import into database named capstone

-- use the following command to import this sql file into capstone db
-- mysql -u zhen --pass="Zhenjiang#1" capstone < seed.sql

-- create tables

DROP TABLE IF EXISTS xyz_tmp;
CREATE TABLE xyz_tmp (
    id INT NOT NULL AUTO_INCREMENT,
    value VARCHAR(100) NOT NULL,
    output VARCHAR(100) NOT NULL,
    PRIMARY KEY (id)
);

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

-- craete procedures

DROP PROCEDURE IF EXISTS get_xyz;
delimiter /
CREATE PROCEDURE get_xyz(OUT rs CHAR(100))
BEGIN
    DECLARE val_id INT;
    IF(EXISTS(SELECT value FROM xyz LIMIT 1)) THEN
        SELECT value, id INTO rs, val_id FROM xyz ORDER BY id DESC LIMIT 1;
        UPDATE xyz_tmp set output="1" where id = val_id;
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
        SELECT input, id INTO rs, val_id FROM command ORDER BY id ASC LIMIT 1;
        DELETE FROM command WHERE id = val_id LIMIT 1;
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

DROP PROCEDURE IF EXISTS add_ui_input;
delimiter /
CREATE PROCEDURE add_ui_input(IN input CHAR(100))
BEGIN
     INSERT INTO ui_xyz VALUES(DEFAULT, input);
END/
delimiter ;

DROP PROCEDURE IF EXISTS get_ui_data;
delimiter /
CREATE PROCEDURE get_ui_data(OUT rs CHAR(100))
BEGIN
    DECLARE val_id INT;
    IF(EXISTS(SELECT value FROM ui_xyz LIMIT 1)) THEN
        SELECT value, id INTO rs, val_id FROM ui_xyz ORDER BY id ASC LIMIT 1;
        DELETE FROM ui_xyz WHERE id = val_id LIMIT 1;
    END IF;
END/
delimiter ;
