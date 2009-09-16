/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-9 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE


#include "juce_MemoryOutputStream.h"


//==============================================================================
MemoryOutputStream::MemoryOutputStream (const int initialSize,
                                        const int blockSizeToIncreaseBy,
                                        MemoryBlock* const memoryBlockToWriteTo) throw()
  : data (memoryBlockToWriteTo),
    position (0),
    size (0),
    blockSize (jmax (16, blockSizeToIncreaseBy)),
    ownsMemoryBlock (memoryBlockToWriteTo == 0)
{
    if (memoryBlockToWriteTo == 0)
        data = new MemoryBlock (initialSize);
    else
        memoryBlockToWriteTo->setSize (initialSize, false);
}

MemoryOutputStream::~MemoryOutputStream() throw()
{
    if (ownsMemoryBlock)
        delete data;
    else
        flush();
}

void MemoryOutputStream::flush()
{
    if (! ownsMemoryBlock)
        data->setSize (size, false);
}

void MemoryOutputStream::reset() throw()
{
    position = 0;
    size = 0;
}

bool MemoryOutputStream::write (const void* buffer, int howMany)
{
    if (howMany > 0)
    {
        int storageNeeded = position + howMany;

        if (storageNeeded >= data->getSize())
        {
            // if we need more space, increase the block by at least 10%..
            storageNeeded += jmax (blockSize, storageNeeded / 10);
            storageNeeded = storageNeeded - (storageNeeded % blockSize) + blockSize;

            data->ensureSize (storageNeeded);
        }

        data->copyFrom (buffer, position, howMany);
        position += howMany;
        size = jmax (size, position);
    }

    return true;
}

const char* MemoryOutputStream::getData() throw()
{
    if (data->getSize() > size)
        ((char*) data->getData()) [size] = 0;

    return (const char*) data->getData();
}

int MemoryOutputStream::getDataSize() const throw()
{
    return size;
}

int64 MemoryOutputStream::getPosition()
{
    return position;
}

bool MemoryOutputStream::setPosition (int64 newPosition)
{
    if (newPosition <= size)
    {
        // ok to seek backwards
        position = jlimit (0, size, (int) newPosition);
        return true;
    }
    else
    {
        // trying to make it bigger isn't a good thing to do..
        return false;
    }
}

END_JUCE_NAMESPACE