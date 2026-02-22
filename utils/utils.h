#ifndef _UTILS_H_
#define _UTILS_H_

#include "platform_types.h"

#define ALIGN_UP(value, alignment) (((value) + ((alignment) - 1)) & ~((alignment) - 1))
#define ALIGN_DOWN(value, alignment) ((value) & ~((alignment) - 1))

UINT8 API MemoryCompare(CONST IN VOID *LhsBuffer, CONST IN VOID *RhsBuffer, CONST IN UINT64 BufferLength);

VOID API MemoryCopy(CONST IN VOID *Source, OUT VOID *Destination, CONST IN UINT64 Size);

VOID API MemorySet(IN OUT VOID *Buffer, IN CONST UINT8 Value, IN CONST UINT64 Size);

VOID API Char8ToChar16(CONST IN CHAR8 *Source, OUT CHAR16 *Destination, IN CONST UINT64 Length);

VOID API StringReverse(IN OUT CHAR16 *String, IN CONST UINT64 StringLength);

VOID API SplitPath(IN CONST CHAR16 *Path, OUT CHAR16 *DirectoryName, OUT CHAR16 *RestOfThePath);

VOID API StringCopy(IN CONST CHAR16* Source, OUT CHAR16* Destination);

static inline UINT8 API CountTrailingZeros(UINT64 Value)
{
    UINT8 NumberOfTrailingZeros = 0;

    if (Value == 0)
    {
        return 64;
    }
    
    while ((Value & 0x1ULL) == 0ULL)
    {
        NumberOfTrailingZeros ++;
        Value = (Value >> 1U);
    }

    return NumberOfTrailingZeros;
}

static inline UINT8 API FloorLog2(UINT64 Value)
{
    UINT8 PowerOfTwo = 0;

    if (Value == 0ULL)
    {
        return 0xFF;
    }

    while (Value > 1ULL)
    {
        PowerOfTwo ++;
        Value = (Value >> 1U);
    }

    return PowerOfTwo;
}

static inline UINT8 API CeilLog2(UINT64 Value)
{
    if (0 == Value)
    {
        return 0xFF;
    }

    if (1 == Value)
    {
        return 0;
    }

    return FloorLog2(Value - 1) + 1;
}

#endif /* _UTILS_H_ */
