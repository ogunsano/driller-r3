/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * block_allocator.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef DRILLER_BLOCK_ALLOCATOR_H
#define DRILLER_BLOCK_ALLOCATOR_H

#include <list>

namespace Driller {

/**
  Allocates memory in blocks of arbitrary size, which is then sliced up
  and given to a user. Very useful for use with lots of small blocks

  Use of memory is not tracked. The only way to free any memory is to
  delete the BlockAllocator
*/
class BlockAllocator {
public:
  /**
    Create a new BlockAllocator, which uses the specified block size

    @param block_size The block size
  */
  BlockAllocator(const unsigned int block_size);

  /**
    De-allocate all memory used by the BlockAllocator
  */
  ~BlockAllocator();

  /**
    Allocate count objects of type T

    @param count How many objects should be allocated

    @return count objects of type T. Don't free or delete this
  */
  template<typename T>
  T* allocate(const unsigned int count){
    return reinterpret_cast<T*>(real_allocate(sizeof(T) * count));
  }

protected:
  /**
    Allocate some chunk of memory

    @param amount How much memory should be allocated, in bytes

    @return The allocated chunk
  */
  void* real_allocate(const unsigned int amount);

  /** How big each block is */
  const unsigned int block_size;

  /** The current offset into the current block */
  unsigned int current_offset;

  /** The block currently being filled */
  void* current_block;

  /** A list of filled blocks */
  std::list<void*> filled_blocks;
};

} // namespace

#endif // DRILLER_BLOCK_ALLOCATOR_H
