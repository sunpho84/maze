#ifndef _ASM_BOOKMARK_HPP
#define _ASM_BOOKMARK_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file asmBookmark.hpp
///
/// \brief Provides an entry in the assembly to trace a location in the source

#include <base/preprocessor.hpp>

namespace maze
{
#ifdef COMPILING_FOR_DEVICE
 
  /// Symbol to be used to begin an assembler comment, different in nvcc
# define _ASM_BOOKMARK_SYMBOL "//"
 
#else
 
# define _ASM_BOOKMARK_SYMBOL "#"
 
#endif
  
/// Include a comment in the assembler, recognizable in the compilation
#define ASM_BOOKMARK(COMMENT)					\
  asm(_ASM_BOOKMARK_SYMBOL "Bookmark file: \"" __FILE__ "\", line: " LINE_AS_STRING  ", " COMMENT)
  
  /// Put a BEGIN for asm bookmark section
#define ASM_BOOKMARK_BEGIN(COMMENT)					\
  ASM_BOOKMARK("BEGIN " COMMENT)
  
  /// Put an END for asm bookmark section
#define ASM_BOOKMARK_END(COMMENT)					\
  ASM_BOOKMARK("END " COMMENT)
  
/////////////////////////////////////////////////////////////////
  
/// Defines an inlined function BOOKMARK_BEGIN/END_NAME(Args...)
///
/// Internal implementation
#define PROVIDE_ASM_DEBUG_HANDLE_BEGIN_OR_END(BE,NAME,ARGS...)		\
  /*! Put in the assembly a bookmark named composing name and the arguments */ \
  INLINE_FUNCTION CUDA_HOST_DEVICE						\
  void BOOKMARK_ ## BE ## _ ## NAME (ARGS)				\
  {									\
    ASM_BOOKMARK_ ## BE(#NAME #ARGS);					\
  }
  
  /// Defines an inlined function BOOKMARK_BEGIN/END_NAME(Args...)
  ///
  /// The arguments can be used to distinguish different template
  /// instances (e.g. double from float)
#define PROVIDE_ASM_DEBUG_HANDLE(NAME,ARGS...)			\
  PROVIDE_ASM_DEBUG_HANDLE_BEGIN_OR_END(BEGIN,NAME,ARGS)	\
  PROVIDE_ASM_DEBUG_HANDLE_BEGIN_OR_END(END,NAME,ARGS)
}

#endif
