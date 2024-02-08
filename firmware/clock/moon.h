// #############################################################################
// #                   --- IV18 VFD Clock Firmware ---                         #
// #############################################################################
// # moon.h - Header: Moon phase calculation                                   #
// # Using code from http://www.voidware.com/moon_phase.htm                    #
// # The non floating point (second) algorithm there seems to be accurate      #
// # enought for our purpose :-)                                               #
// #############################################################################
// #              Version: 2.2 - Compiler: AVR-GCC 4.5.0 (Linux)               #
// #    (c) 08-24 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@poeggel.de     #
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

#ifndef MOON_H
 #define MOON_H
 
 uint8_t Moon_phase(uint16_t year, uint16_t month, uint16_t day);

#endif
