--
-- Database `dsc_db`
--
CREATE DATABASE dsc_db;
USE dsc_db;
--
-- Table structure for table `log_tbl`
--
CREATE TABLE `log_tbl` (
  `user` varchar(32) NOT NULL DEFAULT '',
  `location` varchar(32) NOT NULL DEFAULT '',
  `create_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `comment` varchar(64) NOT NULL DEFAULT '',
  PRIMARY KEY (`create_time`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;
--
-- Table structure for table `user_tbl`
--
CREATE TABLE `user_tbl` (
  `user_id` varchar(10) NOT NULL DEFAULT '',
  `user_passwd` varchar(6) NOT NULL DEFAULT '',
  `comment` varchar(64) NOT NULL DEFAULT '',
  PRIMARY KEY (`user_id`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;
--
-- Grants for database `dsc_db`
--
GRANT ALL ON dsc_db.* TO 'dscadmin'@'localhost' IDENTIFIED BY 'dscpasswd';