#pragma once

#include <vector>
#include <limits>
#include <algorithm>
#include <stdexcept>

class DistanceMatrix
{
public:
	DistanceMatrix(int num_elements) : p_num_elements(num_elements)
	{
		int num_permutations = num_elements * (num_elements - 1) / 2;
		p_calculated_distances.reserve(num_permutations);
		p_defined_distances.reserve(num_permutations);
		for (int i = 0; i < num_elements - 1; i++)
		{
			for (int j = i + 1; j < num_elements; j++)
			{
				p_calculated_distances.push_back(std::numeric_limits<float>::infinity());
				p_defined_distances.push_back(std::numeric_limits<float>::infinity());
			}
		}
		p_adjacency_list.reserve(num_elements);
		for (int i = 0; i < num_elements; i++)
		{
			p_adjacency_list.push_back(std::vector<int>());
		}
	}


	//Defines the distance between the two nodes, and then updates the distance map based on that distance
	void set_distance(int element1, int element2, float distance)
	{
		if (element1 < 0 || element1 >= p_num_elements)
		{
			throw(std::out_of_range("First element must be valid"));
		}
		if (element2 < 0 || element2 >= p_num_elements)
		{
			throw(std::out_of_range("Second element must be valid"));
		}
		if (element1 == element2)
		{
			return;
		}
		p_defined_distances[p_get_index(element1, element2)] = distance;
		p_adjacency_list[element1].push_back(element2);
		p_adjacency_list[element2].push_back(element1);
		float current_calculated_distance = p_calculated_distances[p_get_index(element1, element2)];
		//If the distance is smaller, the matrix can be updated instead of recalculated
		if (distance < current_calculated_distance)
		{
			p_set_calculated_distance(element1, element2, distance);
			for (int i = 0; i < p_num_elements - 1; i++)
			{
				for (int j = i + 1; j < p_num_elements; j++)
				{
					float distance_1 = get_calculated_distance(i, element1);
					float distance_2 = get_calculated_distance(j, element2);
					float current_distance_i_j = get_calculated_distance(i, j);
					float bridge_distance = distance_1 + distance_2 + distance;
					if (bridge_distance < current_distance_i_j)
					{
						p_set_calculated_distance(i, j, bridge_distance);
					}
				}
			}
		}
		else
		{
			p_fully_recalculate_distances();
		}
	}


	//Gets the defined distance between the two nodes (defaults to positive infinity)
	float get_defined_distance(int element1, int element2) const
	{
		if (element1 < 0 || element1 >= p_num_elements)
		{
			throw(std::out_of_range("First element must be valid"));
		}
		if (element2 < 0 || element2 >= p_num_elements)
		{
			throw(std::out_of_range("Second element must be valid"));
		}
		if (element1 == element2)
		{
			return 0;
		}
		return p_defined_distances[p_get_index(element1, element2)];
	}


	//Gets the calculated distance between the two nodes
	float get_calculated_distance(int element1, int element2) const
	{
		if (element1 < 0 || element1 >= p_num_elements)
		{
			throw(std::out_of_range("First element must be valid"));
		}
		if (element2 < 0 || element2 >= p_num_elements)
		{
			throw(std::out_of_range("Second element must be valid"));
		}
		if (element1 == element2)
		{
			return 0;
		}
		return p_calculated_distances[p_get_index(element1, element2)];
	}


	//Changes the size of the map
	//If it's larger, increases the size of the maps and adjacency lists
	//If it's smaller, decreases the size of the maps and removes adjacencies to larger elements
	void resize(int new_size)
	{
		int new_total_permutations = new_size * (new_size + 1) / 2;
		std::vector<float> new_defined_distances = std::vector<float>(new_total_permutations);
		std::vector<float> new_calculated_distances = std::vector<float>(new_total_permutations);
		for (int i = 0; i < p_num_elements - 1; i++)
		{
			int old_start = i * p_num_elements - i * (i + 1) / 2;
			int new_start = i * new_size - i * (i + 1) / 2;
			int size = (new_size > p_num_elements ? p_num_elements - (i + 1) : new_size - (i + 1));
			std::copy(p_defined_distances.begin() + old_start, p_defined_distances.begin() + old_start + size, new_defined_distances.begin() + new_start);
			std::copy(p_calculated_distances.begin() + old_start, p_calculated_distances.begin() + old_start + size, new_calculated_distances.begin() + new_start);
		}
		if (new_size > p_num_elements)
		{
			for (int i = 0; i < new_size - p_num_elements; i++)
			{
				p_adjacency_list.push_back(std::vector<int>());
			}
		}
		else {
			for (int i = 0; i < new_size; i++)
			{
				p_adjacency_list[i].erase(std::remove_if(p_adjacency_list[i].begin(), p_adjacency_list[i].end(), [&](const int& o) { return o >= new_size; }), p_adjacency_list[i].end());
			}
			p_adjacency_list.erase(p_adjacency_list.begin() + new_size, p_adjacency_list.end());
		}
		p_num_elements = new_size;
		p_defined_distances = new_defined_distances;
		p_calculated_distances = new_calculated_distances;
	}


