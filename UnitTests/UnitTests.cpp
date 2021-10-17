#include "pch.h"
#include "CppUnitTest.h"

#define private public
#include "../DistanceMatrix/DistanceMatrix.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(UnitTests)
	{
	public:
		TEST_METHOD(CleanInstantiation)
		{
			DistanceMatrix dm(3);
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; i < 3; i++)
				{
					if (i == j)
					{
						Assert::AreEqual(0.0f, dm.get_defined_distance(i, j));
						Assert::AreEqual(0.0f, dm.get_calculated_distance(i, j));
					}
					else
					{
						Assert::AreEqual(std::numeric_limits<float>::infinity(), dm.get_defined_distance(i, j));
						Assert::AreEqual(std::numeric_limits<float>::infinity(), dm.get_calculated_distance(i, j));
					}
				}
			}
		}
		TEST_METHOD(Index)
		{
			DistanceMatrix dm(5);
			Assert::AreEqual(6, dm.p_get_index(1, 4));
			Assert::AreEqual(9, dm.p_get_index(4, 3));
		}
		TEST_METHOD(SingleDistance)
		{
			DistanceMatrix dm(5);
			dm.set_distance(2, 3, 4.0f);
			Assert::AreEqual(4.0f, dm.get_calculated_distance(2, 3));
			Assert::AreEqual(4.0f, dm.get_defined_distance(2, 3));
		}
		TEST_METHOD(ChainDistance)
		{
			DistanceMatrix dm(5);
			dm.set_distance(1, 2, 1.0f);
			dm.set_distance(2, 3, 1.0f);
			Assert::AreEqual(2.0f, dm.get_calculated_distance(1, 3));
		}
		TEST_METHOD(FourLengthChain)
		{
			DistanceMatrix dm(4);
			dm.set_distance(0, 1, 1.0f);
			dm.set_distance(2, 3, 1.0f);
			dm.set_distance(1, 2, 1.0f);
			Assert::AreEqual(3.0f, dm.get_calculated_distance(0, 3));
		}
		TEST_METHOD(CloserDistance)
		{
			DistanceMatrix dm(3);
			dm.set_distance(0, 2, 10.0f);
			dm.set_distance(0, 1, 3.0f);
			dm.set_distance(1, 2, 4.0f);
			Assert::AreEqual(7.0f, dm.get_calculated_distance(0, 2));
		}
		TEST_METHOD(DecreasingDistance)
		{
			DistanceMatrix dm(5);
			dm.set_distance(0, 1, 3.0f);
			dm.set_distance(0, 2, 2.0f);
			dm.set_distance(1, 3, 3.0f);
			dm.set_distance(2, 3, 3.0f);
			dm.set_distance(3, 4, 3.0f);
			Assert::AreEqual(8.0f, dm.get_calculated_distance(0, 4));
			dm.set_distance(1, 3, 1.0f);
			Assert::AreEqual(7.0f, dm.get_calculated_distance(0, 4));
		}
		TEST_METHOD(IncreasingDistance) {
			DistanceMatrix dm(5);
			dm.set_distance(0, 1, 3.0f);
			dm.set_distance(0, 2, 2.0f);
			dm.set_distance(2, 3, 3.0f);
			dm.set_distance(1, 3, 1.0f);
			dm.set_distance(3, 4, 3.0f);
			Assert::AreEqual(7.0f, dm.get_calculated_distance(0, 4));
			dm.set_distance(1, 3, 3.0f);
			Assert::AreEqual(8.0f, dm.get_calculated_distance(0, 4));
		}
		TEST_METHOD(InvalidIndex)
		{
			auto func = []() {
				DistanceMatrix dm(3);
				dm.set_distance(-1, 2, 1.0f);
			};
			Assert::ExpectException<std::out_of_range>(func);
		}
		TEST_METHOD(IncreaseSize)
		{
			DistanceMatrix dm(3);
			dm.set_distance(0, 1, 2.0f);
			dm.set_distance(1, 2, 1.0f);
			dm.resize(4);
			Assert::AreEqual(3.0f, dm.get_calculated_distance(0, 2));
			Assert::AreEqual(4, (int)dm.p_adjacency_list.size());
		}
		TEST_METHOD(DecreaseSize)
		{
			DistanceMatrix dm(4);
			dm.set_distance(0, 1, 2.0f);
			dm.set_distance(1, 2, 1.0f);
			dm.resize(3);
			Assert::AreEqual(3.0f, dm.get_calculated_distance(0, 2));
			Assert::AreEqual(3, (int)dm.p_adjacency_list.size());
		}
		TEST_METHOD(FindPath)
		{
			DistanceMatrix dm(6);
			dm.set_distance(0, 1, 2.0f);
			dm.set_distance(0, 3, 1.0f);
			dm.set_distance(1, 2, 2.0f);
			dm.set_distance(1, 3, 1.0f);
			dm.set_distance(2, 3, 3.0f);
			dm.set_distance(2, 4, 3.0f);
			dm.set_distance(2, 5, 3.0f);
			dm.set_distance(3, 4, 2.0f);
			dm.set_distance(4, 5, 1.0f);
			std::vector<int> expected{ 0, 3, 4, 5 };
			std::vector<int> actual = dm.get_shortest_path(0, 5);
			for (int i = 0; i < actual.size(); i++) {
				Assert::AreEqual(expected[i], actual[i]);
			}
		}
		TEST_METHOD(PathNotFound)
		{
			DistanceMatrix dm(4);
			dm.set_distance(0, 1, 1.0f);
			dm.set_distance(2, 3, 1.0f);
			Assert::AreEqual(0, (int)dm.get_shortest_path(0, 3).size());
		}
	};
}
