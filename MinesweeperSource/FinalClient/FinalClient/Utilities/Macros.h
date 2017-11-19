// Macros.h
#ifndef MACROS_H
#define MACROS_H

#define SAFE_DELETE(_ptr_) delete _ptr_; _ptr_ = nullptr
#define SAFE_DELETE_ARRAY(_ptr_) delete[] _ptr_; _ptr_ = nullptr

#endif
