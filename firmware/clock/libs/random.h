// #############################################################################
// #                   --- IW18 VFD Clock Firmware ---                         #
// #############################################################################
// # random.h - Header: Functions to create some 'random' data                 #
// #############################################################################
// #              Version: 2.1 - Compiler: AVR-GCC 4.5.0 (Linux)               #
// #  (c) 08-11 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@maltepoeggel.de  #
// #############################################################################
// #  This program is free software; you can redistribute it and/or modify it  #
// #   under the terms of the GNU General Public License as published by the   #
// #        Free Software Foundation; either version 3 of the License,         #
// #                  or (at your option) any later version.                   #
// #                                                                           #
// #      This program is distributed in the hope that it will be useful,      #
// #      but WITHOUT ANY WARRANTY; without even the implied warranty of       #
// #           MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            #
// #           See the GNU General Public License for more details.            #
// #                                                                           #
// #  You should have received a copy of the GNU General Public License along  #
// #      with this program; if not, see <http://www.gnu.org/licenses/>.       #
// #############################################################################

#ifndef RANDOM_H
 #define RANDOM_H
 
 void InitRandom( uint8_t max );
 void RotateRandom( void );
 uint8_t GetRandom( void );

#endif
