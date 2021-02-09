-- phpMyAdmin SQL Dump
-- version 4.6.6deb5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Feb 08, 2021 at 11:32 PM
-- Server version: 5.7.29-0ubuntu0.18.04.1
-- PHP Version: 7.2.24-0ubuntu0.18.04.3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `evony`
--

-- --------------------------------------------------------

--
-- Table structure for table `account`
--

CREATE TABLE `account` (
  `id` int(10) UNSIGNED NOT NULL,
  `name` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `email` varchar(80) COLLATE utf8_unicode_ci NOT NULL,
  `ip` varchar(16) COLLATE utf8_unicode_ci NOT NULL,
  `lastlogin` double NOT NULL DEFAULT '0',
  `creation` double NOT NULL,
  `password` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `status` int(10) NOT NULL DEFAULT '0',
  `reason` varchar(45) COLLATE utf8_unicode_ci NOT NULL DEFAULT ''
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `config_building`
--

CREATE TABLE `config_building` (
  `id` int(10) UNSIGNED NOT NULL,
  `buildingid` int(10) UNSIGNED NOT NULL,
  `level` int(10) UNSIGNED NOT NULL,
  `buildtime` int(10) UNSIGNED NOT NULL,
  `wood` int(10) UNSIGNED NOT NULL,
  `food` int(10) UNSIGNED NOT NULL,
  `stone` int(10) UNSIGNED NOT NULL,
  `iron` int(10) UNSIGNED NOT NULL,
  `gold` int(10) UNSIGNED NOT NULL,
  `prereqbuilding` varchar(45) NOT NULL,
  `prereqitem` varchar(45) NOT NULL,
  `prereqtech` varchar(45) NOT NULL,
  `inside` int(10) UNSIGNED NOT NULL,
  `blimit` int(10) UNSIGNED NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `config_building`
--

INSERT INTO `config_building` (`id`, `buildingid`, `level`, `buildtime`, `wood`, `food`, `stone`, `iron`, `gold`, `prereqbuilding`, `prereqitem`, `prereqtech`, `inside`, `blimit`) VALUES
(1, 1, 1, 75, 500, 100, 100, 50, 0, '0,0', '0,0', '', 1, 0),
(2, 1, 2, 150, 1000, 200, 200, 100, 0, '0,0', '0,0', '', 1, 0),
(3, 1, 3, 300, 2000, 400, 400, 200, 0, '31,2', '0,0', '', 1, 0),
(4, 1, 4, 600, 4000, 800, 800, 400, 0, '31,3', '0,0', '', 1, 0),
(5, 1, 5, 1200, 8000, 1600, 1600, 800, 0, '31,4', '0,0', '', 1, 0),
(6, 1, 6, 2400, 16000, 3200, 3200, 1600, 0, '31,5', '0,0', '', 1, 0),
(7, 1, 7, 4800, 32000, 6400, 6400, 3200, 0, '31,6', '0,0', '', 1, 0),
(8, 1, 8, 9600, 64000, 12800, 12800, 6400, 0, '31,7', '0,0', '', 1, 0),
(9, 1, 9, 19200, 128000, 25600, 25600, 12800, 0, '31,8', '0,0', '', 1, 0),
(10, 1, 10, 38400, 256000, 51200, 51200, 25600, 0, '31,9', '19,1', '', 1, 0),
(11, 2, 1, 300, 1200, 250, 1500, 500, 0, '29,1', '0,0', '', 1, 0),
(12, 2, 2, 600, 2400, 500, 3000, 1000, 0, '29,1', '0,0', '', 1, 0),
(13, 2, 3, 1200, 4800, 1000, 6000, 2000, 0, '29,1', '0,0', '', 1, 0),
(14, 2, 4, 2400, 9600, 2000, 12000, 4000, 0, '29,1', '0,0', '', 1, 0),
(15, 2, 5, 4800, 19200, 4000, 24000, 8000, 0, '29,1', '0,0', '', 1, 0),
(16, 2, 6, 9600, 38400, 8000, 48000, 16000, 0, '29,1', '0,0', '', 1, 0),
(17, 2, 7, 19200, 76800, 16000, 96000, 32000, 0, '29,1', '0,0', '', 1, 0),
(18, 2, 8, 38400, 153600, 32000, 192000, 64000, 0, '29,1', '0,0', '', 1, 0),
(19, 2, 9, 76800, 307200, 64000, 384000, 128000, 0, '29,1', '0,0', '', 1, 0),
(20, 2, 10, 153600, 614400, 128000, 768000, 256000, 0, '29,1', '19,1', '', 1, 0),
(21, 31, 1, 1800, 3000, 200, 2500, 200, 0, '0,0', '0,0', '', 1, 1),
(22, 31, 2, 3600, 6000, 400, 5000, 400, 0, '0,0', '0,0', '', 1, 1),
(23, 31, 3, 7200, 12000, 800, 10000, 800, 0, '32,1', '0,0', '', 1, 1),
(24, 31, 4, 14400, 24000, 1600, 20000, 1600, 0, '32,2', '0,0', '', 1, 1),
(25, 31, 5, 28800, 48000, 3200, 40000, 3200, 0, '32,3', '0,0', '', 1, 1),
(26, 31, 6, 57600, 96000, 6400, 80000, 6400, 0, '32,4', '0,0', '', 1, 1),
(27, 31, 7, 115200, 192000, 12800, 160000, 12800, 0, '32,5', '0,0', '', 1, 1),
(28, 31, 8, 230400, 384000, 25600, 320000, 25600, 0, '32,6', '0,0', '', 1, 1),
(29, 31, 9, 460800, 768000, 51200, 640000, 51200, 0, '32,7', '0,0', '', 1, 1),
(30, 31, 10, 921600, 1536000, 102400, 1280000, 102400, 0, '32,8', '19,1', '', 1, 1),
(31, 3, 1, 600, 1500, 100, 1000, 300, 0, '0,0', '0,0', '', 1, 0),
(32, 3, 2, 1200, 3000, 200, 2000, 600, 0, '0,0', '0,0', '', 1, 0),
(33, 3, 3, 2400, 6000, 400, 4000, 1200, 0, '0,0', '0,0', '', 1, 0),
(34, 3, 4, 4800, 12000, 800, 8000, 2400, 0, '0,0', '0,0', '', 1, 0),
(35, 3, 5, 9600, 24000, 1600, 16000, 4800, 0, '0,0', '0,0', '', 1, 0),
(36, 3, 6, 19200, 48000, 3200, 32000, 9600, 0, '0,0', '0,0', '', 1, 0),
(37, 3, 7, 38400, 96000, 6400, 64000, 19200, 0, '0,0', '0,0', '', 1, 0),
(38, 3, 8, 76800, 192000, 12800, 128000, 38400, 0, '0,0', '0,0', '', 1, 0),
(39, 3, 9, 153600, 384000, 25600, 256000, 76800, 0, '0,0', '0,0', '', 1, 0),
(40, 3, 10, 307200, 768000, 51200, 512000, 153600, 0, '0,0', '19,1', '', 1, 0),
(41, 21, 1, 240, 2000, 300, 1000, 400, 0, '1,2', '0,0', '', 1, 1),
(42, 21, 2, 480, 4000, 600, 2000, 800, 0, '1,2', '0,0', '', 1, 1),
(43, 21, 3, 960, 8000, 1200, 4000, 1600, 0, '1,2', '0,0', '', 1, 1),
(44, 21, 4, 1920, 16000, 2400, 8000, 3200, 0, '1,2', '0,0', '', 1, 1),
(45, 21, 5, 3840, 32000, 4800, 16000, 6400, 0, '1,2', '0,0', '', 1, 1),
(46, 21, 6, 7680, 64000, 9600, 32000, 12800, 0, '1,2', '0,0', '', 1, 1),
(47, 21, 7, 15360, 128000, 19200, 64000, 25600, 0, '1,2', '0,0', '', 1, 1),
(48, 21, 8, 30720, 256000, 38400, 128000, 51200, 0, '1,2', '0,0', '', 1, 1),
(49, 21, 9, 61440, 512000, 76800, 256000, 102400, 0, '1,2', '0,0', '', 1, 1),
(50, 21, 10, 122880, 1024000, 153600, 512000, 204800, 0, '1,2', '19,1', '', 1, 1),
(51, 27, 1, 300, 2500, 400, 1200, 700, 0, '21,1', '0,0', '', 1, 1),
(52, 27, 2, 600, 5000, 800, 2400, 1400, 0, '21,1', '0,0', '', 1, 1),
(53, 27, 3, 1200, 10000, 1600, 4800, 2800, 0, '21,1', '0,0', '', 1, 1),
(54, 27, 4, 2400, 20000, 3200, 9600, 5600, 0, '21,1', '0,0', '', 1, 1),
(55, 27, 5, 4800, 40000, 6400, 19200, 11200, 0, '21,1', '0,0', '', 1, 1),
(56, 27, 6, 9600, 80000, 12800, 38400, 22400, 0, '21,1', '0,0', '', 1, 1),
(57, 27, 7, 19200, 160000, 25600, 76800, 44800, 0, '21,1', '0,0', '', 1, 1),
(58, 27, 8, 38400, 320000, 51200, 153600, 89600, 0, '21,1', '0,0', '', 1, 1),
(59, 27, 9, 76800, 640000, 102400, 307200, 179200, 0, '21,1', '0,0', '', 1, 1),
(60, 27, 10, 153600, 1280000, 204800, 614400, 358400, 0, '21,1', '19,1', '', 1, 1),
(61, 28, 1, 720, 2000, 200, 500, 300, 0, '31,2', '0,0', '', 1, 1),
(62, 28, 2, 1440, 4000, 400, 1000, 600, 0, '31,2', '0,0', '', 1, 1),
(63, 28, 3, 2880, 8000, 800, 2000, 1200, 0, '31,2', '0,0', '', 1, 1),
(64, 28, 4, 5760, 16000, 1600, 4000, 2400, 0, '31,2', '0,0', '', 1, 1),
(65, 28, 5, 11520, 32000, 3200, 8000, 4800, 0, '31,2', '0,0', '', 1, 1),
(66, 28, 6, 23040, 64000, 6400, 16000, 9600, 0, '31,2', '0,0', '', 1, 1),
(67, 28, 7, 46080, 128000, 12800, 32000, 19200, 0, '31,2', '0,0', '', 1, 1),
(68, 28, 8, 92160, 256000, 25600, 64000, 38400, 0, '31,2', '0,0', '', 1, 1),
(69, 28, 9, 184320, 512000, 51200, 128000, 76800, 0, '31,2', '0,0', '', 1, 1),
(70, 28, 10, 368640, 1024000, 102400, 256000, 153600, 0, '31,2', '19,1', '', 1, 1),
(71, 23, 1, 750, 1000, 1000, 1000, 1000, 0, '0,0', '0,0', '', 1, 1),
(72, 23, 2, 1500, 2000, 2000, 2000, 2000, 0, '0,0', '0,0', '', 1, 1),
(73, 23, 3, 3000, 4000, 4000, 4000, 4000, 0, '0,0', '0,0', '', 1, 1),
(74, 23, 4, 6000, 8000, 8000, 8000, 8000, 0, '0,0', '0,0', '', 1, 1),
(75, 23, 5, 12000, 16000, 16000, 16000, 16000, 0, '0,0', '0,0', '', 1, 1),
(76, 23, 6, 24000, 32000, 32000, 32000, 32000, 0, '0,0', '0,0', '', 1, 1),
(77, 23, 7, 48000, 64000, 64000, 64000, 64000, 0, '0,0', '0,0', '', 1, 1),
(78, 23, 8, 96000, 128000, 128000, 128000, 128000, 0, '0,0', '0,0', '', 1, 1),
(79, 23, 9, 192000, 256000, 256000, 256000, 256000, 0, '0,0', '0,0', '', 1, 1),
(80, 23, 10, 384000, 512000, 512000, 512000, 512000, 0, '0,0', '19,1', '', 1, 1),
(81, 25, 1, 480, 2500, 120, 1500, 200, 0, '31,2', '0,0', '', 1, 1),
(82, 25, 2, 960, 5000, 240, 3000, 400, 0, '31,2', '0,0', '', 1, 1),
(83, 25, 3, 1920, 10000, 480, 6000, 800, 0, '31,2', '0,0', '', 1, 1),
(84, 25, 4, 3840, 20000, 960, 12000, 1600, 0, '31,2', '0,0', '', 1, 1),
(85, 25, 5, 7680, 40000, 1920, 24000, 3200, 0, '31,2', '0,0', '', 1, 1),
(86, 25, 6, 15360, 80000, 3840, 48000, 6400, 0, '31,2', '0,0', '', 1, 1),
(87, 25, 7, 30720, 160000, 7680, 96000, 12800, 0, '31,2', '0,0', '', 1, 1),
(88, 25, 8, 61440, 320000, 15360, 192000, 25600, 0, '31,2', '0,0', '', 1, 1),
(89, 25, 9, 122880, 640000, 30720, 384000, 51200, 0, '31,2', '0,0', '', 1, 1),
(90, 25, 10, 245760, 1280000, 61440, 768000, 102400, 0, '31,2', '19,1', '', 1, 1),
(91, 29, 1, 150, 600, 100, 2000, 150, 0, '0,0', '0,0', '', 1, 1),
(92, 29, 2, 300, 1200, 200, 4000, 300, 0, '0,0', '0,0', '', 1, 1),
(93, 29, 3, 600, 2400, 400, 8000, 600, 0, '0,0', '0,0', '', 1, 1),
(94, 29, 4, 1200, 4800, 800, 16000, 1200, 0, '0,0', '0,0', '', 1, 1),
(95, 29, 5, 2400, 9600, 1600, 32000, 2400, 0, '0,0', '0,0', '', 1, 1),
(96, 29, 6, 4800, 19200, 3200, 64000, 4800, 0, '0,0', '0,0', '', 1, 1),
(97, 29, 7, 9600, 38400, 6400, 128000, 9600, 0, '0,0', '0,0', '', 1, 1),
(98, 29, 8, 19200, 76800, 12800, 256000, 19200, 0, '0,0', '0,0', '', 1, 1),
(99, 29, 9, 38400, 153600, 25600, 512000, 38400, 0, '0,0', '0,0', '', 1, 1),
(100, 29, 10, 76800, 307200, 51200, 1024000, 76800, 0, '0,0', '19,1', '', 1, 1),
(101, 30, 1, 450, 1000, 150, 3000, 300, 0, '2,1', '0,0', '', 1, 1),
(102, 30, 2, 900, 2000, 300, 6000, 600, 0, '2,1', '0,0', '', 1, 1),
(103, 30, 3, 1800, 4000, 600, 12000, 1200, 0, '2,1', '0,0', '', 1, 1),
(104, 30, 4, 3600, 8000, 1200, 24000, 2400, 0, '2,1', '0,0', '', 1, 1),
(105, 30, 5, 7200, 16000, 2400, 48000, 4800, 0, '2,1', '0,0', '', 1, 1),
(106, 30, 6, 14400, 32000, 4800, 96000, 9600, 0, '2,1', '0,0', '', 1, 1),
(107, 30, 7, 28800, 64000, 9600, 192000, 19200, 0, '2,1', '0,0', '', 1, 1),
(108, 30, 8, 57600, 128000, 19200, 384000, 38400, 0, '2,1', '0,0', '', 1, 1),
(109, 30, 9, 115200, 256000, 38400, 768000, 76800, 0, '2,1', '0,0', '', 1, 1),
(110, 30, 10, 230400, 512000, 76800, 1536000, 153600, 0, '2,1', '19,1', '', 1, 1),
(111, 22, 1, 180, 1000, 125, 600, 1200, 0, '6,3', '0,0', '', 1, 1),
(112, 22, 2, 360, 2000, 250, 1200, 2400, 0, '6,3', '0,0', '', 1, 1),
(113, 22, 3, 720, 4000, 500, 2400, 4800, 0, '6,3', '0,0', '', 1, 1),
(114, 22, 4, 1440, 8000, 1000, 4800, 9600, 0, '6,3', '0,0', '', 1, 1),
(115, 22, 5, 2880, 16000, 2000, 9600, 19200, 0, '6,3', '0,0', '', 1, 1),
(116, 22, 6, 5760, 32000, 4000, 19200, 38400, 0, '6,3', '0,0', '', 1, 1),
(117, 22, 7, 11520, 64000, 8000, 38400, 76800, 0, '6,3', '0,0', '', 1, 1),
(118, 22, 8, 23040, 128000, 16000, 76800, 153600, 0, '6,3', '0,0', '', 1, 1),
(119, 22, 9, 46080, 256000, 32000, 153600, 307200, 0, '6,3', '0,0', '', 1, 1),
(120, 22, 10, 92160, 512000, 64000, 307200, 614400, 0, '6,3', '19,1', '', 1, 1),
(121, 20, 1, 270, 2000, 1200, 800, 1000, 0, '7,5', '0,0', '', 1, 1),
(122, 20, 2, 540, 4000, 2400, 1600, 2000, 0, '7,5', '0,0', '', 1, 1),
(123, 20, 3, 1080, 8000, 4800, 3200, 4000, 0, '7,5', '0,0', '', 1, 1),
(124, 20, 4, 2160, 16000, 9600, 6400, 8000, 0, '7,5', '0,0', '', 1, 1),
(125, 20, 5, 4320, 32000, 19200, 12800, 16000, 0, '7,5', '0,0', '', 1, 1),
(126, 20, 6, 8640, 64000, 38400, 25600, 32000, 0, '7,5', '0,0', '', 1, 1),
(127, 20, 7, 17280, 128000, 76800, 51200, 64000, 0, '7,5', '0,0', '', 1, 1),
(128, 20, 8, 34560, 256000, 153600, 102400, 128000, 0, '7,5', '0,0', '', 1, 1),
(129, 20, 9, 69120, 512000, 307200, 204800, 256000, 0, '7,5', '0,0', '', 1, 1),
(130, 20, 10, 138240, 1024000, 614400, 409600, 512000, 0, '7,5', '19,1', '', 1, 1),
(131, 26, 1, 540, 1500, 150, 500, 1500, 0, '22,2', '0,0', '', 1, 1),
(132, 26, 2, 1080, 3000, 300, 1000, 3000, 0, '22,2', '0,0', '', 1, 1),
(133, 26, 3, 2160, 6000, 600, 2000, 6000, 0, '22,2', '0,0', '', 1, 1),
(134, 26, 4, 4320, 12000, 1200, 4000, 12000, 0, '22,2', '0,0', '', 1, 1),
(135, 26, 5, 8640, 24000, 2400, 8000, 24000, 0, '22,2', '0,0', '', 1, 1),
(136, 26, 6, 17280, 48000, 4800, 16000, 48000, 0, '22,2', '0,0', '', 1, 1),
(137, 26, 7, 34560, 96000, 9600, 32000, 96000, 0, '22,2', '0,0', '', 1, 1),
(138, 26, 8, 69120, 192000, 19200, 64000, 192000, 0, '22,2', '0,0', '', 1, 1),
(139, 26, 9, 138240, 384000, 38400, 128000, 384000, 0, '22,2', '0,0', '', 1, 1),
(140, 26, 10, 276480, 768000, 76800, 256000, 768000, 0, '22,2', '19,1', '', 1, 1),
(141, 32, 1, 1800, 1500, 3000, 10000, 500, 0, '5,2|26,1', '0,0', '', 2, 1),
(142, 32, 2, 3600, 3000, 6000, 20000, 1000, 0, '5,2|26,1', '0,0', '', 2, 1),
(143, 32, 3, 7200, 6000, 12000, 40000, 2000, 0, '31,1', '0,0', '', 2, 1),
(144, 32, 4, 14400, 12000, 24000, 80000, 4000, 0, '31,2', '0,0', '', 2, 1),
(145, 32, 5, 28800, 24000, 48000, 160000, 8000, 0, '31,3', '0,0', '', 2, 1),
(146, 32, 6, 57600, 48000, 96000, 320000, 16000, 0, '31,4', '0,0', '', 2, 1),
(147, 32, 7, 115200, 96000, 192000, 640000, 32000, 0, '31,5', '0,0', '', 2, 1),
(148, 32, 8, 230400, 192000, 384000, 1280000, 64000, 0, '31,6', '0,0', '', 2, 1),
(149, 32, 9, 460800, 384000, 768000, 2560000, 128000, 0, '31,7', '0,0', '', 2, 1),
(150, 32, 10, 921600, 768000, 1536000, 5120000, 256000, 0, '31,8', '19,1', '', 1, 1),
(151, 24, 1, 3600, 5000, 1500, 4500, 500, 0, '20,1', '0,0', '', 1, 1),
(152, 24, 2, 7200, 10000, 3000, 9000, 1000, 0, '20,1', '0,0', '', 1, 1),
(153, 24, 3, 14400, 20000, 6000, 18000, 2000, 0, '20,1', '0,0', '', 1, 1),
(154, 24, 4, 28800, 40000, 12000, 36000, 4000, 0, '20,1', '0,0', '', 1, 1),
(155, 24, 5, 57600, 80000, 24000, 72000, 8000, 0, '20,1', '0,0', '', 1, 1),
(156, 24, 6, 115200, 160000, 48000, 144000, 16000, 0, '20,1', '0,0', '', 1, 1),
(157, 24, 7, 230400, 320000, 96000, 288000, 32000, 0, '20,1', '0,0', '', 1, 1),
(158, 24, 8, 460800, 640000, 192000, 576000, 64000, 0, '20,1', '0,0', '', 1, 1),
(159, 24, 9, 921600, 1280000, 384000, 1152000, 128000, 0, '20,1', '0,0', '', 1, 1),
(160, 24, 10, 1843200, 2560000, 768000, 2304000, 256000, 0, '20,1', '19,1', '', 1, 1),
(161, 7, 1, 30, 300, 50, 200, 150, 0, '0,0', '0,0', '', 0, 0),
(162, 7, 2, 60, 600, 100, 400, 300, 0, '0,0', '0,0', '', 0, 0),
(163, 7, 3, 120, 1200, 200, 800, 600, 0, '0,0', '0,0', '', 0, 0),
(164, 7, 4, 240, 2400, 400, 1600, 1200, 0, '0,0', '0,0', '', 0, 0),
(165, 7, 5, 480, 4800, 800, 3200, 2400, 0, '0,0', '0,0', '', 0, 0),
(166, 7, 6, 960, 9600, 1600, 6400, 4800, 0, '0,0', '0,0', '', 0, 0),
(167, 7, 7, 1920, 19200, 3200, 12800, 9600, 0, '0,0', '0,0', '', 0, 0),
(168, 7, 8, 3840, 38400, 6400, 25600, 19200, 0, '0,0', '0,0', '', 0, 0),
(169, 7, 9, 7680, 76800, 12800, 51200, 38400, 0, '0,0', '0,0', '', 0, 0),
(170, 7, 10, 15360, 153600, 25600, 102400, 76800, 0, '0,0', '19,1', '', 0, 0),
(171, 4, 1, 45, 100, 100, 250, 300, 0, '0,0', '0,0', '', 0, 0),
(172, 4, 2, 90, 200, 200, 500, 600, 0, '0,0', '0,0', '', 0, 0),
(173, 4, 3, 180, 400, 400, 1000, 1200, 0, '0,0', '0,0', '', 0, 0),
(174, 4, 4, 360, 800, 800, 2000, 2400, 0, '0,0', '0,0', '', 0, 0),
(175, 4, 5, 720, 1600, 1600, 4000, 4800, 0, '0,0', '0,0', '', 0, 0),
(176, 4, 6, 1440, 3200, 3200, 8000, 9600, 0, '0,0', '0,0', '', 0, 0),
(177, 4, 7, 2880, 6400, 6400, 16000, 19200, 0, '0,0', '0,0', '', 0, 0),
(178, 4, 8, 5760, 12800, 12800, 32000, 38400, 0, '0,0', '0,0', '', 0, 0),
(179, 4, 9, 11520, 25600, 25600, 64000, 76800, 0, '0,0', '0,0', '', 0, 0),
(180, 4, 10, 23040, 51200, 51200, 128000, 153600, 0, '0,0', '19,1', '', 0, 0),
(181, 5, 1, 60, 500, 180, 150, 400, 0, '0,0', '0,0', '', 0, 0),
(182, 5, 2, 120, 1000, 360, 300, 800, 0, '0,0', '0,0', '', 0, 0),
(183, 5, 3, 240, 2000, 720, 600, 1600, 0, '0,0', '0,0', '', 0, 0),
(184, 5, 4, 480, 4000, 1440, 1200, 3200, 0, '0,0', '0,0', '', 0, 0),
(185, 5, 5, 960, 8000, 2880, 2400, 6400, 0, '0,0', '0,0', '', 0, 0),
(186, 5, 6, 1920, 16000, 5760, 4800, 12800, 0, '0,0', '0,0', '', 0, 0),
(187, 5, 7, 3840, 32000, 11520, 9600, 25600, 0, '0,0', '0,0', '', 0, 0),
(188, 5, 8, 7680, 64000, 23040, 19200, 51200, 0, '0,0', '0,0', '', 0, 0),
(189, 5, 9, 15360, 128000, 46080, 38400, 102400, 0, '0,0', '0,0', '', 0, 0),
(190, 5, 10, 30720, 256000, 92160, 76800, 204800, 0, '0,0', '19,1', '', 0, 0),
(191, 6, 1, 90, 600, 210, 500, 200, 0, '0,0', '0,0', '', 0, 0),
(192, 6, 2, 180, 1200, 420, 1000, 400, 0, '0,0', '0,0', '', 0, 0),
(193, 6, 3, 360, 2400, 840, 2000, 800, 0, '0,0', '0,0', '', 0, 0),
(194, 6, 4, 720, 4800, 1680, 4000, 1600, 0, '0,0', '0,0', '', 0, 0),
(195, 6, 5, 1440, 9600, 3360, 8000, 3200, 0, '0,0', '0,0', '', 0, 0),
(196, 6, 6, 2880, 19200, 6720, 16000, 6400, 0, '0,0', '0,0', '', 0, 0),
(197, 6, 7, 5760, 38400, 13440, 32000, 12800, 0, '0,0', '0,0', '', 0, 0),
(198, 6, 8, 11520, 76800, 26880, 64000, 25600, 0, '0,0', '0,0', '', 0, 0),
(199, 6, 9, 23040, 153600, 53760, 128000, 51200, 0, '0,0', '0,0', '', 0, 0),
(200, 6, 10, 46080, 307200, 107520, 256000, 102400, 0, '0,0', '19,1', '', 0, 0),
(201, 14, 1, 60, 500, 50, 100, 50, 0, '32,1', '0,0', '0,0', 3, 0),
(202, 15, 1, 120, 1200, 100, 0, 150, 0, '32,2', '0,0', '5,1', 3, 0),
(203, 16, 1, 180, 2000, 200, 1000, 500, 0, '32,3', '0,0', '14,3', 3, 0),
(204, 17, 1, 360, 6000, 300, 0, 0, 0, '32,5', '0,0', '5,4', 3, 0),
(205, 18, 1, 600, 0, 600, 8000, 0, 0, '32,7', '0,0', '5,6', 3, 0);

-- --------------------------------------------------------

--
-- Table structure for table `config_items`
--

CREATE TABLE `config_items` (
  `id` int(10) UNSIGNED NOT NULL,
  `name` varchar(80) NOT NULL,
  `cost` int(10) UNSIGNED NOT NULL,
  `description` varchar(80) NOT NULL,
  `buyable` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `cangamble` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `itemtype` int(10) UNSIGNED NOT NULL,
  `daylimit` int(10) UNSIGNED NOT NULL,
  `rarity` int(10) UNSIGNED NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `config_items`
--

INSERT INTO `config_items` (`id`, `name`, `cost`, `description`, `buyable`, `cangamble`, `itemtype`, `daylimit`, `rarity`) VALUES
(1, 'player.speak.bronze_publicity_ambassador.permanent', 75, 'Bronze Publicity Ambassador (Permanent)', 1, 0, 4, 3650, 0),
(2, 'player.speak.bronze_publicity_ambassador.permanent.15', 75, 'Bronze Publicity Ambassador (15-day)', 1, 0, 4, 15, 0),
(3, 'player.speak.gold_publicity_ambassador.15', 75, 'Gold Publicity Ambassador (15-day)', 1, 0, 4, 15, 0),
(4, 'player.speak.gold_publicity_ambassador.permanent', 75, 'Gold Publicity Ambassador (Permanent)', 1, 0, 4, 3650, 0),
(5, 'player.speak.silver_publicity_ambassador.15', 75, 'Silver Publicity Ambassador (15-day)', 1, 0, 4, 15, 0),
(6, 'player.speak.silver_publicity_ambassador.permanent', 75, 'Silver Publicity Ambassador (Permanent)', 1, 0, 4, 3650, 0),
(7, 'alliance.ritual_of_pact.advanced', 75, 'Ritual of Pact (Advanced)', 1, 0, 4, 15, 0),
(8, 'alliance.ritual_of_pact.premium', 75, 'Ritual of Pact (Premium)', 1, 0, 4, 30, 0),
(9, 'alliance.ritual_of_pact.ultimate', 75, 'Ritual of Pact (Ultimate)', 1, 0, 4, 90, 0),
(10, 'consume.1.a', 3, 'Speaker', 1, 1, 4, 0, 5),
(11, 'consume.1.b', 27, 'Speaker (10 pieces package)', 1, 1, 4, 0, 5),
(12, 'consume.1.c', 240, 'Speaker (100 pieces package)', 1, 1, 4, 0, 3),
(13, 'consume.2.a', 5, 'Beginner Guidelines', 1, 1, 1, 0, 5),
(14, 'consume.2.b', 10, 'Primary Guidelines', 1, 1, 1, 0, 5),
(15, 'consume.2.b.1', 20, 'Intermediate Guidelines', 1, 1, 1, 0, 4),
(16, 'consume.2.c', 50, 'Senior Guidelines', 1, 1, 1, 0, 4),
(17, 'consume.2.c.1', 80, 'Master Guidelines', 1, 1, 1, 0, 3),
(18, 'consume.2.d', 120, 'Ultimate Guidelines', 1, 1, 1, 0, 3),
(19, 'consume.blueprint.1', 50, 'Michelangelo\'s Script', 1, 1, 4, 0, 3),
(20, 'consume.changeflag.1', 6, 'National Flag', 1, 1, 4, 0, 4),
(21, 'consume.hegemony.1', 500, 'Ritual of Pact', 1, 1, 4, 0, 1),
(22, 'consume.key.1', 5, 'Bronze Key', 0, 0, 2, 0, 0),
(23, 'consume.key.2', 40, 'Silver Key', 0, 0, 2, 0, 0),
(24, 'consume.key.3', 100, 'Golden Key', 0, 0, 2, 0, 0),
(25, 'consume.move.1', 30, 'City Teleporter', 1, 1, 4, 0, 5),
(26, 'consume.refreshtavern.1', 5, 'Hero Hunting', 1, 1, 4, 0, 5),
(27, 'consume.transaction.1', 5, 'Merchant Fleet', 1, 1, 1, 0, 5),
(28, 'hero.intelligence.1', 40, 'The Art of War', 1, 0, 4, 0, 3),
(29, 'hero.loyalty.1', 0, 'Cross Medal', 0, 1, 0, 0, 5),
(30, 'hero.loyalty.2', 0, 'Rose Medal', 0, 1, 0, 0, 5),
(31, 'hero.loyalty.3', 0, 'Lion Medal', 0, 1, 0, 0, 5),
(32, 'hero.loyalty.4', 0, 'Honor Medal', 0, 1, 0, 0, 5),
(33, 'hero.loyalty.5', 0, 'Courage Medal', 0, 1, 0, 0, 5),
(34, 'hero.loyalty.6', 0, 'Wisdom Medal', 0, 1, 0, 0, 5),
(35, 'hero.loyalty.7', 0, 'Freedom Medal', 0, 1, 0, 0, 5),
(36, 'hero.loyalty.8', 0, 'Justice Medal', 0, 1, 0, 0, 3),
(37, 'hero.loyalty.9', 0, 'Nation Medal', 0, 1, 0, 0, 3),
(38, 'hero.management.1', 40, 'The Wealth of Nations', 1, 0, 4, 0, 3),
(39, 'hero.power.1', 40, 'Excalibur', 1, 0, 4, 0, 0),
(40, 'hero.reset.1', 20, 'Holy Water', 1, 1, 4, 0, 5),
(41, 'hero.reset.1.a', 90, 'Holy Water (5 pieces package)', 1, 1, 4, 0, 3),
(42, 'player.attackinc.1', 10, 'War Horn', 1, 1, 4, 0, 5),
(43, 'player.attackinc.1.b', 60, 'Ivory Horn', 1, 1, 4, 0, 3),
(44, 'player.box.1', 10, 'Bronze Lockbox', 0, 0, 2, 0, 0),
(45, 'player.box.2', 80, 'Silver Lockbox ', 0, 0, 2, 0, 0),
(46, 'player.box.3', 200, 'Golden Lockbox ', 0, 0, 2, 0, 0),
(47, 'player.box.currently.1', 188, 'Hero Package', 1, 0, 2, 0, 0),
(48, 'player.box.gambling.1', 5, 'Amulet', 0, 0, 2, 0, 0),
(49, 'player.box.gambling.10', 5, 'Amulet', 0, 0, 2, 0, 0),
(50, 'player.box.gambling.11', 5, 'Amulet', 0, 0, 2, 0, 0),
(51, 'player.box.gambling.12', 5, 'Amulet', 0, 0, 2, 0, 0),
(52, 'player.box.gambling.2', 5, 'Amulet', 0, 0, 2, 0, 0),
(53, 'player.box.gambling.3', 5, 'Amulet', 1, 0, 2, 0, 0),
(54, 'player.box.gambling.4', 5, 'Amulet', 0, 0, 2, 0, 0),
(55, 'player.box.gambling.5', 5, 'Amulet', 0, 0, 2, 0, 0),
(56, 'player.box.gambling.6', 5, 'Amulet', 0, 0, 2, 0, 0),
(57, 'player.box.gambling.7', 5, 'Amulet', 0, 0, 2, 0, 0),
(58, 'player.box.gambling.8', 5, 'Amulet', 0, 0, 2, 0, 0),
(59, 'player.box.gambling.9', 5, 'Amulet', 0, 0, 2, 0, 0),
(60, 'player.box.gambling.food', 0, 'Food', 0, 1, 2, 0, 5),
(61, 'player.box.gambling.gold', 0, 'Gold', 0, 1, 2, 0, 5),
(62, 'player.box.gambling.iron', 0, 'Iron', 0, 1, 2, 0, 5),
(63, 'player.box.gambling.medal.10', 0, 'Cent game coin(s)', 0, 1, 2, 0, 4),
(64, 'player.box.gambling.medal.300', 300, 'Closed Beta Awards', 0, 0, 2, 0, 0),
(65, 'player.box.gambling.stone', 0, 'Stone', 0, 1, 2, 0, 5),
(66, 'player.box.gambling.wood', 0, 'Lumber', 0, 1, 2, 0, 5),
(67, 'player.box.hero.a', 0, 'Leather Helm of Robinhood', 0, 1, 2, 0, 5),
(68, 'player.box.hero.b', 0, 'Chain Helm of Beowulf', 0, 1, 2, 0, 4),
(69, 'player.box.hero.c', 0, 'Plate Helm of Lancelot', 0, 1, 2, 0, 3),
(70, 'player.box.hero.d', 0, 'Holy Helm of Mars', 0, 1, 2, 0, 2),
(71, 'player.box.present.1', 0, 'Bonus Package', 0, 0, 2, 0, 0),
(72, 'player.box.present.10', 0, 'Medal Package', 0, 0, 2, 0, 0),
(73, 'player.box.present.11', 0, 'Variety Sack', 0, 0, 2, 0, 0),
(74, 'player.box.present.2', 0, 'Newbies Package', 0, 0, 2, 0, 0),
(75, 'player.box.present.3', 0, 'Closed Beta Rewards', 0, 0, 2, 0, 0),
(76, 'player.box.present.4', 0, 'Package for Lords', 0, 0, 2, 0, 0),
(77, 'player.box.present.5', 0, 'Loyalty Package', 0, 0, 2, 0, 0),
(78, 'player.box.present.6', 0, 'Production Enhancer', 0, 0, 2, 0, 0),
(79, 'player.box.present.7', 0, 'Military Package', 0, 0, 2, 0, 0),
(80, 'player.box.present.8', 0, 'Prestige Package', 0, 0, 2, 0, 0),
(81, 'player.box.present.9', 0, 'Honor Package', 0, 0, 2, 0, 0),
(82, 'player.box.resource.1', 0, 'Grand Resources', 0, 0, 2, 0, 0),
(83, 'player.box.special.1', 80, 'Junior Medal Box', 1, 1, 2, 0, 3),
(84, 'player.box.special.2', 140, 'Medium Medal Box', 1, 1, 2, 0, 2),
(85, 'player.box.special.3', 240, 'Senior Medal Box', 1, 1, 2, 0, 2),
(86, 'player.box.troop.1', 0, 'Superb Regiments', 0, 0, 2, 0, 0),
(87, 'player.box.troop.a', 0, 'Junior Battalion', 0, 1, 2, 0, 5),
(88, 'player.box.troop.b', 0, 'Senior Battalion', 0, 1, 2, 0, 5),
(89, 'player.box.troop.c', 0, 'Veteran Battalion', 0, 1, 2, 0, 3),
(90, 'player.box.troop.d', 0, 'Mechanics Battalion', 0, 1, 2, 0, 2),
(91, 'player.box.wood.1', 0, 'Wooden Chest', 0, 0, 2, 0, 0),
(92, 'player.defendinc.1', 10, 'Corselet', 1, 1, 4, 0, 5),
(93, 'player.defendinc.1.b', 60, 'Ultra Corselet', 1, 1, 4, 0, 3),
(94, 'player.destroy.1.a', 20, 'Dynamite', 1, 1, 4, 0, 4),
(95, 'player.experience.1.a', 8, 'Anabasis', 1, 1, 4, 0, 5),
(96, 'player.experience.1.b', 30, 'Epitome of Military Science', 1, 1, 4, 0, 4),
(97, 'player.experience.1.c', 100, 'On War', 1, 1, 4, 0, 3),
(98, 'player.fort.1.c', 35, 'Archimedes\' Note', 1, 1, 1, 0, 5),
(99, 'player.gold.1.a', 45, 'Tax Policy', 1, 1, 3, 0, 4),
(100, 'player.gold.1.b', 280, 'Adv Tax Policy', 1, 1, 3, 0, 2),
(101, 'player.heart.1.a', 100, 'Speech Text', 1, 1, 4, 0, 3),
(102, 'player.more.castle.1.a', 90, 'Adv City Teleporter', 1, 1, 4, 0, 2),
(103, 'player.name.1.a', 200, 'New ID', 1, 1, 4, 1, 2),
(104, 'player.peace.1', 30, 'Truce Agreement', 1, 1, 4, 0, 4),
(105, 'player.pop.1.a', 50, 'Civil Code', 1, 1, 4, 0, 4),
(106, 'player.relive.1', 120, 'Penicillin', 1, 1, 4, 0, 2),
(107, 'player.resinc.1', 8, 'Plowshares', 1, 1, 3, 0, 5),
(108, 'player.resinc.1.b', 50, 'Iron Rake', 1, 1, 3, 0, 4),
(109, 'player.resinc.2', 8, 'Arch Saw', 1, 1, 3, 0, 5),
(110, 'player.resinc.2.b', 50, 'Double Saw', 1, 1, 3, 0, 4),
(111, 'player.resinc.3', 8, 'Quarrying Tools', 1, 1, 3, 0, 5),
(112, 'player.resinc.3.b', 50, 'Adv Quarrying Tools', 1, 1, 3, 0, 4),
(113, 'player.resinc.4', 8, 'Blower', 1, 1, 3, 0, 5),
(114, 'player.resinc.4.b', 50, 'Blast Furnace', 1, 1, 3, 0, 4),
(115, 'player.troop.1.a', 30, 'War Ensign', 1, 1, 4, 0, 4),
(116, 'player.troop.1.b', 30, 'Napoleon\'s Diary', 1, 1, 1, 0, 4),
(117, 'player.box.present.medal.50', 0, 'Cents Package(50 cents)', 0, 0, 2, 0, 0),
(118, 'player.box.present.12', 0, 'Beginnerâ€™s Package', 0, 0, 2, 0, 0),
(119, 'player.box.present.13', 0, 'Veteranâ€™s Package', 0, 0, 2, 0, 0),
(120, 'player.box.present.14', 0, 'Bonus Package 2', 0, 0, 2, 0, 0),
(121, 'player.box.present.15', 0, 'Bonus Package 3', 0, 0, 2, 0, 0),
(122, 'player.box.present.16', 0, 'Bonus Package 4', 0, 0, 2, 0, 0),
(123, 'player.box.present.17', 0, 'Bonus Package 5', 0, 0, 2, 0, 0),
(124, 'player.box.present.18', 0, 'Bonus Package 6', 0, 0, 2, 0, 0),
(125, 'player.box.present.19', 0, 'Bonus Package 7', 0, 0, 2, 0, 0),
(126, 'player.box.present.20', 0, 'Package Aug A', 0, 0, 2, 0, 0),
(127, 'player.box.present.21', 0, 'Package Aug B', 0, 0, 2, 0, 0),
(128, 'player.box.present.22', 0, 'Package Aug C', 0, 0, 2, 0, 0),
(129, 'player.box.present.medal.3500', 0, 'Cents Package(3500 cents)', 0, 0, 2, 0, 0),
(130, 'player.box.present.medal.500', 0, 'Cents Package(500 cents)', 0, 0, 2, 0, 0),
(131, 'player.box.present.23', 0, 'Compensate pack 09/08/23', 0, 0, 2, 0, 0),
(132, 'player.box.present.24', 0, 'Package Aug D', 0, 0, 2, 0, 0),
(133, 'player.box.present.25', 0, 'Kick-start Gift Package', 0, 0, 2, 0, 0),
(134, 'player.box.present.26', 0, 'Package Sep A', 0, 0, 2, 0, 0),
(135, 'player.box.present.27', 0, 'Package Sep B', 0, 0, 2, 0, 0),
(136, 'player.box.present.28', 0, 'Package Sep C', 0, 0, 2, 0, 0),
(137, 'player.box.present.29', 0, 'Bonus Package 17', 0, 0, 2, 0, 0),
(138, 'player.box.present.3', 0, 'Closed Beta Rewards', 0, 0, 2, 0, 0),
(139, 'player.box.present.30', 0, 'Bonus Package 18', 0, 0, 2, 0, 0),
(140, 'player.box.present.31', 0, 'Bonus Package 19', 0, 0, 2, 0, 0),
(141, 'player.box.present.32', 0, 'Bonus Package 20', 0, 0, 2, 0, 0),
(142, 'player.box.present.33', 0, 'Bonus Package 21', 0, 0, 2, 0, 0),
(143, 'player.box.present.34', 0, 'Bonus Package 22', 0, 0, 2, 0, 0),
(144, 'player.box.present.35', 0, 'Bonus Package 23', 0, 0, 2, 0, 0),
(145, 'player.box.present.36', 0, 'Bonus Package 24', 0, 0, 2, 0, 0),
(146, 'player.box.present.37', 0, 'Bonus Package 25', 0, 0, 2, 0, 0),
(147, 'player.box.present.38', 0, 'Bonus Package 26', 0, 0, 2, 0, 0),
(148, 'player.box.present.recall.a', 0, 'Recall Package A', 0, 0, 2, 0, 0),
(149, 'player.box.present.recall.b', 0, 'Recall Package B', 0, 0, 2, 0, 0),
(150, 'player.box.present.recall.c', 0, 'Recall Package C', 0, 0, 2, 0, 0),
(151, 'player.box.present.money.3', 0, '$5 for 250 cents worth', 0, 0, 2, 0, 0),
(152, 'player.box.present.money.4', 0, 'World Teacher\'s Day Celebration Package', 0, 0, 2, 0, 0),
(153, 'player.box.present.money.5', 0, 'Evony 10 Million Players Milestone Celebration Package', 0, 0, 2, 0, 0),
(154, 'player.box.present.money.6', 0, 'Evony 10 Million Players Milestone Celebration Premium Package', 0, 0, 2, 0, 0),
(155, 'player.box.present.money.7', 0, 'Pumpkin Package', 0, 0, 2, 0, 0),
(156, 'player.box.present.money.8', 0, 'Pumpkin Package', 0, 0, 2, 0, 0),
(157, 'player.box.present.money.9', 0, 'Pumpkin Package', 0, 0, 2, 0, 0),
(158, 'player.box.present.money.10', 0, 'Warm Package', 0, 0, 2, 0, 0),
(159, 'player.box.present.money.11', 0, 'Premium Warm Package', 0, 0, 2, 0, 0),
(160, 'player.box.present.money.12', 0, 'Hot Package 1', 0, 0, 2, 0, 0),
(161, 'player.box.present.money.13', 0, 'Hot Package 2', 0, 0, 2, 0, 0),
(162, 'player.box.present.money.14', 0, 'Hot Package 3', 0, 0, 2, 0, 0),
(163, 'player.box.present.money.15', 0, 'Evony 100 Servers Milestone Celebration Package', 0, 0, 2, 0, 0),
(164, 'player.box.present.money.16', 0, 'Evony 100 Servers Milestone Celebration Premium Package', 0, 0, 2, 0, 0),
(165, 'player.box.present.money.buff.17', 0, 'Thanksgiving Special Package A', 0, 0, 2, 0, 0),
(166, 'player.box.present.money.buff.18', 0, 'Thanksgiving Special Package B', 0, 0, 2, 0, 0),
(167, 'player.box.present.money.buff.19', 0, 'Thanksgiving Special Package C', 0, 0, 2, 0, 0),
(168, 'player.box.present.money.20', 0, 'December Package', 0, 0, 2, 0, 0),
(169, 'player.box.present.money.21', 0, 'December Premium Package', 0, 0, 2, 0, 0),
(170, 'player.box.present.money.22', 0, 'Christmas Package', 0, 0, 2, 0, 0),
(171, 'player.box.present.money.23', 300, 'Special Christmas Chest', 0, 0, 2, 0, 0),
(172, 'player.key.santa', 0, 'Secret Santa Key', 0, 0, 4, 0, 0),
(173, 'player.santa.stoptoopsupkeep', 0, 'Santa\'s Milk and Cookies', 0, 0, 4, 0, 0),
(174, 'player.box.present.christmas.a', 0, 'Special Christmas Package A', 0, 0, 2, 0, 0),
(175, 'player.box.present.christmas.b', 0, 'Special Christmas Package B', 0, 0, 2, 0, 0),
(176, 'player.box.present.christmas.c', 0, 'Special Christmas Package C', 0, 0, 2, 0, 0),
(177, 'player.box.present.money.24', 0, 'New Year Package', 0, 0, 2, 0, 0),
(178, 'player.box.present.money.25', 300, 'Special New Year Chest', 0, 0, 2, 0, 0),
(179, 'player.key.newyear', 0, 'Special New Year Key', 0, 0, 4, 0, 0),
(180, 'player.newyear.stoptoopsupkeep', 0, 'New Year\'s Eve Party Spirit', 0, 0, 4, 0, 0),
(181, 'player.truce.dream', 0, 'Dream Truce', 0, 0, 4, 0, 0),
(182, 'player.box.present.money.27', 0, 'New Year Package', 0, 0, 2, 0, 0),
(183, 'player.move.castle.1.b', 0, 'Pioneer Express Teleport', 0, 0, 4, 0, 0),
(184, 'player.box.present.40', 0, '131 Server Compensation Package', 0, 0, 2, 0, 0),
(185, 'player.key.easter_package', 0, 'Special Easter Chest Key', 0, 0, 4, 0, 0),
(186, 'player.box.present.money.28', 0, 'Easter Package', 0, 0, 2, 0, 0),
(187, 'player.box.present.money.29', 300, 'Special Easter Chest', 0, 0, 2, 0, 0),
(188, 'player.reduce.troops.upkeep.1', 0, 'Vesta\'s Blessing', 0, 0, 4, 0, 0),
(189, 'player.key.special.chest', 0, 'Special Chest Key', 0, 0, 4, 0, 0),
(190, 'player.box.present.money.30', 0, 'Evony Happiness Package', 0, 0, 2, 0, 0),
(191, 'player.box.present.money.31', 300, 'Special Evony Happiness Chest', 0, 0, 2, 0, 0),
(192, 'player.box.merger.compensation', 0, 'Merger Compensation Pack', 0, 0, 2, 0, 0),
(193, 'player.box.present.money.32', 0, 'Halloween Pumpkin Package', 0, 0, 2, 0, 0),
(194, 'player.box.present.money.33', 300, 'Halloween Chest O\'Treats', 0, 0, 2, 0, 0),
(195, 'player.key.halloween', 0, 'Halloween Skeleton Key', 0, 0, 4, 0, 0),
(196, 'player.halloween.candy', 0, 'Halloween Candy', 0, 0, 4, 0, 0),
(197, 'player.box.present.money.34', 0, 'Thanksgiving Package', 0, 0, 2, 0, 0),
(198, 'player.box.present.money.35', 300, 'Special Thanksgiving Package', 0, 0, 2, 0, 0),
(199, 'player.box.evony.birthday', 0, 'Evony Birthday Gift', 0, 0, 2, 0, 0),
(200, 'player.box.present.money.36', 0, 'Evony\'s Christmas Gift', 0, 0, 2, 0, 0),
(201, 'player.box.present.money.37', 300, 'Secret Santa Chest', 0, 0, 2, 0, 0),
(202, 'player.box.evony.subscription', 0, 'Evony Subscription Gift', 0, 0, 2, 0, 0),
(203, 'player.box.toolbarbonus1.greater', 0, 'Greater Daily Bonus', 0, 0, 2, 0, 0),
(204, 'player.box.toolbarbonus1.lesser', 0, 'Lesser Daily Bonus', 0, 0, 2, 0, 0),
(205, 'player.box.toolbarbonus1.medium', 0, 'Medium Daily Bonus', 0, 0, 2, 0, 0),
(206, 'player.box.toolbarbonus1.superior', 0, 'Superior Daily Bonus', 0, 0, 2, 0, 0),
(207, 'player.box.present.money.38', 0, 'Evony\'s Heart-Shaped Box', 0, 0, 2, 0, 0),
(208, 'player.box.present.money.39', 300, 'Valentine\'s Day Chest ', 0, 0, 2, 0, 0),
(209, 'player.key.valentine', 0, 'Valentine\'s Day Key', 0, 0, 4, 0, 0),
(210, 'player.cupid.chocolate', 0, 'Cupid\'s Chocolates ', 0, 0, 4, 0, 0),
(211, 'player.queue.building', 5, 'Bernini\'s Hammer', 1, 1, 4, 0, 5),
(212, 'player.key.patrick', 0, 'St Patrick\'s Day Key', 0, 0, 4, 0, 0),
(213, 'player.box.present.money.40', 0, 'Evony\'s Pot o\' Gold', 0, 0, 2, 0, 0),
(214, 'player.box.present.money.41', 300, 'St Patrick\'s Day Chest', 0, 0, 2, 0, 0),
(215, 'player.irish.whiskey', 0, 'Irish Whiskey ', 0, 0, 4, 0, 0),
(216, 'player.box.present.money.42', 0, 'Easter Package', 0, 0, 2, 0, 0),
(217, 'player.box.present.money.43', 300, 'Special Easter Chest', 0, 0, 2, 0, 0),
(218, 'player.key.easter', 0, 'Special Easter Chest Key', 0, 0, 4, 0, 0),
(219, 'player.easter.egg', 0, 'Easter Egg', 0, 0, 4, 0, 0),
(220, 'player.box.compensation.a', 0, 'Compensation Package', 0, 0, 2, 0, 0),
(221, 'player.box.compensation.b', 0, 'Compensation Package', 0, 0, 2, 0, 0),
(222, 'player.box.compensation.c', 0, 'Compensation Package', 0, 0, 2, 0, 0),
(223, 'player.box.compensation.d', 0, 'Compensation Package', 0, 0, 2, 0, 0),
(224, 'player.box.compensation.e', 0, 'Compensation Package', 0, 0, 2, 0, 0),
(225, 'player.box.present.money.44', 0, 'Pamplona Prize Pack', 0, 0, 2, 0, 0),
(226, 'player.box.present.money.45', 300, 'Hollow Wooden Bull', 0, 0, 2, 0, 0),
(227, 'player.key.bull', 0, 'Wooden Bull Opener', 0, 0, 4, 0, 0),
(228, 'player.running.shoes', 0, 'Extra-Fast Running Shoes', 0, 0, 4, 0, 0),
(229, 'player.box.test.1', 10, 'Test Item', 1, 0, 2, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `config_quests`
--

CREATE TABLE `config_quests` (
  `id` int(5) NOT NULL,
  `mainId` int(5) NOT NULL,
  `isFinish` tinyint(1) NOT NULL,
  `description` varchar(1000) NOT NULL,
  `name` text NOT NULL,
  `typeId` int(5) NOT NULL,
  `prereqbuilding` varchar(45) NOT NULL,
  `prereqitem` varchar(45) NOT NULL,
  `prereqtech` varchar(45) NOT NULL,
  `prereqquest` varchar(45) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `config_quests`
--

INSERT INTO `config_quests` (`id`, `mainId`, `isFinish`, `description`, `name`, `typeId`, `prereqbuilding`, `prereqitem`, `prereqtech`, `prereqquest`) VALUES
(1, 1, 0, 'Rebuild', 'rebuild', 1, '0', '0', '0', '0'),
(2, 1, 0, 'Domain Expansion', 'domain expansion', 2, '0', '0', '0', '0'),
(3, 1, 0, 'Commodity Gathering', 'commodity gathering', 4, '4,5,6,7', '0', '0', '0'),
(4, 1, 0, 'Comforting', 'comforting', 5, '31', '0', '0', '0'),
(5, 1, 0, 'Resource Levy', 'resource levy', 5, '31', '0', '0', '0'),
(6, 1, 0, 'Administration', 'adminstration', 6, '0', '0', '0', '0'),
(7, 1, 0, 'Trading', 'trading', 7, '31', '0', '0', '0'),
(8, 1, 0, 'Scientific Research', 'scientific research', 8, '0', '0', '0', '0'),
(9, 1, 0, 'Alliance', 'alliance', 9, '0', '0', '0', '0'),
(10, 1, 0, 'Correspondence', 'correspondence', 10, '31', '0', '0', '0'),
(11, 1, 0, 'Hero Recuritment', 'hero recuritment', 11, '0', '0', '0', '0'),
(12, 1, 0, 'Military Service', 'military service', 11, '0', '0', '0', '0'),
(13, 1, 0, 'Scouting', 'scouting', 13, '0', '0', '0', '16'),
(14, 1, 0, 'Fortification', 'fortification', 14, '0', '0', '0', '0'),
(15, 1, 0, 'Colony', 'colony', 15, '0', '0', '0', '0'),
(16, 1, 0, 'City Expansion', 'city Expansion', 16, '0', '0', '0', '0'),
(17, 1, 0, 'Rich Store', 'rich store', 17, '7', '0', '0', '0'),
(18, 1, 0, 'Transportation', 'transportation', 18, '0', '0', '0', '0'),
(19, 1, 0, 'Alarming', 'alarming', 19, '0', '0', '0', '12'),
(20, 1, 0, 'Population Increase', 'population increase', 20, '0', '0', '0', '0'),
(21, 1, 1, 'The Medals', 'The Medals', 1, '0', '0', '0', '30'),
(22, 1, 1, 'Wealthy Society', 'Wealthy Society', 1, '', '', '', ''),
(23, 1, 1, 'Prestiage Building', 'Prestiage Building', 1, '31', '', '', ''),
(25, 1, 1, 'Item Purchase', 'Item Purchase', 1, '', '', '', ''),
(26, 1, 1, 'Christmas Hunt', 'Christmas Hunt', 1, '', '', '', ''),
(27, 1, 1, 'Consolation', 'Consolation', 1, '', '', '', ''),
(28, 1, 1, 'Guidelines', 'Guidelines', 1, '31', '', '', ''),
(29, 1, 1, 'BattleField', 'BattleField', 1, '', '', '', '16'),
(30, 1, 1, 'Promotion', 'Promotion', 1, '', '', '', ''),
(31, 3, 1, 'Amulet', 'Amulet', 31, '', '48', '', '');

-- --------------------------------------------------------

--
-- Table structure for table `config_research`
--

CREATE TABLE `config_research` (
  `id` int(10) UNSIGNED NOT NULL,
  `researchid` int(10) UNSIGNED NOT NULL,
  `level` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `buildtime` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `wood` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `food` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `stone` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `iron` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `gold` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `prereqbuilding` varchar(45) DEFAULT '',
  `prereqitem` varchar(45) DEFAULT '',
  `prereqtech` varchar(45) DEFAULT ''
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `config_research`
--

INSERT INTO `config_research` (`id`, `researchid`, `level`, `buildtime`, `wood`, `food`, `stone`, `iron`, `gold`, `prereqbuilding`, `prereqitem`, `prereqtech`) VALUES
(1, 1, 1, 400, 0, 500, 0, 0, 1000, '25,1|7,1', '', ''),
(2, 1, 2, 800, 0, 1000, 0, 0, 2000, '25,1|7,2', '', ''),
(3, 1, 3, 1600, 0, 2000, 0, 0, 4000, '25,1|7,3', '', ''),
(4, 1, 4, 3200, 0, 4000, 0, 0, 8000, '25,1|7,4', '', ''),
(5, 1, 5, 6400, 0, 8000, 0, 0, 16000, '25,1|7,5', '', ''),
(6, 1, 6, 12800, 0, 16000, 0, 0, 32000, '25,1|7,6', '', ''),
(7, 1, 7, 25600, 0, 32000, 0, 0, 64000, '25,1|7,7', '', ''),
(8, 1, 8, 51200, 0, 64000, 0, 0, 128000, '25,1|7,8', '', ''),
(9, 1, 9, 102400, 0, 128000, 0, 0, 256000, '25,1|7,9', '', ''),
(10, 1, 10, 204800, 0, 256000, 0, 0, 512000, '25,1|7,10', '', ''),
(11, 2, 1, 500, 500, 0, 0, 100, 1200, '25,1|4,1', '', ''),
(12, 2, 2, 1000, 1000, 0, 0, 200, 2400, '25,1|4,2', '', ''),
(13, 2, 3, 2000, 2000, 0, 0, 400, 4800, '25,1|4,3', '', ''),
(14, 2, 4, 4000, 4000, 0, 0, 800, 9600, '25,1|4,4', '', ''),
(15, 2, 5, 8000, 8000, 0, 0, 1600, 19200, '25,1|4,5', '', ''),
(16, 2, 6, 16000, 16000, 0, 0, 3200, 38400, '25,1|4,6', '', ''),
(17, 2, 7, 32000, 32000, 0, 0, 6400, 76800, '25,1|4,7', '', ''),
(18, 2, 8, 64000, 64000, 0, 0, 12800, 153600, '25,1|4,8', '', ''),
(19, 2, 9, 128000, 128000, 0, 0, 25600, 307200, '25,1|4,9', '', ''),
(20, 2, 10, 256000, 256000, 0, 0, 51200, 614400, '25,1|4,10', '', ''),
(21, 3, 1, 600, 0, 0, 500, 200, 1500, '25,2|5,1', '', '2,1'),
(22, 3, 2, 1200, 0, 0, 1000, 400, 3000, '25,2|5,2', '', '2,1'),
(23, 3, 3, 2400, 0, 0, 2000, 800, 6000, '25,2|5,3', '', '2,1'),
(24, 3, 4, 4800, 0, 0, 4000, 1600, 12000, '25,2|5,4', '', '2,1'),
(25, 3, 5, 9600, 0, 0, 8000, 3200, 24000, '25,2|5,5', '', '2,1'),
(26, 3, 6, 19200, 0, 0, 16000, 6400, 48000, '25,2|5,6', '', '2,1'),
(27, 3, 7, 38400, 0, 0, 32000, 12800, 96000, '25,2|5,7', '', '2,1'),
(28, 3, 8, 76800, 0, 0, 64000, 25600, 192000, '25,2|5,8', '', '2,1'),
(29, 3, 9, 153600, 0, 0, 128000, 51200, 384000, '25,2|5,9', '', '2,1'),
(30, 3, 10, 307200, 0, 0, 256000, 102400, 768000, '25,2|5,10', '', '2,1'),
(31, 4, 1, 700, 0, 0, 0, 800, 2000, '25,2|6,1', '', '3,1'),
(32, 4, 2, 1400, 0, 0, 0, 1600, 4000, '25,2|6,2', '', '3,1'),
(33, 4, 3, 2800, 0, 0, 0, 3200, 8000, '25,2|6,3', '', '3,1'),
(34, 4, 4, 5600, 0, 0, 0, 6400, 16000, '25,2|6,4', '', '3,1'),
(35, 4, 5, 11200, 0, 0, 0, 12800, 32000, '25,2|6,5', '', '3,1'),
(36, 4, 6, 22400, 0, 0, 0, 25600, 64000, '25,2|6,6', '', '3,1'),
(37, 4, 7, 44800, 0, 0, 0, 51200, 128000, '25,2|6,7', '', '3,1'),
(38, 4, 8, 89600, 0, 0, 0, 102400, 256000, '25,2|6,8', '', '3,1'),
(39, 4, 9, 179200, 0, 0, 0, 204800, 512000, '25,2|6,9', '', '3,1'),
(40, 4, 10, 358400, 0, 0, 0, 409600, 1024000, '25,2|6,10', '', '3,1'),
(41, 5, 1, 900, 500, 0, 0, 500, 5000, '25,3|26,1', '', '4,2'),
(42, 5, 2, 1800, 1000, 0, 0, 1000, 10000, '25,3|26,1', '', '4,2'),
(43, 5, 3, 3600, 2000, 0, 0, 2000, 20000, '25,3|26,1', '', '4,2'),
(44, 5, 4, 7200, 4000, 0, 0, 4000, 40000, '25,3|26,1', '', '4,2'),
(45, 5, 5, 14400, 8000, 0, 0, 8000, 80000, '25,3|26,1', '', '4,2'),
(46, 5, 6, 28800, 16000, 0, 0, 16000, 160000, '25,1|26,1', '', '4,2'),
(47, 5, 7, 57600, 32000, 0, 0, 32000, 320000, '25,1|26,1', '', '4,2'),
(48, 5, 8, 115200, 64000, 0, 0, 64000, 640000, '25,1|26,1', '', '4,2'),
(49, 5, 9, 230400, 128000, 0, 0, 128000, 1280000, '25,1|26,1', '', '4,2'),
(50, 5, 10, 460800, 256000, 0, 0, 256000, 2560000, '25,1|26,1', '', '4,2'),
(51, 7, 1, 300, 0, 300, 0, 0, 2000, '25,3', '', ''),
(52, 7, 2, 600, 0, 600, 0, 0, 4000, '25,3', '', ''),
(53, 7, 3, 1200, 0, 1200, 0, 0, 8000, '25,3', '', ''),
(54, 7, 4, 2400, 0, 2400, 0, 0, 16000, '25,3', '', ''),
(55, 7, 5, 4800, 0, 4800, 0, 0, 32000, '25,3', '', ''),
(56, 7, 6, 9600, 0, 9600, 0, 0, 64000, '25,3', '', ''),
(57, 7, 7, 19200, 0, 19200, 0, 0, 128000, '25,3', '', ''),
(58, 7, 8, 38400, 0, 38400, 0, 0, 256000, '25,3', '', ''),
(59, 7, 9, 76800, 0, 76800, 0, 0, 512000, '25,3', '', ''),
(60, 7, 10, 153600, 0, 153600, 0, 0, 1024000, '25,3', '', ''),
(61, 8, 1, 900, 100, 600, 100, 150, 2500, '25,1|22,1', '', ''),
(62, 8, 2, 1800, 200, 1200, 200, 300, 5000, '25,1|22,2', '', ''),
(63, 8, 3, 3600, 400, 2400, 400, 600, 10000, '25,1|22,3', '', ''),
(64, 8, 4, 7200, 800, 4800, 800, 1200, 20000, '25,1|22,4', '', ''),
(65, 8, 5, 14400, 1600, 9600, 1600, 2400, 40000, '25,1|22,5', '', ''),
(66, 8, 6, 28800, 3200, 19200, 3200, 4800, 80000, '25,1|22,6', '', ''),
(67, 8, 7, 57600, 6400, 38400, 6400, 9600, 160000, '25,1|22,7', '', ''),
(68, 8, 8, 115200, 12800, 76800, 12800, 19200, 320000, '25,1|22,8', '', ''),
(69, 8, 9, 230400, 25600, 153600, 25600, 38400, 640000, '25,1|22,9', '', ''),
(70, 8, 10, 460800, 51200, 307200, 51200, 76800, 1280000, '25,1|22,10', '', ''),
(71, 9, 1, 1200, 120, 800, 0, 200, 3000, '25,1', '', '8,1'),
(72, 9, 2, 2400, 240, 1600, 0, 400, 6000, '25,1', '', '8,1'),
(73, 9, 3, 4800, 480, 3200, 0, 800, 12000, '25,1', '', '8,1'),
(74, 9, 4, 9600, 960, 6400, 0, 1600, 24000, '25,1', '', '8,1'),
(75, 9, 5, 19200, 1920, 12800, 0, 3200, 48000, '25,1', '', '8,1'),
(76, 9, 6, 38400, 3840, 25600, 0, 6400, 96000, '25,1', '', '8,1'),
(77, 9, 7, 76800, 7680, 51200, 0, 12800, 192000, '25,1', '', '8,1'),
(78, 9, 8, 153600, 15360, 102400, 0, 25600, 384000, '25,1', '', '8,1'),
(79, 9, 9, 307200, 30720, 204800, 0, 51200, 768000, '25,1', '', '8,1'),
(80, 9, 10, 614400, 61440, 409600, 0, 102400, 1536000, '25,1', '', '8,1'),
(81, 10, 1, 1500, 150, 700, 0, 300, 3500, '25,3', '', ''),
(82, 10, 2, 3000, 300, 1400, 0, 600, 7000, '25,3', '', ''),
(83, 10, 3, 6000, 600, 2800, 0, 1200, 14000, '25,3', '', ''),
(84, 10, 4, 12000, 1200, 5600, 0, 2400, 28000, '25,3', '', ''),
(85, 10, 5, 24000, 2400, 11200, 0, 4800, 56000, '25,3', '', ''),
(86, 10, 6, 48000, 4800, 22400, 0, 9600, 112000, '25,3', '', ''),
(87, 10, 7, 96000, 9600, 44800, 0, 19200, 224000, '25,3', '', ''),
(88, 10, 8, 192000, 19200, 89600, 0, 38400, 448000, '25,3', '', ''),
(89, 10, 9, 384000, 38400, 179200, 0, 76800, 896000, '25,3', '', ''),
(90, 10, 10, 768000, 76800, 358400, 0, 153600, 1792000, '25,3', '', ''),
(91, 11, 1, 1600, 0, 500, 200, 0, 3000, '25,4', '', '8,2'),
(92, 11, 2, 3200, 0, 1000, 400, 0, 6000, '25,4', '', '8,2'),
(93, 11, 3, 6400, 0, 2000, 800, 0, 12000, '25,4', '', '8,2'),
(94, 11, 4, 12800, 0, 4000, 1600, 0, 24000, '25,4', '', '8,2'),
(95, 11, 5, 25600, 0, 8000, 3200, 0, 48000, '25,4', '', '8,2'),
(96, 11, 6, 51200, 0, 16000, 6400, 0, 96000, '25,4', '', '8,2'),
(97, 11, 7, 102400, 0, 32000, 12800, 0, 192000, '25,4', '', '8,2'),
(98, 11, 8, 204800, 0, 64000, 25600, 0, 384000, '25,4', '', '8,2'),
(99, 11, 9, 409600, 0, 128000, 51200, 0, 768000, '25,4', '', '8,2'),
(100, 11, 10, 819200, 0, 256000, 102400, 0, 1536000, '25,4', '', '8,2'),
(101, 12, 1, 1800, 0, 600, 0, 0, 3000, '25,4', '', '8,3'),
(102, 12, 2, 3600, 0, 1200, 0, 0, 6000, '25,4', '', '8,3'),
(103, 12, 3, 7200, 0, 2400, 0, 0, 12000, '25,4', '', '8,3'),
(104, 12, 4, 14400, 0, 4800, 0, 0, 24000, '25,4', '', '8,3'),
(105, 12, 5, 28800, 0, 9600, 0, 0, 48000, '25,4', '', '8,3'),
(106, 12, 6, 57600, 0, 19200, 0, 0, 96000, '25,4', '', '8,3'),
(107, 12, 7, 115200, 0, 38400, 0, 0, 192000, '25,4', '', '8,3'),
(108, 12, 8, 230400, 0, 76800, 0, 0, 384000, '25,4', '', '8,3'),
(109, 12, 9, 460800, 0, 153600, 0, 0, 768000, '25,4', '', '8,3'),
(110, 12, 10, 921600, 0, 307200, 0, 0, 1536000, '25,4', '', '8,3'),
(111, 13, 1, 2000, 0, 1000, 0, 0, 6000, '25,5|20,1', '', '8,5'),
(112, 13, 2, 4000, 0, 2000, 0, 0, 12000, '25,5|20,1', '', '8,5'),
(113, 13, 3, 8000, 0, 4000, 0, 0, 24000, '25,5|20,1', '', '8,5'),
(114, 13, 4, 16000, 0, 8000, 0, 0, 48000, '25,5|20,1', '', '8,5'),
(115, 13, 5, 32000, 0, 16000, 0, 0, 96000, '25,5|20,1', '', '8,5'),
(116, 13, 6, 64000, 0, 32000, 0, 0, 192000, '25,5|20,1', '', '8,5'),
(117, 13, 7, 128000, 0, 64000, 0, 0, 384000, '25,5|20,1', '', '8,5'),
(118, 13, 8, 256000, 0, 128000, 0, 0, 768000, '25,5|20,1', '', '8,5'),
(119, 13, 9, 512000, 0, 256000, 0, 0, 1536000, '25,5|20,1', '', '8,5'),
(120, 13, 10, 1024000, 0, 512000, 0, 0, 3072000, '25,5|20,1', '', '8,5'),
(121, 14, 1, 2400, 800, 0, 500, 600, 5000, '25,4', '', '8,4'),
(122, 14, 2, 4800, 1600, 0, 1000, 1200, 10000, '25,4', '', '8,4'),
(123, 14, 3, 9600, 3200, 0, 2000, 2400, 20000, '25,4', '', '8,4'),
(124, 14, 4, 19200, 6400, 0, 4000, 4800, 40000, '25,4', '', '8,4'),
(125, 14, 5, 38400, 12800, 0, 8000, 9600, 80000, '25,4', '', '8,4'),
(126, 14, 6, 76800, 25600, 0, 16000, 19200, 160000, '25,4', '', '8,4'),
(127, 14, 7, 153600, 51200, 0, 32000, 38400, 320000, '25,4', '', '8,4'),
(128, 14, 8, 307200, 102400, 0, 64000, 76800, 640000, '25,4', '', '8,4'),
(129, 14, 9, 614400, 204800, 0, 128000, 153600, 1280000, '25,4', '', '8,4'),
(130, 14, 10, 1228800, 409600, 0, 256000, 307200, 2560000, '25,4', '', '8,4'),
(131, 15, 1, 900, 1200, 0, 1000, 800, 2000, '25,6|3,1', '', '2,3'),
(132, 15, 2, 1800, 2400, 0, 2000, 1600, 4000, '25,6|3,2', '', '2,3'),
(133, 15, 3, 3600, 4800, 0, 4000, 3200, 8000, '25,6|3,3', '', '2,3'),
(134, 15, 4, 7200, 9600, 0, 8000, 6400, 16000, '25,6|3,4', '', '2,3'),
(135, 15, 5, 14400, 19200, 0, 16000, 12800, 32000, '25,6|3,5', '', '2,3'),
(136, 15, 6, 28800, 38400, 0, 32000, 25600, 64000, '25,6|3,6', '', '2,3'),
(137, 15, 7, 57600, 76800, 0, 64000, 51200, 128000, '25,6|3,7', '', '2,3'),
(138, 15, 8, 115200, 153600, 0, 128000, 102400, 256000, '25,6|3,8', '', '2,3'),
(139, 15, 9, 230400, 307200, 0, 256000, 204800, 512000, '25,6|3,9', '', '2,3'),
(140, 15, 10, 460800, 614400, 0, 512000, 409600, 1024000, '25,6|3,10', '', '2,3'),
(141, 16, 1, 1800, 0, 1500, 0, 0, 3600, '25,6', '', '11,3'),
(142, 16, 2, 3600, 0, 3000, 0, 0, 7200, '25,6', '', '11,3'),
(143, 16, 3, 7200, 0, 6000, 0, 0, 14400, '25,6', '', '11,3'),
(144, 16, 4, 14400, 0, 12000, 0, 0, 28800, '25,6', '', '11,3'),
(145, 16, 5, 28800, 0, 24000, 0, 0, 57600, '25,6', '', '11,3'),
(146, 16, 6, 57600, 0, 48000, 0, 0, 115200, '25,6', '', '11,3'),
(147, 16, 7, 115200, 0, 96000, 0, 0, 230400, '25,6', '', '11,3'),
(148, 16, 8, 230400, 0, 192000, 0, 0, 460800, '25,6', '', '11,3'),
(149, 16, 9, 460800, 0, 384000, 0, 0, 921600, '25,6', '', '11,3'),
(150, 16, 10, 921600, 0, 768000, 0, 0, 1843200, '25,6', '', '11,3'),
(151, 17, 1, 1800, 2000, 0, 2000, 2000, 5000, '25,5', '', '2,5|5,2'),
(152, 17, 2, 3600, 4000, 0, 4000, 4000, 10000, '25,5', '', '2,5|5,2'),
(153, 17, 3, 7200, 8000, 0, 8000, 8000, 20000, '25,5', '', '2,5|5,2'),
(154, 17, 4, 14400, 16000, 0, 16000, 16000, 40000, '25,5', '', '2,5|5,2'),
(155, 17, 5, 28800, 32000, 0, 32000, 32000, 80000, '25,5', '', '2,5|5,2'),
(156, 17, 6, 57600, 64000, 0, 64000, 64000, 160000, '25,5', '', '2,5|5,2'),
(157, 17, 7, 115200, 128000, 0, 128000, 128000, 320000, '25,5', '', '2,5|5,2'),
(158, 17, 8, 230400, 256000, 0, 256000, 256000, 640000, '25,5', '', '2,5|5,2'),
(159, 17, 9, 460800, 512000, 0, 512000, 512000, 1280000, '25,5', '', '2,5|5,2'),
(160, 17, 10, 921600, 1024000, 0, 1024000, 1024000, 2560000, '25,5', '', '2,5|5,2'),
(161, 18, 1, 2100, 2500, 0, 3000, 1500, 6000, '25,8', '', '17,3'),
(162, 18, 2, 4200, 5000, 0, 6000, 3000, 12000, '25,8', '', '17,3'),
(163, 18, 3, 8400, 10000, 0, 12000, 6000, 24000, '25,8', '', '17,3'),
(164, 18, 4, 16800, 20000, 0, 24000, 12000, 48000, '25,8', '', '17,3'),
(165, 18, 5, 33600, 40000, 0, 48000, 24000, 96000, '25,8', '', '17,3'),
(166, 18, 6, 67200, 80000, 0, 96000, 48000, 192000, '25,8', '', '17,3'),
(167, 18, 7, 134400, 160000, 0, 192000, 96000, 384000, '25,8', '', '17,3'),
(168, 18, 8, 268800, 320000, 0, 384000, 192000, 768000, '25,8', '', '17,3'),
(169, 18, 9, 537600, 640000, 0, 768000, 384000, 1536000, '25,8', '', '17,3'),
(170, 18, 10, 1075200, 1280000, 0, 1536000, 768000, 3072000, '25,8', '', '17,3'),
(171, 19, 1, 2400, 600, 0, 700, 800, 5500, '25,9', '', '5,4'),
(172, 19, 2, 4800, 1200, 0, 1400, 1600, 11000, '25,9', '', '5,4'),
(173, 19, 3, 9600, 2400, 0, 2800, 3200, 22000, '25,9', '', '5,4'),
(174, 19, 4, 19200, 4800, 0, 5600, 6400, 44000, '25,9', '', '5,4'),
(175, 19, 5, 38400, 9600, 0, 11200, 12800, 88000, '25,9', '', '5,4'),
(176, 19, 6, 76800, 19200, 0, 22400, 25600, 176000, '25,9', '', '5,4'),
(177, 19, 7, 153600, 38400, 0, 44800, 51200, 352000, '25,9', '', '5,4'),
(178, 19, 8, 307200, 76800, 0, 89600, 102400, 704000, '25,9', '', '5,4'),
(179, 19, 9, 614400, 153600, 0, 179200, 204800, 1408000, '25,9', '', '5,4'),
(180, 19, 10, 1228800, 307200, 0, 358400, 409600, 2816000, '25,9', '', '5,4'),
(181, 20, 1, 3600, 0, 5000, 0, 2000, 10000, '25,10', '', '7,5|8,8'),
(182, 20, 2, 7200, 0, 10000, 0, 4000, 20000, '25,10', '', '7,5|8,8'),
(183, 20, 3, 14400, 0, 20000, 0, 8000, 40000, '25,10', '', '7,5|8,8'),
(184, 20, 4, 28800, 0, 40000, 0, 16000, 80000, '25,10', '', '7,5|8,8'),
(185, 20, 5, 57600, 0, 80000, 0, 32000, 160000, '25,10', '', '7,5|8,8'),
(186, 20, 6, 115200, 0, 160000, 0, 64000, 320000, '25,10', '', '7,5|8,8'),
(187, 20, 7, 230400, 0, 320000, 0, 128000, 640000, '25,10', '', '7,5|8,8'),
(188, 20, 8, 460800, 0, 640000, 0, 256000, 1280000, '25,10', '', '7,5|8,8'),
(189, 20, 9, 921600, 0, 1280000, 0, 512000, 2560000, '25,10', '', '7,5|8,8'),
(190, 20, 10, 1843200, 0, 2560000, 0, 1024000, 5120000, '25,10', '', '7,5|8,8');

-- --------------------------------------------------------

--
-- Table structure for table `config_troops`
--

CREATE TABLE `config_troops` (
  `troopid` int(10) UNSIGNED NOT NULL,
  `buildtime` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `population` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `wood` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `food` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `stone` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `iron` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `gold` int(10) UNSIGNED NOT NULL DEFAULT '1',
  `inside` int(10) UNSIGNED NOT NULL,
  `prereqbuilding` varchar(45) DEFAULT '',
  `prereqitem` varchar(45) DEFAULT '',
  `prereqtech` varchar(45) DEFAULT '',
  `life` int(10) UNSIGNED NOT NULL,
  `attack` int(10) UNSIGNED NOT NULL,
  `defense` int(10) UNSIGNED NOT NULL,
  `load` int(10) UNSIGNED NOT NULL,
  `movecost` int(10) UNSIGNED NOT NULL,
  `speed` int(10) UNSIGNED NOT NULL,
  `range` int(10) UNSIGNED NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `config_troops`
--

INSERT INTO `config_troops` (`troopid`, `buildtime`, `population`, `wood`, `food`, `stone`, `iron`, `gold`, `inside`, `prereqbuilding`, `prereqitem`, `prereqtech`, `life`, `attack`, `defense`, `load`, `movecost`, `speed`, `range`) VALUES
(2, 50, 1, 150, 50, 0, 10, 0, 1, '2,1', '', '', 100, 5, 10, 200, 2, 11, 10),
(3, 25, 1, 100, 80, 0, 50, 0, 1, '2,1', '', '', 200, 50, 50, 20, 3, 12, 20),
(4, 100, 1, 200, 120, 0, 150, 0, 1, '2,2', '', '7,1', 100, 20, 20, 5, 5, 180, 20),
(5, 150, 1, 500, 150, 0, 100, 0, 1, '2,2', '', '9,1', 300, 150, 150, 40, 6, 18, 50),
(6, 225, 1, 150, 200, 0, 400, 0, 1, '2,3', '', '10,1', 350, 100, 250, 30, 7, 17, 30),
(7, 350, 2, 350, 300, 0, 300, 0, 1, '2,4', '', '14,1', 250, 120, 50, 25, 9, 15, 1200),
(8, 1000, 3, 1500, 600, 0, 350, 0, 1, '2,6', '', '11,1|5,3', 500, 250, 180, 100, 18, 60, 100),
(9, 500, 6, 600, 1000, 0, 500, 0, 1, '2,5', '', '13,1', 1000, 350, 350, 80, 35, 45, 80),
(10, 1500, 4, 500, 2000, 0, 2500, 0, 1, '2,7', '', '10,5|13,5', 700, 10, 60, 5000, 10, 9, 10),
(11, 3000, 5, 3000, 2500, 0, 1800, 0, 1, '2,9', '', '5,5|14,6', 320, 450, 160, 35, 50, 6, 1400),
(12, 4500, 10, 6000, 4000, 0, 1500, 0, 1, '2,9', '', '10,8|5,7', 5000, 250, 160, 45, 100, 7, 600),
(13, 6000, 8, 5000, 5000, 8000, 1200, 0, 1, '2,10', '', '5,10|14,10', 480, 600, 200, 75, 250, 5, 1500),
(14, 60, 1, 500, 50, 100, 50, 0, 2, '32,1', '', '', 0, 0, 0, 0, 0, 0, 0),
(15, 120, 2, 1200, 100, 0, 150, 0, 2, '32,2', '', '5,1', 0, 0, 0, 0, 0, 0, 0),
(16, 180, 0, 2000, 200, 1000, 500, 0, 2, '32,3', '', '14,3', 0, 0, 0, 0, 0, 0, 0),
(18, 600, 4, 0, 600, 8000, 0, 0, 2, '32,7', '', '5,6', 0, 0, 0, 0, 0, 0, 0),
(17, 360, 5, 6000, 300, 0, 0, 0, 2, '32,5', '', '5,4', 0, 0, 0, 0, 0, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `pma__bookmark`
--

CREATE TABLE `pma__bookmark` (
  `id` int(11) NOT NULL,
  `dbase` varchar(255) COLLATE utf8_bin NOT NULL DEFAULT '',
  `user` varchar(255) COLLATE utf8_bin NOT NULL DEFAULT '',
  `label` varchar(255) CHARACTER SET utf8 NOT NULL DEFAULT '',
  `query` text COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Bookmarks';

-- --------------------------------------------------------

--
-- Table structure for table `pma__central_columns`
--

CREATE TABLE `pma__central_columns` (
  `db_name` varchar(64) COLLATE utf8_bin NOT NULL,
  `col_name` varchar(64) COLLATE utf8_bin NOT NULL,
  `col_type` varchar(64) COLLATE utf8_bin NOT NULL,
  `col_length` text COLLATE utf8_bin,
  `col_collation` varchar(64) COLLATE utf8_bin NOT NULL,
  `col_isNull` tinyint(1) NOT NULL,
  `col_extra` varchar(255) COLLATE utf8_bin DEFAULT '',
  `col_default` text COLLATE utf8_bin
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Central list of columns';

-- --------------------------------------------------------

--
-- Table structure for table `pma__column_info`
--

CREATE TABLE `pma__column_info` (
  `id` int(5) UNSIGNED NOT NULL,
  `db_name` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `table_name` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `column_name` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `comment` varchar(255) CHARACTER SET utf8 NOT NULL DEFAULT '',
  `mimetype` varchar(255) CHARACTER SET utf8 NOT NULL DEFAULT '',
  `transformation` varchar(255) COLLATE utf8_bin NOT NULL DEFAULT '',
  `transformation_options` varchar(255) COLLATE utf8_bin NOT NULL DEFAULT '',
  `input_transformation` varchar(255) COLLATE utf8_bin NOT NULL DEFAULT '',
  `input_transformation_options` varchar(255) COLLATE utf8_bin NOT NULL DEFAULT ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Column information for phpMyAdmin';

-- --------------------------------------------------------

--
-- Table structure for table `pma__designer_settings`
--

CREATE TABLE `pma__designer_settings` (
  `username` varchar(64) COLLATE utf8_bin NOT NULL,
  `settings_data` text COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Settings related to Designer';

-- --------------------------------------------------------

--
-- Table structure for table `pma__export_templates`
--

CREATE TABLE `pma__export_templates` (
  `id` int(5) UNSIGNED NOT NULL,
  `username` varchar(64) COLLATE utf8_bin NOT NULL,
  `export_type` varchar(10) COLLATE utf8_bin NOT NULL,
  `template_name` varchar(64) COLLATE utf8_bin NOT NULL,
  `template_data` text COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Saved export templates';

--
-- Dumping data for table `pma__export_templates`
--

INSERT INTO `pma__export_templates` (`id`, `username`, `export_type`, `template_name`, `template_data`) VALUES
(1, 'root', 'database', 'evdb', '{\"quick_or_custom\":\"quick\",\"what\":\"sql\",\"structure_or_data_forced\":\"0\",\"table_select[]\":[\"account\",\"config_building\",\"config_items\",\"config_research\",\"config_troops\",\"pma__bookmark\",\"pma__central_columns\",\"pma__column_info\",\"pma__designer_settings\",\"pma__export_templates\",\"pma__favorite\",\"pma__history\",\"pma__navigationhiding\",\"pma__pdf_pages\",\"pma__recent\",\"pma__relation\",\"pma__savedsearches\",\"pma__table_coords\",\"pma__table_info\",\"pma__table_uiprefs\",\"pma__tracking\",\"pma__userconfig\",\"pma__usergroups\",\"pma__users\",\"servers\",\"settings\"],\"table_structure[]\":[\"account\",\"config_building\",\"config_items\",\"config_research\",\"config_troops\",\"pma__bookmark\",\"pma__central_columns\",\"pma__column_info\",\"pma__designer_settings\",\"pma__export_templates\",\"pma__favorite\",\"pma__history\",\"pma__navigationhiding\",\"pma__pdf_pages\",\"pma__recent\",\"pma__relation\",\"pma__savedsearches\",\"pma__table_coords\",\"pma__table_info\",\"pma__table_uiprefs\",\"pma__tracking\",\"pma__userconfig\",\"pma__usergroups\",\"pma__users\",\"servers\",\"settings\"],\"table_data[]\":[\"account\",\"config_building\",\"config_items\",\"config_research\",\"config_troops\",\"pma__bookmark\",\"pma__central_columns\",\"pma__column_info\",\"pma__designer_settings\",\"pma__export_templates\",\"pma__favorite\",\"pma__history\",\"pma__navigationhiding\",\"pma__pdf_pages\",\"pma__recent\",\"pma__relation\",\"pma__savedsearches\",\"pma__table_coords\",\"pma__table_info\",\"pma__table_uiprefs\",\"pma__tracking\",\"pma__userconfig\",\"pma__usergroups\",\"pma__users\",\"servers\",\"settings\"],\"output_format\":\"sendit\",\"filename_template\":\"@DATABASE@\",\"remember_template\":\"on\",\"charset\":\"utf-8\",\"compression\":\"none\",\"maxsize\":\"\",\"yaml_structure_or_data\":\"data\",\"phparray_structure_or_data\":\"data\",\"xml_structure_or_data\":\"data\",\"xml_export_events\":\"something\",\"xml_export_functions\":\"something\",\"xml_export_procedures\":\"something\",\"xml_export_tables\":\"something\",\"xml_export_triggers\":\"something\",\"xml_export_views\":\"something\",\"xml_export_contents\":\"something\",\"sql_include_comments\":\"something\",\"sql_header_comment\":\"\",\"sql_compatibility\":\"NONE\",\"sql_structure_or_data\":\"structure_and_data\",\"sql_create_table\":\"something\",\"sql_auto_increment\":\"something\",\"sql_create_view\":\"something\",\"sql_procedure_function\":\"something\",\"sql_create_trigger\":\"something\",\"sql_backquotes\":\"something\",\"sql_type\":\"INSERT\",\"sql_insert_syntax\":\"both\",\"sql_max_query_size\":\"50000\",\"sql_hex_for_binary\":\"something\",\"sql_utc_time\":\"something\",\"mediawiki_structure_or_data\":\"structure_and_data\",\"mediawiki_caption\":\"something\",\"mediawiki_headers\":\"something\",\"texytext_structure_or_data\":\"structure_and_data\",\"texytext_null\":\"NULL\",\"csv_separator\":\",\",\"csv_enclosed\":\"\\\"\",\"csv_escaped\":\"\\\"\",\"csv_terminated\":\"AUTO\",\"csv_null\":\"NULL\",\"csv_structure_or_data\":\"data\",\"latex_caption\":\"something\",\"latex_structure_or_data\":\"structure_and_data\",\"latex_structure_caption\":\"Structure of table @TABLE@\",\"latex_structure_continued_caption\":\"Structure of table @TABLE@ (continued)\",\"latex_structure_label\":\"tab:@TABLE@-structure\",\"latex_relation\":\"something\",\"latex_comments\":\"something\",\"latex_mime\":\"something\",\"latex_columns\":\"something\",\"latex_data_caption\":\"Content of table @TABLE@\",\"latex_data_continued_caption\":\"Content of table @TABLE@ (continued)\",\"latex_data_label\":\"tab:@TABLE@-data\",\"latex_null\":\"\\\\textit{NULL}\",\"json_structure_or_data\":\"data\",\"codegen_structure_or_data\":\"data\",\"codegen_format\":\"0\",\"odt_structure_or_data\":\"structure_and_data\",\"odt_relation\":\"something\",\"odt_comments\":\"something\",\"odt_mime\":\"something\",\"odt_columns\":\"something\",\"odt_null\":\"NULL\",\"ods_null\":\"NULL\",\"ods_structure_or_data\":\"data\",\"pdf_report_title\":\"\",\"pdf_structure_or_data\":\"structure_and_data\",\"htmlword_structure_or_data\":\"structure_and_data\",\"htmlword_null\":\"NULL\",\"excel_null\":\"NULL\",\"excel_edition\":\"win\",\"excel_structure_or_data\":\"data\",\"\":null,\"lock_tables\":null,\"as_separate_files\":null,\"sql_dates\":null,\"sql_relation\":null,\"sql_mime\":null,\"sql_use_transaction\":null,\"sql_disable_fk\":null,\"sql_views_as_tables\":null,\"sql_metadata\":null,\"sql_create_database\":null,\"sql_drop_table\":null,\"sql_if_not_exists\":null,\"sql_truncate\":null,\"sql_delayed\":null,\"sql_ignore\":null,\"texytext_columns\":null,\"csv_removeCRLF\":null,\"csv_columns\":null,\"json_pretty_print\":null,\"ods_columns\":null,\"htmlword_columns\":null,\"excel_removeCRLF\":null,\"excel_columns\":null}');

-- --------------------------------------------------------

--
-- Table structure for table `pma__favorite`
--

CREATE TABLE `pma__favorite` (
  `username` varchar(64) COLLATE utf8_bin NOT NULL,
  `tables` text COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Favorite tables';

-- --------------------------------------------------------

--
-- Table structure for table `pma__history`
--

CREATE TABLE `pma__history` (
  `id` bigint(20) UNSIGNED NOT NULL,
  `username` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `db` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `table` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `timevalue` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `sqlquery` text COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='SQL history for phpMyAdmin';

-- --------------------------------------------------------

--
-- Table structure for table `pma__navigationhiding`
--

CREATE TABLE `pma__navigationhiding` (
  `username` varchar(64) COLLATE utf8_bin NOT NULL,
  `item_name` varchar(64) COLLATE utf8_bin NOT NULL,
  `item_type` varchar(64) COLLATE utf8_bin NOT NULL,
  `db_name` varchar(64) COLLATE utf8_bin NOT NULL,
  `table_name` varchar(64) COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Hidden items of navigation tree';

-- --------------------------------------------------------

--
-- Table structure for table `pma__pdf_pages`
--

CREATE TABLE `pma__pdf_pages` (
  `db_name` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `page_nr` int(10) UNSIGNED NOT NULL,
  `page_descr` varchar(50) CHARACTER SET utf8 NOT NULL DEFAULT ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='PDF relation pages for phpMyAdmin';

-- --------------------------------------------------------

--
-- Table structure for table `pma__recent`
--

CREATE TABLE `pma__recent` (
  `username` varchar(64) COLLATE utf8_bin NOT NULL,
  `tables` text COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Recently accessed tables';

-- --------------------------------------------------------

--
-- Table structure for table `pma__relation`
--

CREATE TABLE `pma__relation` (
  `master_db` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `master_table` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `master_field` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `foreign_db` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `foreign_table` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `foreign_field` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Relation table';

-- --------------------------------------------------------

--
-- Table structure for table `pma__savedsearches`
--

CREATE TABLE `pma__savedsearches` (
  `id` int(5) UNSIGNED NOT NULL,
  `username` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `db_name` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `search_name` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `search_data` text COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Saved searches';

-- --------------------------------------------------------

--
-- Table structure for table `pma__table_coords`
--

CREATE TABLE `pma__table_coords` (
  `db_name` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `table_name` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `pdf_page_number` int(11) NOT NULL DEFAULT '0',
  `x` float UNSIGNED NOT NULL DEFAULT '0',
  `y` float UNSIGNED NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Table coordinates for phpMyAdmin PDF output';

-- --------------------------------------------------------

--
-- Table structure for table `pma__table_info`
--

CREATE TABLE `pma__table_info` (
  `db_name` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `table_name` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `display_field` varchar(64) COLLATE utf8_bin NOT NULL DEFAULT ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Table information for phpMyAdmin';

-- --------------------------------------------------------

--
-- Table structure for table `pma__table_uiprefs`
--

CREATE TABLE `pma__table_uiprefs` (
  `username` varchar(64) COLLATE utf8_bin NOT NULL,
  `db_name` varchar(64) COLLATE utf8_bin NOT NULL,
  `table_name` varchar(64) COLLATE utf8_bin NOT NULL,
  `prefs` text COLLATE utf8_bin NOT NULL,
  `last_update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Tables'' UI preferences';

-- --------------------------------------------------------

--
-- Table structure for table `pma__tracking`
--

CREATE TABLE `pma__tracking` (
  `db_name` varchar(64) COLLATE utf8_bin NOT NULL,
  `table_name` varchar(64) COLLATE utf8_bin NOT NULL,
  `version` int(10) UNSIGNED NOT NULL,
  `date_created` datetime NOT NULL,
  `date_updated` datetime NOT NULL,
  `schema_snapshot` text COLLATE utf8_bin NOT NULL,
  `schema_sql` text COLLATE utf8_bin,
  `data_sql` longtext COLLATE utf8_bin,
  `tracking` set('UPDATE','REPLACE','INSERT','DELETE','TRUNCATE','CREATE DATABASE','ALTER DATABASE','DROP DATABASE','CREATE TABLE','ALTER TABLE','RENAME TABLE','DROP TABLE','CREATE INDEX','DROP INDEX','CREATE VIEW','ALTER VIEW','DROP VIEW') COLLATE utf8_bin DEFAULT NULL,
  `tracking_active` int(1) UNSIGNED NOT NULL DEFAULT '1'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Database changes tracking for phpMyAdmin';

-- --------------------------------------------------------

--
-- Table structure for table `pma__userconfig`
--

CREATE TABLE `pma__userconfig` (
  `username` varchar(64) COLLATE utf8_bin NOT NULL,
  `timevalue` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `config_data` text COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='User preferences storage for phpMyAdmin';

-- --------------------------------------------------------

--
-- Table structure for table `pma__usergroups`
--

CREATE TABLE `pma__usergroups` (
  `usergroup` varchar(64) COLLATE utf8_bin NOT NULL,
  `tab` varchar(64) COLLATE utf8_bin NOT NULL,
  `allowed` enum('Y','N') COLLATE utf8_bin NOT NULL DEFAULT 'N'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='User groups with configured menu items';

-- --------------------------------------------------------

--
-- Table structure for table `pma__users`
--

CREATE TABLE `pma__users` (
  `username` varchar(64) COLLATE utf8_bin NOT NULL,
  `usergroup` varchar(64) COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='Users and their assignments to user groups';

-- --------------------------------------------------------

--
-- Table structure for table `servers`
--

CREATE TABLE `servers` (
  `id` int(10) UNSIGNED NOT NULL,
  `name` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `ip` varchar(16) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `settings`
--

CREATE TABLE `settings` (
  `id` int(10) UNSIGNED NOT NULL,
  `server` varchar(10) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `setting` varchar(45) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `options` text COLLATE utf8_unicode_ci NOT NULL,
  `desription` varchar(100) COLLATE utf8_unicode_ci NOT NULL DEFAULT ''
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Dumping data for table `settings`
--

INSERT INTO `settings` (`id`, `server`, `setting`, `options`, `desription`) VALUES
(1, '2', 'itemxml', '<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<itemdef>\r\n<items>\r\n<itemEum id=\"player.box.compensation.e\" name=\"Compensation Package\" itemType=\"宝箱\" dayLimit=\"0\" userLimit=\"1\" desc=\"Includes: 10 amulets, 100 cents.\" itemDesc=\"This package was sent to every member of your server to apologize for extended downtime.\" iconUrl=\"images/items/chongzhidalibao.png\" price=\"0\" playerItem=\"true\"/>\r\n<itemEum id=\"player.box.present.money.44\" name=\"Pamplona Prize Pack\" itemType=\"宝箱\" dayLimit=\"0\" userLimit=\"1\" desc=\"Includes: Wooden Bull Opener, Lion Medal, Rose Medal, Cross Medal, Primary Guidelines, Intermediate Guidelines, War Horn, Corselet, Holy Water, Hero Hunting, Truce Agreement, City Teleporter, Amulet.\" itemDesc=\"These packages are delivered as gifts to players for every $30 worth of purchases made during our Run with the Bulls promotion.\" iconUrl=\"images/icon/shop/PamplonaPrizePack.png\" price=\"0\" playerItem=\"true\"/>\r\n<itemEum id=\"player.box.present.money.45\" name=\"Hollow Wooden Bull\" itemType=\"宝箱\" dayLimit=\"0\" userLimit=\"1\" desc=\"Includes: Chest A (Freedom Medal, Justice Medal, Nation Medal, Michelangelo\'s Script, Plowshares, Arch Saw, Quarrying Tools, Blower, War Ensign, Excalibur, The Wealth of Nations, Amulet) or Chest B (Primary Guidelines, Intermediate Guidelines, Hero Hunting, Merchant Fleet, Plowshares, Double Saw, Quarrying Tools, Blower, Michelangelo\'s Script, Tax Policy, The Wealth of Nations) or Chest C (Excalibur, War Horn, Corselet, Truce Agreement, War Ensign, Adv City Teleporter, Michelangelo\'s Script)\" itemDesc=\"These chests are sent to you as Run with the Bulls gifts from your friends in the game. They require a Wooden Bull Opener to open. You can obtain a Wooden Bull Opener for every $30 worth of purchases made during the Run with the Bulls promotion. When opened, you will receive the contents of Hollow Wooden Bull A, B or C at random.\" iconUrl=\"images/icon/shop/HollowWoodenBull.png\" price=\"300\" playerItem=\"true\"/>\r\n<itemEum id=\"player.key.bull\" name=\"Wooden Bull Opener\" itemType=\"宝箱\" dayLimit=\"0\" userLimit=\"0\" desc=\"You can use this key to open one Hollow Wooden Bull sent to you by your friends. If you don’t have any Hollow Wooden Bull, you should ask your friends to send you some!\" itemDesc=\"You can open any Hollow Wooden Bull your friends gave you with this key once.\" iconUrl=\"images/icon/shop/WoodenBullOpener.png\" price=\"0\"/>\r\n<itemEum id=\"player.running.shoes\" name=\"Extra-Fast Running Shoes\" itemType=\"宝箱\" dayLimit=\"0\" userLimit=\"0\" desc=\"A gift from your friends around Run with the Bulls. Use it to get 24 hours of 50% upkeep in ALL your cities any time from July 9th through July 13th. Extra-Fast Running Shoes is stackable (meaning if you already have this buff, using it again will add an additional 24 hours). Once July 14th comes, this item will expire if you haven\'t used it yet.\" itemDesc=\"Get a 24 hours 50% upkeep buff during July 9th and July 13th.\" iconUrl=\"images/icon/shop/RunningShoes.png\" price=\"0\" playerItem=\"true\"/>\r\n<itemEum id=\"player.box.test.1\" name=\"Test Item\" itemType=\"宝箱\" dayLimit=\"0\" userLimit=\"0\" desc=\"Includes: test items.\" itemDesc=\"This package exists as a test.\" iconUrl=\"images/items/chongzhidalibao.png\" price=\"10\" playerItem=\"true\"/></items><special><pack id=\"Special Christmas Chest\"/><pack id=\"Special New Year Chest\"/><pack id=\"Special Easter Chest\"/>\r\n<itemEum id=\"alliance.ritual_of_pact.ultimate\" name=\"Ritual of Pact (Ultimate)\" itemType=\"宝箱\" dayLimit=\"90\" userLimit=\"0\" desc=\"Ritual of Pact (Ultimate): member limit is 1,000; effective for 90 days; leeway period is 7 days.\" itemDesc=\"It allows alliance to increase member limit to 1,000 once applied, which is effective for 90 days, while multiple applications of this item can lengthen the effective period. Once the item effect is due, 7-day leeway is given to the alliance. During this time, new members are denied to be recruited to the alliance. If no further application of the item, the alliance disbands automatically once the 7-day leeway period passes.\" iconUrl=\"images/items/Ritual_of_Pact_Ultimate.png\" price=\"75\"/>\r\n<pack id=\"Special Evony Happiness Chest\"/>\r\n<pack id=\"Halloween Chest O\'Treats\"/>\r\n<pack id=\"Special Thanksgiving Package\"/>\r\n<pack id=\"Secret Santa Chest\"/>\r\n<pack id=\"Valentine\'s Day Chest \"/>\r\n<pack id=\"St Patrick\'s Day Chest\"/>\r\n<pack id=\"Special Easter Chest\"/>\r\n<pack id=\"Hollow Wooden Bull\"/>\r\n</special>\r\n</itemdef>', 'Item XML of Server 2');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `account`
--
ALTER TABLE `account`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `config_building`
--
ALTER TABLE `config_building`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `config_items`
--
ALTER TABLE `config_items`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `config_quests`
--
ALTER TABLE `config_quests`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `id` (`id`);

--
-- Indexes for table `config_research`
--
ALTER TABLE `config_research`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `config_troops`
--
ALTER TABLE `config_troops`
  ADD PRIMARY KEY (`troopid`) USING BTREE;

--
-- Indexes for table `pma__bookmark`
--
ALTER TABLE `pma__bookmark`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `pma__central_columns`
--
ALTER TABLE `pma__central_columns`
  ADD PRIMARY KEY (`db_name`,`col_name`);

--
-- Indexes for table `pma__column_info`
--
ALTER TABLE `pma__column_info`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `db_name` (`db_name`,`table_name`,`column_name`);

--
-- Indexes for table `pma__designer_settings`
--
ALTER TABLE `pma__designer_settings`
  ADD PRIMARY KEY (`username`);

--
-- Indexes for table `pma__export_templates`
--
ALTER TABLE `pma__export_templates`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `u_user_type_template` (`username`,`export_type`,`template_name`);

--
-- Indexes for table `pma__favorite`
--
ALTER TABLE `pma__favorite`
  ADD PRIMARY KEY (`username`);

--
-- Indexes for table `pma__history`
--
ALTER TABLE `pma__history`
  ADD PRIMARY KEY (`id`),
  ADD KEY `username` (`username`,`db`,`table`,`timevalue`);

--
-- Indexes for table `pma__navigationhiding`
--
ALTER TABLE `pma__navigationhiding`
  ADD PRIMARY KEY (`username`,`item_name`,`item_type`,`db_name`,`table_name`);

--
-- Indexes for table `pma__pdf_pages`
--
ALTER TABLE `pma__pdf_pages`
  ADD PRIMARY KEY (`page_nr`),
  ADD KEY `db_name` (`db_name`);

--
-- Indexes for table `pma__recent`
--
ALTER TABLE `pma__recent`
  ADD PRIMARY KEY (`username`);

--
-- Indexes for table `pma__relation`
--
ALTER TABLE `pma__relation`
  ADD PRIMARY KEY (`master_db`,`master_table`,`master_field`),
  ADD KEY `foreign_field` (`foreign_db`,`foreign_table`);

--
-- Indexes for table `pma__savedsearches`
--
ALTER TABLE `pma__savedsearches`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `u_savedsearches_username_dbname` (`username`,`db_name`,`search_name`);

--
-- Indexes for table `pma__table_coords`
--
ALTER TABLE `pma__table_coords`
  ADD PRIMARY KEY (`db_name`,`table_name`,`pdf_page_number`);

--
-- Indexes for table `pma__table_info`
--
ALTER TABLE `pma__table_info`
  ADD PRIMARY KEY (`db_name`,`table_name`);

--
-- Indexes for table `pma__table_uiprefs`
--
ALTER TABLE `pma__table_uiprefs`
  ADD PRIMARY KEY (`username`,`db_name`,`table_name`);

--
-- Indexes for table `pma__tracking`
--
ALTER TABLE `pma__tracking`
  ADD PRIMARY KEY (`db_name`,`table_name`,`version`);

--
-- Indexes for table `pma__userconfig`
--
ALTER TABLE `pma__userconfig`
  ADD PRIMARY KEY (`username`);

--
-- Indexes for table `pma__usergroups`
--
ALTER TABLE `pma__usergroups`
  ADD PRIMARY KEY (`usergroup`,`tab`,`allowed`);

--
-- Indexes for table `pma__users`
--
ALTER TABLE `pma__users`
  ADD PRIMARY KEY (`username`,`usergroup`);

--
-- Indexes for table `servers`
--
ALTER TABLE `servers`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `settings`
--
ALTER TABLE `settings`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `account`
--
ALTER TABLE `account`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=132;
--
-- AUTO_INCREMENT for table `config_building`
--
ALTER TABLE `config_building`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=207;
--
-- AUTO_INCREMENT for table `config_items`
--
ALTER TABLE `config_items`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=230;
--
-- AUTO_INCREMENT for table `config_research`
--
ALTER TABLE `config_research`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=191;
--
-- AUTO_INCREMENT for table `pma__bookmark`
--
ALTER TABLE `pma__bookmark`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `pma__column_info`
--
ALTER TABLE `pma__column_info`
  MODIFY `id` int(5) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `pma__export_templates`
--
ALTER TABLE `pma__export_templates`
  MODIFY `id` int(5) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `pma__history`
--
ALTER TABLE `pma__history`
  MODIFY `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `pma__pdf_pages`
--
ALTER TABLE `pma__pdf_pages`
  MODIFY `page_nr` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `pma__savedsearches`
--
ALTER TABLE `pma__savedsearches`
  MODIFY `id` int(5) UNSIGNED NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `servers`
--
ALTER TABLE `servers`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `settings`
--
ALTER TABLE `settings`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
