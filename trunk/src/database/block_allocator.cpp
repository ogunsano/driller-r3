/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * block_allocator.cpp
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

#include "block_allocator.h"

namespace Driller {

BlockAllocator::BlockAllocator(const unsigned int _block_size):
  block_size(_block_size),
  current_offset(0),
  current_block(new char[block_size]){
}

BlockAllocator::~BlockAllocator(){
  delete[] reinterpret_cast<char*>(current_block);

  // Delete all filled blocks
  std::list<void*>::iterator iter;
  for(iter = filled_blocks.begin(); iter != filled_blocks.end(); iter++){
    delete[] reinterpret_cast<char*>(*iter);
  }
}

void* BlockAllocator::real_allocate(const unsigned int amount){
  // If an amount is being requested that cannot fit in one block,
  // create a special block just to hold it
  if (amount >= block_size){
    void* new_block = new char[amount];

    // Immediately push the block onto the list of full blocks
    filled_blocks.push_back(new_block);

    return new_block;
  }

  // If the amount cannot fit within the current block, create a new one
  if (amount + current_offset > block_size){
    // Push the existing block onto the list of full blocks
    filled_blocks.push_back(current_block);

    // Allocate a new block
    current_block = new char[block_size];

    // Reset the current offset
    current_offset = 0;
  }

  char* memory = reinterpret_cast<char*>(current_block) + current_offset;
  current_offset += amount;
  return memory;
}

} // namespace

