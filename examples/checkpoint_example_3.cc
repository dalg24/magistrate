/*
//@HEADER
// *****************************************************************************
//
//                           checkpoint_example_3.cc
//                 DARMA/checkpoint => Serialization Library
//
// Copyright 2019 National Technology & Engineering Solutions of Sandia, LLC
// (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact darma@sandia.gov
//
// *****************************************************************************
//@HEADER
*/

/// [Serialize constraints constructor destructor]

#include <checkpoint/checkpoint.h>

#include <cstdio>

//
// This example illustrates how checkpoint uses traits to determine a
// reconstruction strategy and shows several mechanisms for reconstructing a
// class of a serializable/deserializable type.
//

namespace magistrate::intrusive::examples {

// \brief Structure with a variable of built-in type.
//
// \note This structure is a serializable / deserializable type. It has a
// default constructor and a `serialize` function.
struct TestDefaultCons {
  int a = 29;

  TestDefaultCons() = default;

  template <typename Serializer>
  void serialize(Serializer& s) {
    s | a;
  }
};

// \brief Structure with a variable of built-in type.
//
// \note This structure is a byte-serializable/deserializable type but doesn't
// contain any traits to indicate that making it not serializable.
//
struct TestNoSerialize {
  int a = 29;
};

// \brief Structure with a variable of built-in type.
//
// \note This structure is not a serializable / deserializable type. The
// structure has an explicitly deleted default constructor and a constructor
// that takes an `int` parameter.  Serialization has no way to construct the
// structure.
struct TestShouldFailReconstruct {
  int a = 29;

  TestShouldFailReconstruct(int const) { }
  TestShouldFailReconstruct() = delete;

  template <typename Serializer>
  void serialize(Serializer& s) {
    s | a;
  }
};

// \brief Structure with a variable of built-in type.
//
// \note This structure is a serializable / deserializable type. The structure
// has an explicitly deleted default constructor and a constructor that takes an
// `int` parameter. However, the structure provides a static reconstruct
// method that serialization can use to construct the structure.
struct TestReconstruct {
  int a = 29;

  TestReconstruct(int const) { }
  TestReconstruct() = delete;

  static TestReconstruct& reconstruct(void* buf) {
    auto a = new (buf) TestReconstruct(100);
    return *a;
  }

  template <typename Serializer>
  void serialize(Serializer& s) {
    s | a;
  }
};

} // end namespace magistrate::intrusive::examples

#include "checkpoint/traits/serializable_traits.h"

namespace checkpoint {

using namespace magistrate::intrusive::examples;

static_assert(
  SerializableTraits<TestDefaultCons>::is_serializable,
  "Should be serializable"
);
static_assert(
  ! SerializableTraits<TestNoSerialize>::is_serializable,
  "Should not be serializable"
);

static_assert(
  ! SerializableTraits<TestShouldFailReconstruct>::is_serializable,
  "Should not be serializable"
);

static_assert(
  SerializableTraits<TestReconstruct>::is_serializable,
  "Should be serializable"
);
} // end namespace checkpoint


int main(int, char**) {
  // Example is a compile-time test of serializability traits
  return 0;
}

/// [Serialize constraints constructor destructor]
