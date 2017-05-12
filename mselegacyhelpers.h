
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSELEGACYHELPERS_H_
#define MSELEGACYHELPERS_H_

#include "msepoly.h"
#include <cstring>

#ifdef MSE_SAFER_SUBSTITUTES_DISABLED
#define MSE_LEGACYHELPERS_DISABLED
#endif /*MSE_SAFER_SUBSTITUTES_DISABLED*/

namespace mse {
	namespace lh {

#ifdef MSE_LEGACYHELPERS_DISABLED

#define MSE_LH_FIXED_ARRAY_DECLARATION(element_type, size, name) element_type name[size]
#define MSE_LH_ITERATOR_FROM_ARRAY(array) (&(array[0]))

#define MSE_LH_DYNAMIC_ARRAY_TYPE(element_type) element_type *
#define MSE_LH_ALLOC_DYNAMIC_ARRAY(element_type, dyn_array, num_bytes) dyn_array = (element_type *)malloc(num_bytes)
#define MSE_LH_REALLOC_DYNAMIC_ARRAY(element_type, dyn_array, num_bytes) dyn_array = (element_type *)realloc(dyn_array, num_bytes)
#define MSE_LH_FREE_DYNAMIC_ARRAY(dyn_array) free(dyn_array)

#define MSE_LH_FREAD(ptr, size, count, stream) fread(ptr, size, count, stream)
#define MSE_LH_FWRITE(ptr, size, count, stream) fwrite(ptr, size, count, stream)

#define MSE_LH_TYPED_MEMCPY(element_type, destination, source, num_bytes) memcpy(destination, source, num_bytes)
#define MSE_LH_TYPED_MEMSET(element_type, ptr, value, num_bytes) memset(ptr, value, num_bytes)
#define MSE_LH_MEMCPY(destination, source, num_bytes) memcpy(destination, source, num_bytes)
#define MSE_LH_MEMSET(ptr, value, num_bytes) memset(ptr, value, num_bytes)

#else /*MSE_LEGACYHELPERS_DISABLED*/

#define MSE_LH_FIXED_ARRAY_DECLARATION(element_type, size, name) mse::lh::TNativeArrayReplacement< element_type, size > name
#define MSE_LH_ITERATOR_FROM_ARRAY(array) array

#define MSE_LH_DYNAMIC_ARRAY_TYPE(element_type) mse::lh::TIPointerWithBundledVector< element_type >
#define MSE_LH_ALLOC_DYNAMIC_ARRAY(element_type, dyn_array, num_bytes) mse::lh::CAllocF< decltype(dyn_array) >::allocate(dyn_array, num_bytes)
#define MSE_LH_REALLOC_DYNAMIC_ARRAY(element_type, dyn_array, num_bytes) mse::lh::CAllocF< decltype(dyn_array) >::reallocate(dyn_array, num_bytes)
#define MSE_LH_FREE_DYNAMIC_ARRAY(dyn_array) mse::lh::CAllocF< decltype(dyn_array) >::free(dyn_array)

#define MSE_LH_FREAD(ptr, size, count, stream) mse::lh::CFileF< decltype(ptr) >::fread(ptr, size, count, stream)
#define MSE_LH_FWRITE(ptr, size, count, stream) mse::lh::CFileF< decltype(ptr) >::fwrite(ptr, size, count, stream)

#define MSE_LH_TYPED_MEMCPY(element_type, destination, source, num_bytes) mse::lh::CMemF< mse::TNullableAnyRandomAccessIterator<element_type> >::memcpy(destination, source, num_bytes)
#define MSE_LH_TYPED_MEMSET(element_type, ptr, value, num_bytes) mse::lh::CMemF< mse::TNullableAnyRandomAccessIterator<element_type> >::memset(ptr, value, num_bytes)
#define MSE_LH_MEMCPY(destination, source, num_bytes) mse::lh::CMemF< decltype(destination) >::memcpy(destination, source, num_bytes)
#define MSE_LH_MEMSET(ptr, value, num_bytes) mse::lh::CMemF< decltype(ptr) >::memset(ptr, value, num_bytes)

#endif /*MSE_LEGACYHELPERS_DISABLED*/

		template <typename _Ty>
		class TOpaqueWrapper {
		public:
			TOpaqueWrapper(const _Ty& value_param) : m_value(value_param) {}
			_Ty& value() { return m_value; }
			const _Ty& value() const { return m_value; }

			_Ty m_value;
		};
		template <typename _Ty> using TVectorRefcfptrWrapper = TOpaqueWrapper<mse::TRefCountingFixedPointer<mse::msevector<_Ty>>>;

		/* This data type was motivated by the need for a direct substitute for native pointers targeting dynamically
		allocated (native) arrays, which can kind of play a dual role as a reference to the array object and/or as an
		iterator. I'm not sure about this implementation though. We could base it on an mse::ivector::ipointer
		instead... */
		template <typename _Ty>
		class TIPointerWithBundledVector : private TVectorRefcfptrWrapper<_Ty>, public mse::msevector<_Ty>::ipointer {
		public:
			typedef typename mse::msevector<_Ty>::ipointer ipointer_base_class;
			typedef typename mse::msevector<_Ty>::size_type size_type;

