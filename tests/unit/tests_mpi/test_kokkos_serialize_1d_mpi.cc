#if KOKKOS_ENABLED_SERDES

#include "test_harness.h"
#include "test_commons.h"

#include "test_kokkos_1d_commons.h"

#include "tests_mpi/mpi-init.h"

#include <mpich-clang39/mpi.h>
// Manual 1,2,3 dimension comparison

template <typename ParamT> struct KokkosViewTest1DMPI : KokkosViewTest<ParamT> { };

TYPED_TEST_CASE_P(KokkosViewTest1DMPI);

TYPED_TEST_P(KokkosViewTest1DMPI, test_1d_any) {
  using namespace serialization::interface;

  using LayoutType = typename std::tuple_element<0,TypeParam>::type;
  using DataType   = typename std::tuple_element<1,TypeParam>::type;
  using ViewType   = Kokkos::View<DataType, LayoutType>;

  static constexpr size_t const N = 241;

  LayoutType layout = layout1d<LayoutType>(N);
  ViewType in_view("test", layout);

  init1d(in_view);

  // Test the respect of the max rank needed for the test'
  EXPECT_EQ(MPIEnvironment::isRankValid(1), true);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (world_rank == 0) {
    std::cout << " RANK 0 ==== > Do the Serialization " << std::endl;
    auto ret = serialize<ViewType>(in_view);
    int viewSize = ret->getSize();
    MPI_Send( &viewSize, 1, MPI_INT, 1, 0, MPI_COMM_WORLD );
    char * viewBuffer = ret->getBuffer();
    MPI_Send(viewBuffer, viewSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
  }
  else  {
    // MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
    std::cout << " RANK "<< world_rank << " ==== > Do the Deserialization " << std::endl;
    int viewSize;
    MPI_Recv( & viewSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    char * recv = (char *) malloc(viewSize);

    MPI_Recv(recv, viewSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    auto out_view = deserialize<ViewType>(recv, viewSize);
    auto const& out_view_ref = *out_view;
#if SERDES_USE_ND_COMPARE
  compareND(in_view, out_view_ref);
#else
  compare1d(in_view, out_view_ref);
#endif
  }

}


REGISTER_TYPED_TEST_CASE_P(KokkosViewTest1DMPI, test_1d_any);

#if DO_UNIT_TESTS_FOR_VIEW

///////////////////////////////////////////////////////////////////////////////
// 1-D Kokkos::View Tests
///////////////////////////////////////////////////////////////////////////////

using Test1DTypes = std::tuple<
  int      *, int      [1], int      [4],
  double   *, double   [1], double   [4],
  float    *, float    [1], float    [4],
  int32_t  *, int32_t  [1], int32_t  [4],
  int64_t  *, int64_t  [1], int64_t  [4],
  unsigned *, unsigned [1], unsigned [4],
  long     *, long     [1], long     [4],
  long long*, long long[1], long long[4]
>;

using Test1DTypesLeft =
  typename TestFactory<Test1DTypes,Kokkos::LayoutLeft>::ResultType;
using Test1DTypesRight =
  typename TestFactory<Test1DTypes,Kokkos::LayoutRight>::ResultType;
using Test1DTypesStride =
  typename TestFactory<Test1DTypes,Kokkos::LayoutStride>::ResultType;

INSTANTIATE_TYPED_TEST_CASE_P(test_1d_L, KokkosViewTest1DMPI, Test1DTypesLeft);
INSTANTIATE_TYPED_TEST_CASE_P(test_1d_R, KokkosViewTest1DMPI, Test1DTypesRight);
INSTANTIATE_TYPED_TEST_CASE_P(test_1d_S, KokkosViewTest1DMPI, Test1DTypesStride);

#endif

///////////////////////////////////////////////////////////////////////////////
// Kokkos::DynamicView Unit Tests: dynamic view is restricted to 1-D in kokkos
///////////////////////////////////////////////////////////////////////////////

template <typename ParamT>
struct KokkosDynamicViewTestMPI : KokkosViewTest<ParamT> { };

TYPED_TEST_CASE_P(KokkosDynamicViewTestMPI);

TYPED_TEST_P(KokkosDynamicViewTestMPI, test_dynamic_1d) {
  using namespace serialization::interface;

  using DataType = TypeParam;
  using ViewType = Kokkos::Experimental::DynamicView<DataType>;

  static constexpr std::size_t const N = 64;
  static constexpr unsigned const min_chunk = 8;
  static constexpr unsigned const max_extent = 1024;

  ViewType in_view("my-dynamic-view", min_chunk, max_extent);
  in_view.resize_serial(N);

  // std::cout << "INIT size=" << in_view.size() << std::endl;

  init1d(in_view);

  auto ret = serialize<ViewType>(in_view);
  auto out_view = deserialize<ViewType>(ret->getBuffer(), ret->getSize());
  auto const& out_view_ref = *out_view;

  /*
   *  Uncomment these lines (one or both) to test the failure mode: ensure the
   *  view equality test code is operating correctly.
   *
   *   out_view_ref(3) = 10;
   *   out_view->resize_serial(N-1);
   *
   */

#if SERDES_USE_ND_COMPARE
  compareND(in_view, out_view_ref);
#else
  compare1d(in_view, out_view_ref);
#endif
}

REGISTER_TYPED_TEST_CASE_P(KokkosDynamicViewTestMPI, test_dynamic_1d);

using DynamicTestTypes = testing::Types<
  int      *,
  double   *,
  float    *,
  int32_t  *,
  int64_t  *,
  unsigned *,
  long     *,
  long long*
>;

INSTANTIATE_TYPED_TEST_CASE_P(
  test_dynamic_view_1, KokkosDynamicViewTestMPI, DynamicTestTypes
);



#endif