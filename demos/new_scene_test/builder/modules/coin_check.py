/* 
 *  coin_check.py
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 2009-02-24 16:01:56 +0100
 *  Author(s): Felix Klein
 */
 
 from builder.btools import RegisterCustomTest

def CheckCoin(ctx):
    ctx.Message('Checking for Coin... ')
    ret, output = ctx.TryAction('coin-config --version')
    if ret:
        ctx.env.ParseConfig('coin-config --cflags --cxxflags --cppflags --ldflags')
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckCoin', CheckCoin)
