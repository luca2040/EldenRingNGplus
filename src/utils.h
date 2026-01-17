#pragma once

#include <cerrno>
#include <cstdlib>

bool strToUInt(const char *input, unsigned int &outValue)
{
  if (!input || *input == '\0')
    return false;

  errno = 0;
  char *endPtr = nullptr;
  unsigned long val = std::strtoul(input, &endPtr, 10);

  if (errno == ERANGE || val > UINT_MAX)
    return false;

  if (*endPtr != '\0')
    return false;

  outValue = static_cast<unsigned int>(val);
  return true;
}