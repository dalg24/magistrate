/*
//@HEADER
// *****************************************************************************
//
//                     test_kokkos_serialize_special.cc
//                           DARMA Toolkit v. 1.0.0
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
#if KOKKOS_ENABLED_CHECKPOINT

#include "test_harness.h"
#include "test_commons.h"

struct KokkosViewContentsTest : virtual testing::Test { };


TEST_F(KokkosViewContentsTest, test_view_contents) {
  using ViewType = Kokkos::View<int*>;

  ViewType original = ViewType("my view", 2);
  ViewType alias = original;

  original(0) = 10;
  original(1) = 20;

  EXPECT_EQ(10, alias(0));
  EXPECT_EQ(20, alias(1));

  using namespace checkpoint;

  auto sizer = Sizer();
  serializeContentsOnly(sizer, original);

  auto packer = Packer(sizer.getSize());
  serializeContentsOnly(packer, original);

  Kokkos::deep_copy(alias, 0);

  auto buffer = packer.extractPackedBuffer();
  auto unpacker = Unpacker(buffer->getBuffer());

  serializeContentsOnly(unpacker, original);

  EXPECT_EQ(10, alias(0));
  EXPECT_EQ(20, alias(1));
}

struct KokkosViewExtentTest : virtual testing::Test { };


TEST_F(KokkosViewExtentTest, test_view_extent) {
  using ViewType = Kokkos::View<int*>;

  ViewType original = ViewType("v", 12345);

  using namespace checkpoint;

  auto sizer = Sizer();
  serializeExtentOnly(sizer, original, "v");

  EXPECT_LE(sizer.getSize(), 1000);

  auto packer = Packer(sizer.getSize());
  serializeExtentOnly(packer, original, "v");

  auto buffer = packer.extractPackedBuffer();
  auto unpacker = Unpacker(buffer->getBuffer());

  ViewType target = ViewType("w", 10);

  serializeExtentOnly(unpacker, target, "v");

  EXPECT_EQ(12345, target.extent(0));
}

TEST_F(KokkosViewExtentTest, test_view_extent_2d) {
  using ViewType = Kokkos::View<int**>;

  ViewType original = ViewType("v", 12345, 5);

  using namespace checkpoint;

  auto sizer = Sizer();
  serializeExtentOnly(sizer, original, "v");

  EXPECT_LE(sizer.getSize(), 1000);

  auto packer = Packer(sizer.getSize());
  serializeExtentOnly(packer, original, "v");

  auto buffer = packer.extractPackedBuffer();
  auto unpacker = Unpacker(buffer->getBuffer());

  ViewType target = ViewType("w", 10);

  serializeExtentOnly(unpacker, target, "v");

  EXPECT_EQ(12345, target.extent(0));
  EXPECT_EQ(5, target.extent(1));
}

TEST_F(KokkosViewExtentTest, test_view_extent_3d) {
  using ViewType = Kokkos::View<int***>;

  ViewType original = ViewType("v", 12345, 5, 4);

  using namespace checkpoint;

  auto sizer = Sizer();
  serializeExtentOnly(sizer, original, "v");

  EXPECT_LE(sizer.getSize(), 1000);

  auto packer = Packer(sizer.getSize());
  serializeExtentOnly(packer, original, "v");

  auto buffer = packer.extractPackedBuffer();
  auto unpacker = Unpacker(buffer->getBuffer());

  ViewType target = ViewType("w", 10);

  serializeExtentOnly(unpacker, target, "v");

  EXPECT_EQ(12345, target.extent(0));
  EXPECT_EQ(5, target.extent(1));
  EXPECT_EQ(4, target.extent(2));
}

TEST_F(KokkosViewExtentTest, test_view_extent_4d) {
  using ViewType = Kokkos::View<int****>;

  ViewType original = ViewType("v", 9, 23, 5, 4);

  using namespace checkpoint;

  auto sizer = Sizer();
  serializeExtentOnly(sizer, original, "v");

  EXPECT_LE(sizer.getSize(), 1000);

  auto packer = Packer(sizer.getSize());
  serializeExtentOnly(packer, original, "v");

  auto buffer = packer.extractPackedBuffer();
  auto unpacker = Unpacker(buffer->getBuffer());

  ViewType target = ViewType("w", 10);

  serializeExtentOnly(unpacker, target, "v");

  EXPECT_EQ(9, target.extent(0));
  EXPECT_EQ(23, target.extent(1));
  EXPECT_EQ(5, target.extent(2));
  EXPECT_EQ(4, target.extent(3));
}

#endif