			TIPointerWithBundledVector() : TVectorRefcfptrWrapper<_Ty>(mse::make_refcounting<mse::msevector<_Ty>>())
				, ipointer_base_class(*vector_refcptr()) {}
			TIPointerWithBundledVector(const TIPointerWithBundledVector& src) : TVectorRefcfptrWrapper<_Ty>(src.vector_refcptr())
				, ipointer_base_class(*vector_refcptr()) {
				ipointer_base_class::operator=(src);
			}
			TIPointerWithBundledVector(_XSTD initializer_list<_Ty> _Ilist) : TVectorRefcfptrWrapper<_Ty>(mse::make_refcounting<mse::msevector<_Ty>>(_Ilist))
				, ipointer_base_class(*vector_refcptr()) {}
			explicit TIPointerWithBundledVector(size_type _N) : TVectorRefcfptrWrapper<_Ty>(mse::make_refcounting<mse::msevector<_Ty>>(_N))
				, ipointer_base_class(*vector_refcptr()) {}
			explicit TIPointerWithBundledVector(size_type _N, const _Ty& _V) : TVectorRefcfptrWrapper<_Ty>(mse::make_refcounting<mse::msevector<_Ty>>(_N, _V))
				, ipointer_base_class(*vector_refcptr()) {}
			/*
			template <class... Args>
			TIPointerWithBundledVector(Args&&... args) : TVectorRefcfptrWrapper<_Ty>(mse::make_refcounting<mse::msevector<_Ty>>(std::forward<Args>(args)...))
				, ipointer_base_class(*vector_refcptr()) {}
				*/

			size_type size() const {
				return (*vector_refcptr()).size();
			}
			void resize(size_type _N, const _Ty& _X = _Ty()) {
				auto old_size = size();

				(*vector_refcptr()).resize(_N, _X);
				(*vector_refcptr()).shrink_to_fit();

				if (true || (0 == old_size)) {
					(*this).set_to_beginning();
				}
			}

			TIPointerWithBundledVector& operator=(const TIPointerWithBundledVector& _Right_cref) {
				if (_Right_cref.vector_refcptr() == vector_refcptr()) {
					ipointer_base_class::operator=(_Right_cref);
				}
				else {
					~TIPointerWithBundledVector();
					::new (this) TIPointerWithBundledVector(_Right_cref);
				}
				return(*this);
			}

			explicit operator bool() const {
				return ((*this).size() != 0);
			}

			template <class... Args>
			static TIPointerWithBundledVector make(Args&&... args) {
				return TIPointerWithBundledVector(std::forward<Args>(args)...);
			}

		private:
			mse::TRefCountingFixedPointer<mse::msevector<_Ty>>& vector_refcptr() { return (*this).value(); }
			const mse::TRefCountingFixedPointer<mse::msevector<_Ty>>& vector_refcptr() const { return (*this).value(); }
			//mse::TRefCountingFixedPointer<mse::msevector<_Ty>> m_vector_refcptr;
		};

		template <class X, class... Args>
		TIPointerWithBundledVector<X> make_ipointer_with_bundled_vector(Args&&... args) {
			return TIPointerWithBundledVector<X>::make(std::forward<Args>(args)...);
		}

		template <typename _Ty, size_t _Size>
		class TNativeArrayReplacement : public mse::mstd::array<_Ty, _Size> {
		public:
			typedef mse::mstd::array<_Ty, _Size> base_class;
			using base_class::base_class;

			operator mse::TNullableAnyRandomAccessIterator<_Ty>() {
				return base_class::begin();
			}
			operator mse::TAnyRandomAccessIterator<_Ty>() {
				return base_class::begin();
			}
			operator mse::TAnyRandomAccessConstIterator<_Ty>() const {
				return base_class::cbegin();
			}
			operator typename mse::mstd::array<_Ty, _Size>::iterator() {
				return base_class::begin();
			}
			operator typename mse::mstd::array<_Ty, _Size>::const_iterator() {
				return base_class::cbegin();
			}
		};

		template<class _Ty>
		class CAllocF {
		public:
			static void free(_Ty& ptr);
			static void allocate(_Ty& ptr, size_t num_bytes);
			static void reallocate(_Ty& ptr, size_t num_bytes);
		};
		template<class _Ty>
		class CAllocF<_Ty*> {
		public:
			static void free(_Ty* ptr) {
				::free(ptr);
			}
			static void allocate(_Ty*& ptr, size_t num_bytes) {
				ptr = ::malloc(num_bytes);
			}
			static void reallocate(_Ty*& ptr, size_t num_bytes) {
				ptr = ::realloc(ptr, num_bytes);
			}
		};
		template<class _Ty>
		class CAllocF<mse::lh::TIPointerWithBundledVector<_Ty>> {
		public:
			static void free(mse::lh::TIPointerWithBundledVector<_Ty>& ptr) {
				ptr.resize(0);
			}
			static void allocate(mse::lh::TIPointerWithBundledVector<_Ty>& ptr, size_t num_bytes) {
				ptr.resize(num_bytes / sizeof(_Ty));
			}
			static void reallocate(mse::lh::TIPointerWithBundledVector<_Ty>& ptr, size_t num_bytes) {
				ptr.resize(num_bytes / sizeof(_Ty));
			}
		};

