#pragma once

/** `assert` that prints a message on failure. */

#include <cassert>
#include <iostream>

#ifndef NDEBUG
#define assertm(predicate, msg)                                                \
  do {                                                                         \
    if (!(predicate)) {                                                        \
      std::cerr << __FILE__ << ":" << __LINE__ << ": `" << #predicate          \
                << "` failed. " << msg << std::endl;                           \
      std::terminate();                                                        \
    }                                                                          \
  } while (false)
#else
#define assertm(predicate, msg)                                                \
  do {                                                                         \
  } while (false)
#endif
