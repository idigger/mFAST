// Copyright (c) 2013, Huang-Ming Huang,  Object Computing, Inc.
// All rights reserved.
//
// This file is part of mFAST.
//
//     mFAST is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Lesser General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     mFAST is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU Lesser General Public License
//     along with mFast.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef DEBUG_ALLOCATOR_H_PPV2L7KE
#define DEBUG_ALLOCATOR_H_PPV2L7KE


#include <mfast/malloc_allocator.h>
#include <set>

class debug_allocator : public mfast::malloc_allocator
{
public:

  debug_allocator() {
    BOOST_CHECK_EQUAL(leased_addresses_.size(), 0);
  }

  virtual void* allocate(std::size_t s) {
    void* pointer = std::malloc(s);
    if (pointer == 0) throw std::bad_alloc();
    leased_addresses_.insert(pointer);
    return pointer;
  }

  virtual std::size_t reallocate(void*& pointer, std::size_t /* old_size */, std::size_t new_size)
  {
    pointer = std::realloc(pointer, new_size);
    if (pointer == 0) throw std::bad_alloc();
    leased_addresses_.insert(pointer);
    return new_size;
  }

  virtual void deallocate(void* pointer) {

    BOOST_CHECK(leased_addresses_.count(pointer));
    std::free(pointer);
    leased_addresses_.erase(pointer);
  }
 private:
  std::set<void*> leased_addresses_;
};


#endif /* end of include guard: DEBUG_ALLOCATOR_H_PPV2L7KE */