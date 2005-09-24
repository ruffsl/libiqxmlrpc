//  Libiqnet + Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004 Anton Dedov
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
//  
//  $Id: reactor_poll_impl.h,v 1.2 2005-09-24 16:24:57 bada Exp $

#ifndef _iqxmlrpc_reactor_poll_impl_h_
#define _iqxmlrpc_reactor_poll_impl_h_

#include "config.h"

#ifdef HAVE_POLL
#include <boost/utility.hpp>
#include "reactor.h"
#include "sysinc.h"

namespace iqnet
{

//! Reactor implementation helper based on poll() system call.
class Reactor_poll_impl: boost::noncopyable {
  struct Impl;
  Impl* impl;

public:
  Reactor_poll_impl();
  virtual ~Reactor_poll_impl();
  
  void reset(const Reactor_base::HandlerStateList&);
  bool poll(Reactor_base::HandlerStateList& out, Reactor_base::Timeout);
};

} // namespace iqnet

#endif // HAVE_POLL
#endif
