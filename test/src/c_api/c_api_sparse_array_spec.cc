/**
 * Copyright (c) 2016  Massachusetts Institute of Technology and Intel Corp.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
 * OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * Tests of C API for read/write/update operations for
 * sparse arrays
 */

#include <gtest/gtest.h>
#include "c_api.h"
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <cstring>
#include <sstream>
#include <map>

class SparseArrayTestFixture: public testing::Test {
  const std::string WORKSPACE = ".__workspace/";
  const std::string ARRAY_100x100 = "sparse_test_100x100_10x10";
  const int ARRAY_RANK = 2;

public:
  // Array schema object under test
  TileDB_ArraySchema schema;
  // TileDB context
  TileDB_CTX* tiledb_ctx;
  // Array name is initialized with the workspace folder
  std::string arrayName;

  int **generate_2Dbuffer(
      const int, const int);

  int *generate_1Dbuffer(
        const int, const int);

  int create_sparse_array_2D(
      const long dim0_tile_extent,
      const long dim1_tile_extent,
      const long dim0_lo,
      const long dim0_hi,
      const long dim1_lo,
      const long dim1_hi,
      const int capacity,
	  const bool enable_compression);

  int write_sparse_array_2D(
      const int64_t dim0,
      const int64_t dim1,
      const int64_t chunkDim0,
      const int64_t chunkDim1);

  int write_sparse_array_sorted_2D(
    const int64_t dim0,
    const int64_t dim1,
    const int64_t chunkDim0,
    const int64_t chunkDim1);

  int update_sparse_array_2D(
      const int dim0,
      const int dim1,
      int length,
      int srand_key,
      int *buffer_a1,
      int64_t *buffer_coords,
      const void* buffers[],
      size_t buffer_sizes[2]);

  int * read_sparse_array_2D(
      const int64_t dim0_lo,
      const int64_t dim0_hi,
      const int64_t dim1_lo,
      const int64_t dim1_hi);

  virtual void SetUp() {
    // Initialize context with the default configuration parameters
    tiledb_ctx_init(&tiledb_ctx, NULL);
    if (tiledb_workspace_create(
        tiledb_ctx,
        WORKSPACE.c_str()) != TILEDB_OK) {
      exit(EXIT_FAILURE);
    }

    arrayName.append(WORKSPACE);
    arrayName.append(ARRAY_100x100);
  }

  virtual void TearDown() {
    // Finalize TileDB context
    tiledb_ctx_finalize(tiledb_ctx);

    // Remove the temporary workspace
    std::string command = "rm -rf ";
    command.append(WORKSPACE);
    int rc = system(command.c_str());
    ASSERT_EQ(rc, 0);
  }
};
