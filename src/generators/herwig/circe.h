c circe.h -- canonical beam spectra for linear collider physics
c $Id$
c   Copyright (C) 1996-2002 by Thorsten Ohl <ohl@hep.tu-darmstadt.de>
c
c   Circe is free software; you can redistribute it and/or modify it
c   under the terms of the GNU General Public License as published by
c   the Free Software Foundation; either version 2, or (at your option)
c   any later version.
c
c   Circe is distributed in the hope that it will be useful, but
c   WITHOUT ANY WARRANTY; without even the implied warranty of
c   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
c   GNU General Public License for more details.
c
c   You should have received a copy of the GNU General Public License
c   along with this program; if not, write to the Free Software
c   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
      integer ELECTR, POSITR, PHOTON
      parameter (ELECTR =  11)
      parameter (POSITR = -11)
      parameter (PHOTON =  22)
      integer SBAND, TESLA, XBAND
      parameter (SBAND  =  1, TESLA  =  2, XBAND  =  3)
      integer JLCNLC
      parameter (JLCNLC =  3)
      integer SBNDEE, TESLEE, XBNDEE
      parameter (SBNDEE =  4, TESLEE =  5, XBNDEE =  6)
      integer NLCH
      parameter (NLCH =  7)
      integer NACC
      parameter (NACC = 7)
