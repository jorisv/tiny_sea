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

// std
#include <memory>

// tiny_sea
#include <tiny_sea/gsp/state_factory.h>

using namespace tiny_sea;
using namespace tiny_sea::gsp;

class StateFactoryFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_factory.reset(new StateFactory(std::chrono::minutes(30),
                                         meter_t(10.),
                                         meter_t(1000.),
                                         NVector(1., 0., 0.),
                                         velocity_t(2.)));
    }

    std::unique_ptr<StateFactory> m_factory;
};

TEST_F(StateFactoryFixture, TEST_build1)
{
    // 447.214 894.427 once scaled
    NVector pos(Eigen::Vector3d(1., 2., 0.).normalized());
    std::chrono::seconds time(std::chrono::minutes(29));

    auto res = m_factory->build(pos, time);
    EXPECT_EQ(res.position(), pos);
    EXPECT_EQ(res.seconds(), time);
    EXPECT_EQ(res.discretState(), DiscretState(0, 44, 89, 0));
}

TEST_F(StateFactoryFixture, TEST_build2)
{
    // -182.33   911.65 -368.307 once scaled
    NVector pos(Eigen::Vector3d(-50., 250., -101.).normalized());
    std::chrono::seconds time(std::chrono::minutes(45));
    DiscretState parentState(2, 10, 22, -20);

    auto res = m_factory->build(pos, time, parentState);
    EXPECT_EQ(res.position(), pos);
    EXPECT_EQ(res.seconds(), time);
    EXPECT_EQ(res.discretState(), DiscretState(1, -19, 91, -37));
    EXPECT_EQ(res.g(), cost_t(fromChrono(time).t));
    EXPECT_NEAR(
      res.h().t, cost_t(pos.distance(NVector(1., 0., 0.)).t / 2.).t, 1e-8);
    EXPECT_EQ(res.f(), res.g() + res.h());
    EXPECT_EQ(res.parentState(), parentState);
}
