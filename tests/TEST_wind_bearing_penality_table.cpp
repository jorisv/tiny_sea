// TinySea: sailing boat routing library
// Copyright (C) 2019 Joris Vaillant
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// includes
// GTest
#include <gtest/gtest.h>

// tiny_sea
#include <tiny_sea/core/boat_velocity_table.h>
#include <tiny_sea/core/numeric_constants.h>
#include <tiny_sea/core/wind_bearing_penalty_table.h>

using namespace tiny_sea;

/*! Test PenaltyTableBuilder
 * Use following wind bearing:
 *  -  PI/4
 *  - -PI/4
 *  -  PI/2
 *  - -PI/2
 *
 * The result PenaltyTable should look like this:
 *       PI/4   -PI/4   PI/2   -PI/2    null
 *  PI/4  0       2      1       3       0
 * -PI/4  2       0      3       1       0
 *  PI/2  1       3      0       2       0
 * -PI/2  3       1      2       0       0
 *  null  0       0      0       0       0
 */
TEST(PENALTY_TABLE_TESTS, TEST_builder)
{
    BoatVelocityTableBuilder velTableBuilder(
      makeLinearSpace(velocity_t(1), velocity_t(1.), 3));
    velTableBuilder.addSymetric(
      radian_t(PI / 4.), { velocity_t(0.5), velocity_t(1.), velocity_t(1.5) });
    velTableBuilder.addSymetric(
      radian_t(PI / 2.),
      { velocity_t(0.75), velocity_t(1.5), velocity_t(2.25) });

    PenaltyTableBuilder penaltyBuilder(velTableBuilder.build());
    // Build diag
    penaltyBuilder.safeSetSymetric(0, 0, tiny_sea::time_t(0));
    penaltyBuilder.safeSetSymetric(1, 1, tiny_sea::time_t(0));
    penaltyBuilder.safeSetSymetric(2, 2, tiny_sea::time_t(0));
    penaltyBuilder.safeSetSymetric(3, 3, tiny_sea::time_t(0));

    // Build first line
    penaltyBuilder.safeSetSymetric(1, 0, tiny_sea::time_t(2));
    penaltyBuilder.safeSetSymetric(2, 0, tiny_sea::time_t(1));
    penaltyBuilder.safeSetSymetric(3, 0, tiny_sea::time_t(3));

    // Build second line
    penaltyBuilder.safeSetSymetric(2, 1, tiny_sea::time_t(3));
    penaltyBuilder.safeSetSymetric(3, 1, tiny_sea::time_t(1));

    // Build third line
    penaltyBuilder.safeSetSymetric(3, 2, tiny_sea::time_t(2));

    // Test built table
    PenaltyTable penaltyTable = penaltyBuilder.build();
    // Test diagonal
    EXPECT_EQ(penaltyTable.safePenalty(0, 0), tiny_sea::time_t(0));
    EXPECT_EQ(penaltyTable.safePenalty(1, 1), tiny_sea::time_t(0));
    EXPECT_EQ(penaltyTable.safePenalty(2, 2), tiny_sea::time_t(0));
    EXPECT_EQ(penaltyTable.safePenalty(3, 3), tiny_sea::time_t(0));
    EXPECT_EQ(penaltyTable.safePenalty(4, 4), tiny_sea::time_t(0));

    // Test first line and column
    EXPECT_EQ(penaltyTable.safePenalty(1, 0), tiny_sea::time_t(2));
    EXPECT_EQ(penaltyTable.safePenalty(0, 1), tiny_sea::time_t(2));
    EXPECT_EQ(penaltyTable.safePenalty(2, 0), tiny_sea::time_t(1));
    EXPECT_EQ(penaltyTable.safePenalty(0, 2), tiny_sea::time_t(1));
    EXPECT_EQ(penaltyTable.safePenalty(3, 0), tiny_sea::time_t(3));
    EXPECT_EQ(penaltyTable.safePenalty(0, 3), tiny_sea::time_t(3));
    EXPECT_EQ(penaltyTable.safePenalty(4, 0), tiny_sea::time_t(0));
    EXPECT_EQ(penaltyTable.safePenalty(0, 4), tiny_sea::time_t(0));

    // Test second line and column
    EXPECT_EQ(penaltyTable.safePenalty(2, 1), tiny_sea::time_t(3));
    EXPECT_EQ(penaltyTable.safePenalty(1, 2), tiny_sea::time_t(3));
    EXPECT_EQ(penaltyTable.safePenalty(3, 1), tiny_sea::time_t(1));
    EXPECT_EQ(penaltyTable.safePenalty(1, 3), tiny_sea::time_t(1));
    EXPECT_EQ(penaltyTable.safePenalty(4, 1), tiny_sea::time_t(0));
    EXPECT_EQ(penaltyTable.safePenalty(1, 4), tiny_sea::time_t(0));

    // Test third line and column
    EXPECT_EQ(penaltyTable.safePenalty(3, 2), tiny_sea::time_t(2));
    EXPECT_EQ(penaltyTable.safePenalty(2, 3), tiny_sea::time_t(2));
    EXPECT_EQ(penaltyTable.safePenalty(4, 2), tiny_sea::time_t(0));
    EXPECT_EQ(penaltyTable.safePenalty(2, 4), tiny_sea::time_t(0));

    // Test last line
    EXPECT_EQ(penaltyTable.safePenalty(4, 3), tiny_sea::time_t(0));
    EXPECT_EQ(penaltyTable.safePenalty(3, 4), tiny_sea::time_t(0));
}

// TEST builder

// TEST fromwind
