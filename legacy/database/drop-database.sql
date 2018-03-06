--
-- Re-creating database
--

-- This line to remove old database
DROP DATABASE IF EXISTS boltek_flashes_raw_data;
DROP DATABASE IF EXISTS lightning_detection_system;
CREATE SCHEMA lightning_detection_system DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
