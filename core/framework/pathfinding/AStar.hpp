#pragma once

#include "graph.hpp"

#include <queue>
#include <list>

#include <glm/gtx/hash.hpp>

namespace framework
{

	template <typename Key, typename DataType>
	class AStar
	{

		static std::unordered_map<int, Key> reconstruct_path(
			Key start, Key goal,
			std::unordered_map<Key, Key> came_from)
		{
			std::unordered_map<int, Key> path;
			Key current = goal;
			if (came_from.find(goal) == came_from.end())
			{
				return path; // no path can be found
			}
			int counter = 0;
			while (current != start)
			{
				path[counter] = current;
				current = came_from[current];
				counter++;
			}
			path[counter] = start; // optional
			// std::reverse(path.begin(), path.end());
			return path;
		}

		class CompareDist
		{
		public:
			bool operator()(std::pair<float, Key> n1, std::pair<float, Key> n2)
			{
				return n1.first > n2.first;
			}
		};

	public:
		static float Heuristic(Key a, Key b)
		{
			return glm::distance(a, b);
		}

		static std::unordered_map<int, Key> traverse(NodeGraph<Key, DataType> graph, Key start, Key end)
		{
			std::unordered_map<Key, Key> path;
			using QElement = std::pair<float, Key>;

			std::priority_queue<QElement, std::vector<QElement>, CompareDist> frontier;
			std::unordered_map<Key, float> cost_so_far;
			cost_so_far.reserve(1500);
			path.reserve(1500);

			bool goalReached = false;

			path[start] = start;

			frontier.emplace(0.0f, start);
			cost_so_far[start] = 0.0f;

			while (!goalReached)
			{

				Key currentKey = frontier.top().second;
				frontier.pop();

				auto &neighbors = graph.getNeightbors(currentKey);

				for (auto &neighbor : neighbors)
				{
					// float currentWeigth = Heuristic(neighbor, end);
					// frontier.emplace(currentWeigth, neighbor);

					float new_cost = cost_so_far[currentKey] + graph.at(currentKey).data()->weight;
					if (!cost_so_far.contains(neighbor) || new_cost < cost_so_far[neighbor])
					{
						cost_so_far[neighbor] = new_cost;
						float priority = new_cost + Heuristic(neighbor, end);
						frontier.emplace(priority, neighbor);
						// came_from[neighbor] = currentKey;
						path[neighbor] = currentKey;
					}
				}

				// path.push_back(currentKey);

				if (currentKey == end)
					goalReached = true;
			}

			auto finalPath = reconstruct_path(start, end, path);

			return finalPath;
		}
	};

}