		template<class _Ty>
		class CFileF {
		public:
			static size_t fread(_Ty ptr, size_t size, size_t count, FILE * stream);
			static size_t fwrite(_Ty ptr, size_t size, size_t count, FILE * stream);
		};
		template<class _Ty>
		class CFileF<_Ty*> {
		public:
			static size_t fread(_Ty* ptr, size_t size, size_t count, FILE * stream) {
				return ::fread(ptr, size, count, stream);
			}
			static size_t fwrite(_Ty ptr, size_t size, size_t count, FILE * stream) {
				return ::fwrite(ptr, size, count, stream);
			}
		};
		template<class _Ty>
		class CFileF<mse::TNullableAnyRandomAccessIterator<_Ty>> {
		public:
			static size_t fread(mse::TNullableAnyRandomAccessIterator<_Ty> ptr, size_t size, size_t count, FILE * stream) {
				static std::vector<unsigned char> v;
				v.resize(size * count);
				auto num_bytes_read = ::fread(v.data(), size, count, stream);
				auto num_items_read = num_bytes_read / sizeof(_Ty);
				size_t uc_index = 0;
				size_t Ty_index = 0;
				for (; Ty_index < num_items_read; uc_index += sizeof(_Ty), Ty_index += 1) {
					unsigned char* uc_ptr = &(v[uc_index]);
					_Ty* Ty_ptr = (_Ty*)uc_ptr;
					ptr[Ty_index] = (*Ty_ptr);
				}
				v.resize(0);
				return num_bytes_read;
			}
			static size_t fwrite(mse::TNullableAnyRandomAccessIterator<_Ty> ptr, size_t size, size_t count, FILE * stream) {
				static std::vector<unsigned char> v;
				v.resize(size * count);
				auto num_items_to_write = size * count / sizeof(_Ty);
				size_t uc_index = 0;
				size_t Ty_index = 0;
				for (; Ty_index < num_items_to_write; uc_index += sizeof(_Ty), Ty_index += 1) {
					unsigned char* uc_ptr = &(v[uc_index]);
					_Ty* Ty_ptr = (_Ty*)uc_ptr;
					(*Ty_ptr) = ptr[Ty_index];
				}
				auto res = ::fwrite(v.data(), size, count, stream);
				v.resize(0);
				return res;
			}
		};

		template<class _Ty>
		class CMemF {
		public:
			static void memcpy(_Ty destination, _Ty source, size_t num);
			static void memset(_Ty ptr, int value, size_t num);
		};
		template<class _Ty>
		class CMemF<_Ty*> {
		public:
			static void memcpy(_Ty* destination, _Ty* source, size_t num) {
				::memcpy(destination, source, num);
			}
			static void memset(_Ty ptr, int value, size_t num) {
				::memset(ptr, value, num);
			}
		};
		template<class _Ty>
		class CMemF<mse::TNullableAnyRandomAccessIterator<_Ty>> {
		public:
			static void memcpy(mse::TNullableAnyRandomAccessIterator<_Ty> destination, mse::TNullableAnyRandomAccessIterator<_Ty> source, size_t num_bytes) {
				auto num_items = num_bytes / sizeof(_Ty);
				for (size_t i = 0; i < num_items; i += 1) {
					destination[i] = source[i];
				}
			}
			static void memset(mse::TNullableAnyRandomAccessIterator<_Ty> ptr, int value, size_t num_bytes) {
				auto Ty_value = _Ty(value);
				auto num_items = num_bytes / sizeof(_Ty);
				for (size_t i = 0; i < num_items; i += 1) {
					ptr[i] = Ty_value;
				}
			}
		};
		template<class _Ty>
		class CMemF<mse::lh::TIPointerWithBundledVector<_Ty>> {
		public:
			static void memcpy(mse::TNullableAnyRandomAccessIterator<_Ty> destination, mse::TNullableAnyRandomAccessIterator<_Ty> source, size_t num_bytes) {
				CMemF< mse::TNullableAnyRandomAccessIterator<_Ty> >::memcpy(destination, source, num_bytes);
			}
			static void memset(mse::TNullableAnyRandomAccessIterator<_Ty> ptr, int value, size_t num_bytes) {
				CMemF< mse::TNullableAnyRandomAccessIterator<_Ty> >::memset(ptr, value, num_bytes);
			}
		};

	}
}

#endif // MSELEGACYHELPERS_H_