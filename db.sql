--
-- Database `room_db`
--
CREATE DATABASE room_db;
USE room_db;
--
-- Table structure for table `log_tbl`
--
CREATE TABLE `log_tbl` (
  `addr_a` varchar(32) NOT NULL DEFAULT '',
  `port_a` varchar(32) NOT NULL DEFAULT '',
  `addr_b` varchar(32) NOT NULL DEFAULT '',
  `port_b` varchar(32) NOT NULL DEFAULT '',
  `create_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `comment` varchar(64) NOT NULL DEFAULT '',
  PRIMARY KEY (`create_time`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;
--
-- Table structure for table `room_tbl`
--
CREATE TABLE `room_tbl` (
  `room_id` varchar(32) NOT NULL DEFAULT '',
  `addr_s` varchar(32) NOT NULL DEFAULT '',
  `port_s` varchar(32) NOT NULL DEFAULT '',
  `addr_a` varchar(32) NOT NULL DEFAULT '',
  `port_a` varchar(32) NOT NULL DEFAULT '',
  `addr_b` varchar(32) NOT NULL DEFAULT '',
  `port_b` varchar(32) NOT NULL DEFAULT '',
  `stream_a` varchar(32) NOT NULL DEFAULT '',
  `stream_b` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`room_id`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;
--
-- Grants for database `room_db`
--
GRANT ALL ON room_db.* TO 'roomadmin'@'localhost' IDENTIFIED BY 'roompasswd';