	//Gets the shortest bath from the start to the end
	//Returns an empty vector if there is no path
	std::vector<int> get_shortest_path(int start, int end)
	{
		if (start == end)
		{
			return std::vector<int>{ start };
		}
		if (get_calculated_distance(start, end) == std::numeric_limits<float>::infinity()) {
			return std::vector<int>();
		}
		int best_neighbor = p_adjacency_list[end][0];
		float best_distance = get_calculated_distance(best_neighbor, start) + get_defined_distance(best_neighbor, end);
		for (int i = 1; i < p_adjacency_list[end].size(); i++)
		{
			float distance = get_calculated_distance(p_adjacency_list[end][i], start) + get_defined_distance(p_adjacency_list[end][i], end);
			if (distance < best_distance)
			{
				best_distance = distance;
				best_neighbor = p_adjacency_list[end][i];
			}
		}
		std::vector<int> path = get_shortest_path(start, best_neighbor);
		path.push_back(end);
		return path;
	}


	//Swaps the indices of the two elements so that adjacencies and distances to element1 now reference element2 and vice versa
	void swap_vertices(int element1, int element2)
	{
		if (element1 < 0 || element1 >= p_num_elements)
		{
			throw(std::out_of_range("First element must be valid"));
		}
		if (element2 < 0 || element2 >= p_num_elements)
		{
			throw(std::out_of_range("Second element must be valid"));
		}
		if (element1 == element2)
		{
			return;
		}
		for (int i = 0; i < p_num_elements; i++)
		{
			if (i == element1 || i == element2)
			{
				continue;
			}
			std::swap(p_calculated_distances[p_get_index(element1, i)], p_calculated_distances[p_get_index(element2, i)]);
			std::swap(p_defined_distances[p_get_index(element1, i)], p_defined_distances[p_get_index(element2, i)]);
		}
		std::vector<int> adjacent_to_element1 = p_adjacency_list[element1];
		for (int i = 0; i < adjacent_to_element1.size(); i++)
		{
			int adjacent_index = adjacent_to_element1[i];
			std::replace(p_adjacency_list[adjacent_index].begin(), p_adjacency_list[adjacent_index].end(), element1, element2);
		}
		std::vector<int> adjacent_to_element2 = p_adjacency_list[element2];
		for (int i = 0; i < adjacent_to_element2.size(); i++)
		{
			int adjacent_index = adjacent_to_element2[i];
			std::replace(p_adjacency_list[adjacent_index].begin(), p_adjacency_list[adjacent_index].end(), element2, element1);
		}
		std::swap(p_adjacency_list[element1], p_adjacency_list[element2]);
	}

	//Removes all edges involving the given vertex
	void reset_vertex(int vertex)
	{
		for (size_t i = 0; i < vertex; i++)
		{
			p_defined_distances[p_get_index(i, vertex)] = std::numeric_limits<float>::infinity();
			p_remove_from_adjacency_list(i, vertex);
		}
		for (int i = vertex + 1; i < p_num_elements; i++)
		{
			p_defined_distances[p_get_index(vertex, i)] = std::numeric_limits<float>::infinity();
			p_remove_from_adjacency_list(i, vertex);
		}
		p_adjacency_list[vertex].clear();
		p_fully_recalculate_distances();
	}

private:
	int p_num_elements;
	std::vector<float> p_defined_distances;
	std::vector<float> p_calculated_distances;
	std::vector<std::vector<int>> p_adjacency_list;


	//Get index of element pair in the vectors
	//Whenever possible, use element1 < element2 to avoid the need to swap
	//Don't use when elements are the same, instead return default
	int p_get_index(int element1, int element2) const
	{
		if (element1 > element2)
		{
			//Simple swap of two variables
			element1 = element1 + element2;
			element2 = element1 - element2;
			element1 = element1 - element2;
		}
		int start = element1 * p_num_elements - element1 * (element1 + 1) / 2;
		return start + element2 - element1 - 1;
	}


	//Updates the calculated distance between the two elements
	void p_set_calculated_distance(int element1, int element2, float distance)
	{
		//Calculated distance to same element is always 0
		if (element1 == element2)
		{
			return;
		}
		p_calculated_distances[p_get_index(element1, element2)] = distance;
	}

	//Fully recalculates the distances using the Floyd-Warshall algorithm
	void p_fully_recalculate_distances()
	{
		//k = 0 case, just enter the defined distances
		for (int i = 0; i < p_num_elements - 1; i++)
		{
			for (int j = 0; j < p_num_elements; j++)
			{
				p_set_calculated_distance(i, j, get_defined_distance(i, j));
			}
		}
		for (int k = 1; k < p_num_elements; k++)
		{
			for (int i = 0; i < p_num_elements - 1; i++)
			{
				for (int j = 0; j < p_num_elements; j++)
				{
					float bridge_distance = get_calculated_distance(i, k) + get_calculated_distance(k, j);
					if (get_calculated_distance(i, j) > bridge_distance)
					{
						p_set_calculated_distance(i, j, bridge_distance);
					}
				}
			}
		}
	}

	//Removes the element from the adjacency list at the given index
	void p_remove_from_adjacency_list(int adjacency_list_index, int element)
	{
		p_adjacency_list[adjacency_list_index].erase(std::remove(p_adjacency_list[adjacency_list_index].begin(), p_adjacency_list[adjacency_list_index].end(), element));
	}
};